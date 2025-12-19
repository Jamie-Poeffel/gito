use std::fs::File;
use std::io::{Read, Write};
use std::env;
use std::fs;
use std::path::PathBuf;
use indicatif::{ProgressBar, ProgressStyle};

const TEMPLATE_URL: &str = "https://raw.githubusercontent.com/Jamie-Poeffel/gito/refs/heads/main/templates";
const TEMPLATE_FILE: &str = "template.gito";


pub fn install_with_cached_template(template_name: &str) {
    let template_url = format!("{}/{}/{}", TEMPLATE_URL, template_name, TEMPLATE_FILE);

    let _ = download_with_progress(&template_url, TEMPLATE_FILE);   
}

fn download_with_progress(
    url: &str,
    output_file: &str,
) -> Result<(), Box<dyn std::error::Error>> {    
    let dir = templates_dir()?;
    let path = dir.join(output_file);
    let mut response = reqwest::blocking::get(url)?;

    let total_size = response
        .content_length()
        .ok_or("Failed to get content length")?;

    let pb = ProgressBar::new(total_size);

    pb.set_style(
        ProgressStyle::with_template(
            "[{elapsed_precise}] [{bar:40.cyan/blue}] \
             {bytes}/{total_bytes} ({bytes_per_sec}, ETA {eta})",
        )?
        .progress_chars("#>-"),
    );


    let mut file = File::create(path)?;
    let mut downloaded: u64 = 0;
    let mut buffer = [0; 8192];

    loop {
        let n = response.read(&mut buffer)?;
        if n == 0 {
            break;
        }
        file.write_all(&buffer[..n])?;
        downloaded += n as u64;
        pb.set_position(downloaded);
    }

    pb.finish_with_message("Download complete");
    Ok(())
}



fn templates_dir() -> Result<PathBuf, Box<dyn std::error::Error>> {
    let cwd = env::current_dir()?;

    let dir = cwd.join(".gito");

    fs::create_dir_all(&dir)?;

    Ok(dir)
}

