mod file_utils;
mod parse_utils;
mod template_utils;
mod structs;
mod install;

use clap::Parser;
use std::path::Path;
use install::install_with_cached_template;

use file_utils::read_template_file;
use std::io::{self, Write};
use parse_utils::parse_template;
use structs::{Question, QuestionType};
use std::collections::HashMap;
use regex::Regex;
use crate::structs::Includes;
use std::process::Command;
use clap::Subcommand;

#[derive(Parser)]
#[command(name = "gito")]
#[command(about = "A Command-Line Tool Providing Git Templates and Additional Git-Related Utilities")]
struct Args {
    #[arg(short, long)]
    verbose: bool,

    #[arg(long)]
    template: bool,

    
    #[command(subcommand)]
    command: Option<Commands>,
}

#[derive(Subcommand)]
enum Commands {
    Install {
        name: String,
    },
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();

    if args.verbose {
        println!("Verbose mode enabled");
    }

    // --- TEMPLATE CHECK MODE ---
    if args.template {
        let lines = read_template_file(".gito/template.gito")?;

        if let Some((_, name)) = lines[0].split_once(':') {
            println!("You use the {} template", name.trim());
        }

        match parse_template(lines) {
            Ok(_) => println!("Template is correct ✅"),
            Err(e) => println!("Template error ❌: {}", e),
        }

        return Ok(());
    }

    match args.command {
        Some(Commands::Install { name }) => {
            install_with_cached_template(&name);

            return Ok(());
        }
        None => {
            println!("")
        }
    }

    // --- NORMAL MODE ---
    if !file_exists(Path::new(".gito/template.gito")) {
        println!("No Template installed");
        return Ok(());
    }

    let lines = read_template_file(".gito/template.gito")?;
    let template = parse_template(lines)?;

    println!("Using template: {}\n", template.name);

    let mut answers = std::collections::HashMap::new();

    let questions = template.questions;

    for question in questions.iter() {
        let answer = ask_question(question)?;
        answers.insert(question.id.clone(), answer);
    }

    let mut questions_map: HashMap<String, Question> = HashMap::new();
    for q in &questions {
        questions_map.insert(q.id.clone(), q.clone());
    }

    let build_string = template.build;
    let commit_msg = render_build(&build_string, &answers, &questions_map);

    println!("Your commit message: {}", commit_msg);

    println!("");

    print!("Do you want to commit this? [Y/n]: ");
    io::stdout().flush()?; 

    let mut input = String::new();
    io::stdin().read_line(&mut input)?;
    let input = input.trim(); 

    let commit = if input.is_empty() {
        true
    } else {
        matches!(input.to_lowercase().as_str(), "y" | "yes")
    };

    if commit {
        let status = Command::new("git")
        .args(["commit", "-m", &commit_msg])
        .status()
        .expect("Failed to execute git command");

        if status.success() {
            println!("Commit successful!");
        } else {
            println!("Commit failed with exit code: {}", status);
        }
    } else {
        println!("Commit cancelled.");
    }

    Ok(())
}

fn file_exists(path: &Path) -> bool {
    path.exists()
}

pub fn render_build(build: &str, answers: &HashMap<String, String>, questions: &HashMap<String, Question>) -> String {
    let re = Regex::new(r"\{([^}]+)\}").unwrap();

    let result = re.replace_all(build, |caps: &regex::Captures| {
        let key = &caps[1];
        let mut answer = answers.get(key).cloned().unwrap_or_default();

        let q = questions.get(key);
        
        if &q.unwrap().before != "" && answer != "" {
            answer = format!("{}{}", &q.unwrap().before, answer);
        }

        if let Some(Includes::Brackets) = &q.unwrap().includes {
            if !answer.is_empty() {
                answer = format!("({})", answer);
            }
        }

        answer
    });

    result.to_string()
}


fn ask_question(q: &Question) -> Result<String, Box<dyn std::error::Error>> {
    loop {
        let prompt = if let Some(options) = &q.options {
            let options_str: Vec<String> = options.iter()
                .map(|opt| {
                    if let Some(default) = &q.default {
                        if opt == default { opt.to_uppercase() } else { opt.clone() }
                    } else { opt.clone() }
                })
                .collect();

            format!("{} ({}): ", q.text, options_str.join(", "))
        } else {
            format!("{}: ", q.text)
        };


        print!("{}", prompt);
        io::stdout().flush()?;

        let mut input = String::new();
        io::stdin().read_line(&mut input)?;
        let input = input.trim();

        if input.is_empty() {
            if let Some(default) = &q.default {
                return Ok(default.clone());
            }
            if q.optional {
                return Ok(String::new());
            }
            println!("This field is required.");
            continue;
        }

        if let Some(options) = &q.options {
            if !options.contains(&input.to_string()) {
                println!("Allowed values: {:?}", options);
                continue;
            }
        }

        match q.kind {
            QuestionType::Number => {
                if input.parse::<u64>().is_err() {
                    println!("Must be a number");
                    continue;
                }
            }
            _ => {}
        }

        return Ok(input.to_string());
    }
}
