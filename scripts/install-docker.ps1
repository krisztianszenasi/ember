param(
    [string]$ImageName = "ember-cli",

    [ValidateSet("Debug", "Release")]
    [string]$Config = "Release",

    [ValidateSet("binary", "source")]
    [string]$PremakeInstallMode = "binary"
)

$ErrorActionPreference = "Stop"
$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent $ScriptRoot

Push-Location $RepoRoot

try {
    $DockerConfig = $Config.ToLowerInvariant()

    docker build `
        --build-arg "PROJECT_CONFIG=$DockerConfig" `
        --build-arg "PREMAKE_INSTALL_MODE=$PremakeInstallMode" `
        -t $ImageName `
        .
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}
finally {
    Pop-Location
}

Write-Host ""
Write-Host "Docker image built: $ImageName"
