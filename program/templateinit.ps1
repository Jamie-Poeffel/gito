[CmdletBinding()]
param (
    [Parameter()]
    [string]
    $Path
)

if ([string]::IsNullOrWhiteSpace($Path)) {
    $Path = Get-Location
}

$DEFAULT_TEMPLATE = "https://raw.githubusercontent.com/Jamie-Poeffel/Gito/refs/heads/main/template/template.gito"

Write-Host "Select template type:"
Write-Host "1. Empty"
Write-Host "2. Default"
$selection = Read-Host "Enter 1 or 2"
$outputFileName = Join-Path $Path ".gito/template.gito"


switch ($selection) {
    "1" {
        Write-Host "You selected: Empty template"

        New-Item -ItemType File -Path $outputFileName -Force | Out-Null

        Write-Host "Created empty template at $outputFileName"
    }
    "2" {
        Write-Host "You selected: Default template"

        $rawUrl = $DEFAULT_TEMPLATE

        Invoke-WebRequest -Uri $rawUrl -OutFile $outputFileName

        Write-Host "Downloaded to $outputFileName"
    }
    default {
        Write-Host "Invalid selection."
        exit 1
    }
}
