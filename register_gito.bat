@echo off
SETLOCAL

echo Registering .gito file extension...

:: Associate .gito with a new file type
reg add "HKCR\.gito" /ve /d "GITO.File" /f

:: Set a friendly name
reg add "HKCR\GITO.File" /ve /d "GITO Template File" /f

:: Copy the .yaml icon if it exists
reg query "HKCR\.yaml" >nul 2>&1
IF %ERRORLEVEL% EQU 0 (
    for /f "tokens=2*" %%a in ('reg query "HKCR\.yaml" /ve 2^>nul') do (
        set YAMLCODE=%%b
    )
    reg query "HKCR\%YAMLCODE%\DefaultIcon" >nul 2>&1
    IF %ERRORLEVEL% EQU 0 (
        for /f "tokens=2*" %%a in ('reg query "HKCR\%YAMLCODE%\DefaultIcon" /ve 2^>nul') do (
            set YAMLICON=%%b
        )
        reg add "HKCR\GITO.File\DefaultIcon" /ve /d "%YAMLICON%" /f
        echo Icon copied from .yaml
    ) ELSE (
        echo Could not find .yaml DefaultIcon. Skipping icon copy.
    )
) ELSE (
    echo .yaml not registered. Skipping icon copy.
)

reg add "HKCR\GITO.File\shell\open\command" /ve /d "\"%LocalAppData%\Programs\Microsoft VS Code\Code.exe\" \"%%1\"" /f

echo Done. You may need to restart Explorer for changes to take effect.

call "%~dp0move.bat"
