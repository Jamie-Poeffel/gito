use std::fs;

pub fn read_template_file(path: &str) -> Result<Vec<String>, String> {
    let content = fs::read_to_string(path)
        .map_err(|e| e.to_string())?;

    Ok(content
        .lines()
        .map(|l| l.to_string())
        .collect())
}

pub fn join_continued_lines(lines: Vec<String>) -> Vec<String> {
    let mut result = Vec::new();
    let mut current = String::new();

    for line in lines {
        let trimmed = line.trim_end();

        if trimmed.ends_with('/') {
            current.push_str(trimmed.trim_end_matches('/'));
            current.push(' ');
        } else {
            current.push_str(trimmed);
            result.push(current.trim().to_string());
            current.clear();
        }
    }

    if !current.is_empty() {
        result.push(current.trim().to_string());
    }

    result
}
