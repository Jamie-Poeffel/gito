[CmdletBinding()]
param (
    [Parameter(Mandatory = $true)]
    [string] $Type,

    [string] $Value
)

# Ensure admin rights
if (-NOT ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    $argsList = "-ExecutionPolicy Bypass -NoExit -File `"$PSCommandPath`" -Type `"$Type`""
    if ($Value) {
        $argsList += " -Value `"$Value`""
    }
    Start-Process powershell -Verb RunAs -ArgumentList $argsList
    exit
}

# Config helpers
$CONFIG_FILE = "C:/Windows/System32/ELW/gito/config.json"

function Get-Config {
    if (Test-Path $CONFIG_FILE) {
        $json = Get-Content $CONFIG_FILE -Raw
        return $json | ConvertFrom-Json
    } else {
        throw "Config file not found: $CONFIG_FILE"
    }
}

function Set-Config($config) {
    $config | ConvertTo-Json -Depth 10 | Set-Content $CONFIG_FILE
}

switch ($Type.ToLower()) {

    "stage" {
        Write-Host "Entered stage block"
        $config = Get-Config

        $validStages = @("git add .", "git add *", "")
        Write-Host "Select a stage command:"
        for ($i = 0; $i -lt $validStages.Count; $i++) {
            Write-Host "[$i] $($validStages[$i])"
        }

        $selection = Read-Host "Enter number (0-$($validStages.Count - 1))"
        if ($selection -match '^\d+$' -and [int]$selection -ge 0 -and [int]$selection -lt $validStages.Count) {
            $selectedValue = $validStages[$selection]
            $config.stage = $selectedValue
            Set-Config $config
            Write-Host "Stage set to '$selectedValue'"
            exit
        } else {
            Write-Warning "Invalid selection. Please enter a number between 0 and $($validStages.Count - 1)."
            Pause
        }

        exit
    }

    "user.email" {
        $config = Get-Config

        if ($Value -match '^[\w\.\-]+@([\w\-]+\.)+[a-zA-Z]{2,}$') {
            $config.email = $Value
            Set-Config $config
            Write-Host "User email set to '$Value'"
            exit
        } else {
            Write-Warning "Invalid email address: $Value"
            Pause
        }

        exit
    }

    "user.name" {
        $config = Get-Config

        if ($Value -ne "") {
            $config.name = $Value
            Set-Config $config
            Write-Host "User name set to '$Value'"
        } else {
            Write-Warning "Name cannot be empty."
            Pause
        }

        exit
    }

    "desc" {
        $config = Get-Config

        $valid = @("true", "false")
        if ($valid -contains $Value.ToLower()) {
            $config.desc = $Value.ToLower()
            Set-Config $config
            Write-Host "Desc set to '$Value'"
        } else {
            Write-Warning "Invalid value: $Value. Valid values are: $($valid -join ', ')"
            Pause
        }

        exit
    }

    "files" {
        $config = Get-Config

        $valid = @("true", "false")
        if ($valid -contains $Value.ToLower()) {
            $config.files = $Value.ToLower()
            Set-Config $config
            Write-Host "Files flag set to '$Value'"
        } else {
            Write-Warning "Invalid value: $Value. Valid values are: $($valid -join ', ')"
            Pause
        }

        exit    
    }

    default {
        Write-Warning "Unknown type: $Type"
        Pause
        exit
    }
}
