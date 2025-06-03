#!/usr/bin/env python3
"""
Unified Git Commit Message Template

This script builds a single text commit message that includes:
  - The commit type (e.g., fix, feature, docs, etc.)
  - A short commit summary
  - A multi-line description
  - A list of staged (changed) files

The final message is a one-block text that you can directly use with Git.
"""

import subprocess
import sys
import os
import json

CONFIG_FILE = "C:/Windows/System32/ELW/gito/config.json"
LOCAL_CONFIG_FILE = os.path.join(os.getcwd(), ".gito", "config.json")

def load_commit_config(config_file):
    """Lädt die Commit-Konfiguration aus einer JSON-Datei (auch mit UTF-8 BOM)."""
    if not os.path.exists(config_file):
        print(f"Fehler: {config_file} nicht gefunden.")
        sys.exit(1)

    if os.path.getsize(config_file) == 0:
        print(f"Fehler: {config_file} ist leer.")
        sys.exit(1)

    try:
        with open(config_file, "r", encoding="utf-8-sig") as f: 
            config = json.load(f)
    except json.JSONDecodeError as e:
        print(f"Fehler beim Parsen der JSON-Datei:\n→ {e}")
        sys.exit(1)

    return config


def get_files():
    result = subprocess.run(
        ['git', 'status', '--porcelain'],
        capture_output=True,
        text=True
    )
    lines = result.stdout.strip().split('\n')
    files = []
    for line in lines:
        if line:
            status = line[:2].strip()
            rest = line[3:].strip()
            files.append((status, rest))
    return files

def get_staged_files():
    """Returns a list of files that are staged for commit."""
    try:
        result = subprocess.run(
            ["git", "diff", "--cached", "--name-only"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            check=True
        )
        files = [line.strip() for line in result.stdout.splitlines() if line.strip()]
        return files
    except subprocess.CalledProcessError as error:
        sys.stderr.write("Error retrieving staged files:\n" + error.stderr)
        return []

def execute_git_add_command(config):
    """
    Prüft, ob ein 'git_add_command' in der Konfiguration vorhanden ist.
    Wenn ja, wird der Befehl ausgeführt, ansonsten wird er übersprungen.
    """
    git_add_command = config.get("stage")
    if git_add_command:
        try:
            # Aufteilen in Argumente, um subprocess.run zu nutzen.
            subprocess.run(git_add_command.split(), check=True)
        except subprocess.CalledProcessError as error:
            print("Fehler beim Ausführen des git add-Befehls:", error)
    else:
        print("Kein git add-Befehl in der Konfiguration gefunden; überspringe.")

def get_multiline_input(prompt="Enter commit description (finish with a single '.' on a new line):"):
    """Collects multi-line input until a single '.' is entered on a line."""
    print(prompt)
    lines = []
    while True:
        line = input()
        if line.strip() == '.':
            break
        lines.append(line)
    return "\n".join(lines)

def main():

    if os.path.exists(LOCAL_CONFIG_FILE):
        config = load_commit_config(LOCAL_CONFIG_FILE)
    else:
        config = load_commit_config(CONFIG_FILE)
    
    execute_git_add_command(config)

    files = get_files()
    if not files:
        print("Keine gestagten Änderungen gefunden.")
        return

    try:
        commit_type = input("Commit type (e.g., fix, feature, docs, refactor): ").strip()
        summary = input("Commit summary: ").strip()
        if config.get("desc") == "true":
            description = get_multiline_input("Enter commit description (finish input with '.' on a new line):")
        
        if config.get("files") == "true":
            staged_files = get_staged_files()
            files_str = ", ".join(staged_files) if staged_files else "None"
        
        issue_or_pr = input("issue number or PR number (#5) (OPTIONAL): ").strip()
        if issue_or_pr:
            if not issue_or_pr.startswith("#"):
                issue_or_pr = "#" + issue_or_pr
                # Check if the rest is a whole number
                rest = issue_or_pr[1:]
                if not rest.isdigit():
                    issue_or_pr = ""
        
        # Construct a unified commit message as one text block
        commit_message = f"[{commit_type}] {summary}"

        if issue_or_pr:
            commit_message += f" ({issue_or_pr})"

        if config.get("files") == "true":
            commit_message += f" | Files: {files_str}"
        if config.get("desc") == "true":
            commit_message += f"\n{description}"
    
    except KeyboardInterrupt:
        print("\nCommit aborted.")
        sys.exit(1)
    
    print("\nGenerated Commit Message:\n")
    print(commit_message)
    
    confirmation = input("\nDo you want to commit with this message? [c]: ").strip().lower()
    if confirmation == 'c':
        subprocess.run(["git", "commit", "-m", commit_message])
        print("Commit created!")
    else:
        print("Commit aborted.")

if __name__ == "__main__":
    main()
