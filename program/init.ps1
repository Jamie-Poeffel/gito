[CmdletBinding()]
param (
    [Parameter()]
    [string]
    $Path,

    [Alias("d")][string]
    $useDefault
)
if ([string]::IsNullOrWhiteSpace($Path)) {
    $Path = Get-Location
}


$CONFIG_FILE = "C:/Windows/System32/ELW/gito/config.json"

function Get-Config {
    if (Test-Path $CONFIG_FILE) {
        $json = Get-Content $CONFIG_FILE -Raw
        return $json | ConvertFrom-Json
    }
    else {
        throw "Config file not found: $CONFIG_FILE"
    }
}

$conf = Get-Config


if ($useDefault -eq "true") {
    Write-Host $conf
    $projectName = Split-Path -Path $Path -Leaf
    $stage = $conf.stage
    $version = "1.0.0"
    $gitRepo = ""   
    $email = $conf.email
    $name = $conf.name
    $files = "false"
    $desc = "false"
}
else {
    $projectName = Read-Host "Project name ($([System.IO.Path]::GetFileName($Path)))"
    if ([string]::IsNullOrWhiteSpace($projectName)) {
        $projectName = Split-Path -Path $Path -Leaf
    }

    $version = Read-Host "Version (1.0.0)"
    if ([string]::IsNullOrWhiteSpace($version)) {
        $version = "1.0.0"
    }

    $gitRepo = Read-Host "Git repository (leave blank if none)"


    $email = Read-Host "Email ($($conf.email))"
    if ([string]::IsNullOrWhiteSpace($email)) {
        $email = $conf.email
    }

    $name = Read-Host "Name ($($conf.name))"
    if ([string]::IsNullOrWhiteSpace($name)) {
        $name = $conf.name
    }

    $files = Read-Host "Track files? (true/FALSE)"
    if ([string]::IsNullOrWhiteSpace($files)) {
        $files = "false"
    }

    $desc = Read-Host "Description? (true/FALSE)"
    if ([string]::IsNullOrWhiteSpace($desc)) {
        $desc = "false"
    }
}

$config = @{
    "projectName" = $projectName
    "version"     = $version
    "stage"       = $stage 
    "gitRepo"     = $gitRepo
    "email"       = $email
    "name"        = $name
    "files"       = $files
    "desc"        = $desc
}

$gitoDir = Join-Path -Path $Path -ChildPath ".gito"
if (-not (Test-Path -Path $gitoDir)) {
    New-Item -Path $gitoDir -ItemType Directory -Force | Out-Null
    (Get-Item $gitoDir).Attributes += 'Hidden'
}

$configPath = Join-Path -Path $gitoDir -ChildPath "config.json"
$config | ConvertTo-Json -Depth 5 | Set-Content -Path $configPath -Encoding UTF8