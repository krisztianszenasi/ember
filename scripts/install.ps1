param(
    [ValidateSet("Debug", "Release")]
    [string]$Config = "Release",

    [Parameter(Mandatory = $true)]
    [ValidateSet("vs2022", "vs2026")]
    [string]$VsAction,

    [ValidateSet("x64", "ARM64")]
    [string]$Platform = "x64",

    [string]$InstallDir = "$env:USERPROFILE\.ember\bin"
)

$ErrorActionPreference = "Stop"
$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent $ScriptRoot

function Show-Install-Hint {
    param([string]$Name)

    Write-Host ""
    Write-Host "Missing required command: $Name"
    Write-Host ""

    switch ($Name) {
        "premake5" {
            Write-Host "Install Premake 5:"
            Write-Host "  1. Download Premake from:"
            Write-Host "     https://premake.github.io/download/"
            Write-Host "  2. Put premake5.exe somewhere like:"
            Write-Host "     C:\Tools\premake5.exe"
            Write-Host "  3. Add the containing folder to PATH:"
            Write-Host "     C:\Tools"
            Write-Host ""
            Write-Host "After that, open a new PowerShell window and check:"
            Write-Host "  premake5 --version"
        }
        "win_bison" {
            Write-Host "Install WinFlexBison:"
            Write-Host "  With Chocolatey:"
            Write-Host "    choco install winflexbison3"
            Write-Host ""
            Write-Host "  Or download it manually, then add its folder to PATH."
            Write-Host ""
            Write-Host "After that, open a new PowerShell window and check:"
            Write-Host "  win_bison --version"
        }
        "win_flex" {
            Write-Host "Install WinFlexBison:"
            Write-Host "  With Chocolatey:"
            Write-Host "    choco install winflexbison3"
            Write-Host ""
            Write-Host "  Or download it manually, then add its folder to PATH."
            Write-Host ""
            Write-Host "After that, open a new PowerShell window and check:"
            Write-Host "  win_flex --version"
        }
        "msbuild" {
            Write-Host "Install Visual Studio Build Tools or Visual Studio Community."
            Write-Host ""
            Write-Host "During installation, select:"
            Write-Host "  Desktop development with C++"
            Write-Host ""
            Write-Host "Then run this script from:"
            Write-Host "  Developer PowerShell for Visual Studio"
            Write-Host ""
            Write-Host "Check with:"
            Write-Host "  msbuild -version"
            Write-Host "  cl"
        }
        default {
            Write-Host "Install $Name and make sure it is available on PATH."
        }
    }

    Write-Host ""
}

function Require-Command {
    param([string]$Name)

    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        Show-Install-Hint $Name
        Write-Error "Missing required command: $Name"
    }
}

function Invoke-Native {
    param(
        [string]$Command,
        [string[]]$Arguments
    )

    & $Command @Arguments

    if ($LASTEXITCODE -ne 0) {
        Write-Error "$Command failed with exit code $LASTEXITCODE"
    }
}

function Find-Solution {
    $SolutionCandidates =
        if ($VsAction -eq "vs2026") {
            @(
                (Join-Path $RepoRoot "build\ember.slnx"),
                (Join-Path $RepoRoot "build\Ember.slnx"),
                (Join-Path $RepoRoot "build\ember.sln"),
                (Join-Path $RepoRoot "build\Ember.sln")
            )
        } else {
            @(
                (Join-Path $RepoRoot "build\ember.sln"),
                (Join-Path $RepoRoot "build\Ember.sln"),
                (Join-Path $RepoRoot "build\ember.slnx"),
                (Join-Path $RepoRoot "build\Ember.slnx")
            )
        }

    foreach ($Candidate in $SolutionCandidates) {
        if (Test-Path $Candidate) {
            return $Candidate
        }
    }

    Write-Error "Visual Studio solution was not found under build\"
}

function Find-TestProject {
    $ProjectCandidates = @(
        (Join-Path $RepoRoot "build\ember-test\ember_test.vcxproj")
    )

    foreach ($Candidate in $ProjectCandidates) {
        if (Test-Path $Candidate) {
            return $Candidate
        }
    }

    Write-Error "ember_test project was not found under build\ember-test\"
}

Write-Host "Checking dependencies..."
Require-Command premake5
Require-Command win_bison
Require-Command win_flex
Require-Command msbuild

function Require-ThirdParty {
    $ThirdPartyDir = Join-Path $RepoRoot "third_party"

    if (-not (Test-Path $ThirdPartyDir)) {
        Write-Error "third_party\ was not found. Initialize submodules first: git submodule update --init --recursive"
    }

    $Entries = Get-ChildItem -Path $ThirdPartyDir -Force
    if ($Entries.Count -eq 0) {
        Write-Error "third_party\ is empty. Initialize submodules first: git submodule update --init --recursive"
    }
}

Write-Host "Using configuration: $Config"
Write-Host "Using Visual Studio action: $VsAction"
Write-Host "Using platform: $Platform"
Write-Host "Install directory: $InstallDir"

Push-Location $RepoRoot

try {
    Require-ThirdParty

    Write-Host ""
    Write-Host "Generating Visual Studio build files..."
    Invoke-Native "premake5" @($VsAction)

    $SolutionPath = Find-Solution
    Write-Host "Using solution: $SolutionPath"

    Write-Host ""
    Write-Host "Building Ember ($Config)..."
    Invoke-Native "msbuild" @($SolutionPath, "/p:Configuration=$Config", "/p:Platform=$Platform")

    $TestProjectPath = Find-TestProject
    Write-Host ""
    Write-Host "Building ember_test ($Config)..."
    Invoke-Native "msbuild" @($TestProjectPath, "/p:Configuration=$Config", "/p:Platform=$Platform")

    $BuildDir = Join-Path $RepoRoot "bin\$Config"
    if (-not (Test-Path $BuildDir)) {
        Write-Error "Build output directory not found: $BuildDir"
    }

    Write-Host ""
    Write-Host "Installing to $InstallDir..."
    New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null

    $Executables = @(
        "ember-cli.exe"
    )

    foreach ($Exe in $Executables) {
        $Source = Join-Path $BuildDir $Exe

        if (Test-Path $Source) {
            Copy-Item $Source $InstallDir -Force
            Write-Host "Installed $Exe"
        } else {
            Write-Error "Executable not found: $Source"
        }
    }
}
finally {
    Pop-Location
}

Write-Host ""
Write-Host "Done."
Write-Host "Installed to: $InstallDir"
Write-Host ""
Write-Host "Add this directory to PATH if it is not already there:"
Write-Host "  $InstallDir"
