import argparse
import subprocess

def run_powershell(script, *args):
    cmd = ["powershell", "-ExecutionPolicy", "Bypass", "-File", script] + list(args)
    subprocess.run(cmd)

def main():
    parser = argparse.ArgumentParser(
        description="Gito: A starting point CLI tool for managing templates and configurations."
    )
    subparsers = parser.add_subparsers(dest="command")

    parser.add_argument('--version', '-v', action='store_true', help='Show version information and exit.')


    # --delete
    subparsers.add_parser('--delete', help="Uninstall Gito.")

    # init
    parser_init = subparsers.add_parser('init', help="Initialize a new project or template.")
    parser_init.add_argument('template', nargs='?', default=None, help="Specify 'template' to use the template initializer.")
    parser_init.add_argument('-y', action='store_true', help="Automatic yes to prompts.")

    # fast
    subparsers.add_parser('fast', help="Run the main executable quickly after checking version.")

    # a
    subparsers.add_parser('a', help="Run the AI executable after checking version.")

    # --config / -c
    parser_config = subparsers.add_parser('--config', help="Configure Gito settings.")
    parser_config.add_argument('type', help="Type of configuration.")
    parser_config.add_argument('value', help="Value for the configuration.")
    parser_config2 = subparsers.add_parser('-c', help="Configure Gito settings (short flag).")
    parser_config2.add_argument('type', help="Type of configuration.")
    parser_config2.add_argument('value', help="Value for the configuration.")

    # --help
    subparsers.add_parser('--help', help="Show help information.")

    args = parser.parse_args()

    try:
        if args.command == '--delete':
            run_powershell(r"C:\Windows\System32\ELW\gito\uninstall.ps1")
        elif args.version:
            run_powershell(r"C:\Windows\System32\ELW\gito\version.ps1")
        elif args.command == 'init':
            if args.template == "template":
                if args.y:
                    run_powershell(r"C:\Windows\System32\ELW\gito\templateinit.ps1", "-d", "true")
                else:
                    run_powershell(r"C:\Windows\System32\ELW\gito\templateinit.ps1")
            else:
                if args.y:
                    run_powershell(r"C:\Windows\System32\ELW\gito\init.ps1", "-d", "true")
                else:
                    run_powershell(r"C:\Windows\System32\ELW\gito\init.ps1")
        elif args.command == 'fast':
            run_powershell(r"C:\Windows\System32\ELW\gito\checkversion.ps1")
            subprocess.run([r"C:\Windows\System32\ELW\gito\main.exe"])
        elif args.command == 'a':
            run_powershell(r"C:\Windows\System32\ELW\gito\checkversion.ps1")
            subprocess.run([r"C:\Windows\System32\ELW\gito\ai.exe"])
        elif args.command in ('--config', '-c'):
            run_powershell(r"C:\Windows\System32\ELW\gito\config.ps1", "-Type", args.type, "-Value", args.value)
        elif args.command == '--help':
            run_powershell(r"C:\Windows\System32\ELW\gito\help.ps1")
        else:
            run_powershell(r"C:\Windows\System32\ELW\gito\checkversion.ps1")
            subprocess.run([r"C:\Windows\System32\ELW\gito\template.exe"])
    except KeyboardInterrupt:
        print("\nOperation cancelled by user.")

if __name__ == "__main__":
    main()