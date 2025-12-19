use directories::ProjectDirs;
use std::path::PathBuf;
use std::fs;
use std::fs::OpenOptions;
use std::io::Write;

const APP_NAME: &str = "Gito";
const APP_ORG: &str = "";
const HISTORY_FILE: &str = "history.txt";
const APP_QUALIFIER: &str = "com";

pub fn get_history_file_path() -> Option<PathBuf> {
    if let Some(proj_dirs) = ProjectDirs::from(APP_QUALIFIER, APP_ORG, APP_NAME) {
        let cache_dir = proj_dirs.cache_dir();
        
        if !cache_dir.exists() {
            if let Err(e) = fs::create_dir_all(cache_dir) {
                eprintln!("Failed to create cache directory: {}", e);
                return None;
            }
        }
        
        return Some(cache_dir.join(HISTORY_FILE));
    }
    None
}

pub fn append_to_history(entry: &str) -> std::io::Result<()> {
    if let Some(path) = get_history_file_path() {
        let mut file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(path)?;

        writeln!(file, "{}", entry)?;
    }
    Ok(())
}

