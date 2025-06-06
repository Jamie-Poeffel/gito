# Path to the JSON file
$version_file = "C:\Windows\System32\ELW\gito\version.json"

# Read the local JSON file and extract the version
$vers = (Get-Content $version_file | ConvertFrom-Json).version.Trim()

# Define the URL of the online JSON file
$jsonUrl = "https://raw.githubusercontent.com/Jamie-Poeffel/gito/main/program/version.json"

# Fetch online version JSON data
try {
    $response = Invoke-RestMethod -Uri $jsonUrl
    $onvers = $response.version.Trim()
}
catch {
    Write-Host "Failed to fetch online version."
    exit 1
}

Write-Host "gito $vers"

# Compare versions only if $onvers is set
if ($null -ne $onvers -and $vers -ne $onvers) {
    Write-Host "Update to new Version $onvers with command: gito -u / gito --update / elx i"
}
