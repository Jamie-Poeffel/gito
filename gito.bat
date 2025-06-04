@echo off

if "%1"=="--delete" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\uninstall.ps1"
) else if "%1"=="init" (
    if "%2"=="template" (
        if "%3"=="-y" (
            powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\templateinit.ps1" -d "true"
        ) else (
            powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\templateinit.ps1"
        )
    ) else (
        if "%2"=="-y" (
            powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\init.ps1" -d "true"
        ) else (
            powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\init.ps1"
        )
    )
) else if "%1"=="-v" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\version.ps1"
) else if "%1"=="--version" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\version.ps1"
) else if "%1"=="fast" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\checkversion.ps1"
    C:\Windows\System32\ELW\gito\main.exe
) else if "%1"=="a" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\checkversion.ps1"
    C:\Windows\System32\ELW\gito\ai.exe
) else if "%1"=="--config" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\config.ps1" -Type "%2" -Value "%~3"
) else if "%1"=="-c" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\config.ps1" -Type "%2" -Value "%~3"
) else if "%1"=="--help" (
    powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\help.ps1"
) else (
    if exist "%~dp0.gito/template.gito" (
        powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\checkversion.ps1"
        C:\Windows\System32\ELW\gito\template.exe
    ) else (
        powershell -ExecutionPolicy Bypass -File "C:\Windows\System32\ELW\gito\checkversion.ps1"
        C:\Windows\System32\ELW\gito\more.exe
    )
)
