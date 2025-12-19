use std::fs::File;
use std::io::Write;
use std::error::Error;
use std::env;
use std::fs;
use std::path::PathBuf;

const TEMPLATE_URL: &str = "https://raw.githubusercontent.com/Jamie-Poeffel/gito/refs/heads/main/templates";
const TEMPLATE_FILE: &str = "template.gito";


pub fn install_with_cached_template(template_name: &str) {
    println!("Installing {}", template_name);

    let template_url = format!("{}/{}/{}", TEMPLATE_URL, template_name, TEMPLATE_FILE);

    println!("template_url {}", template_url);

    let _ = download_template(&template_url, TEMPLATE_FILE);   
    
    
}

fn download_template(template_url: &str, output_file: &str) -> Result<(), Box<dyn Error>> {
    let dir = templates_dir()?;
    let path = dir.join(output_file);
    
    let response = reqwest::blocking::get(template_url)?;

    if !response.status().is_success() {
        return Err(format!("Download failed: {}", response.status()).into());
    }

    let bytes = response.bytes()?;

    let mut file = File::create(path)?;
    file.write_all(&bytes)?;

    Ok(())
}


fn templates_dir() -> Result<PathBuf, Box<dyn std::error::Error>> {
    let cwd = env::current_dir()?;

    let dir = cwd.join(".gito");

    fs::create_dir_all(&dir)?;

    Ok(dir)
}

