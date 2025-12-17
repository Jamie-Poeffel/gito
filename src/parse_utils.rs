use crate::file_utils::join_continued_lines;
use crate::structs::*;
use crate::template_utils::Key;

#[derive(Default)]
struct QuestionBuilder {
    id: Option<String>,
    text: Option<String>,
    kind: Option<QuestionType>,
    options: Vec<String>,
    default: Option<String>,
    before: Option<String>,
    optional: bool,
    includes: Option<Includes>,
}

impl QuestionBuilder {
    fn build(self) -> Result<Question, String> {
        Ok(Question {
            id: self.id.ok_or("Missing ID")?,
            text: self.text.ok_or("Missing QUESTION")?,
            kind: self.kind.ok_or("Missing TYPE")?,
            options: if self.options.is_empty() { None } else { Some(self.options) },
            before: self.before.clone().unwrap_or_default(),
            default: self.default,
            optional: self.optional,
            includes: self.includes,
        })
    }
}

pub fn parse_template(lines: Vec<String>) -> Result<Template, String> {
    let lines = join_continued_lines(lines);

    let mut name = None;
    let mut build = None;
    let mut questions = Vec::new();
    let mut current = QuestionBuilder::default();

    for line in lines {
        if line == "" {
            continue
        }

        let (key, value) = line
            .split_once(':')
            .ok_or("Invalid format")?;

        match Key::from_str(key.trim())? {
            Key::NAME => name = Some(strip(value)),
            Key::QUESTION => {
                if current.text.is_some() {
                    questions.push(current.build()?);
                    current = QuestionBuilder::default();
                }
                current.text = Some(strip(value));
            }
            Key::TYPE => {
                current.kind = Some(match value.trim() {
                    "string" => QuestionType::String,
                    "number" => QuestionType::Number,
                    _ => return Err("Invalid TYPE".into()),
                });
            }
            Key::ID => current.id = Some(value.trim().to_string()),
            Key::DEFAULT => current.default = Some(strip(value)),
            Key::OPTIONAL => current.optional = value.trim() == "True",
            Key::BEFORE => current.before = Some(value.trim().to_string()),
            Key::INCLUDES => current.includes = Some(Includes::Brackets),
            Key::OPTIONS => {
                current.options.clear();

                let value = value.trim();
                if !value.is_empty() {
                    for cap in regex::Regex::new(r#""([^"]+)""#).unwrap().captures_iter(value) {
                        current.options.push(cap[1].to_string());
                    }
                }
            }
            Key::BUILD => build = Some(value.trim().to_string()),
        }
    }

    questions.push(current.build()?);

    Ok(Template {
        name: name.ok_or("Missing NAME")?,
        questions,
        build: build.ok_or("Missing BUILD")?,
    })
}

fn strip(s: &str) -> String {
    s.trim().trim_matches('"').to_string()
}
