param(
    [string]$VenvDir = ".venv"
)

$ErrorActionPreference = "Stop"
$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent $ScriptRoot

if ([System.IO.Path]::IsPathRooted($VenvDir)) {
    $ResolvedVenvDir = $VenvDir
} else {
    $ResolvedVenvDir = Join-Path $RepoRoot $VenvDir
}

$PythonCommand = Get-Command py -ErrorAction SilentlyContinue
if ($null -ne $PythonCommand) {
    $PythonExe = "py"
    $PythonArgs = @("-3")
} else {
    $PythonCommand = Get-Command python -ErrorAction SilentlyContinue
    if ($null -eq $PythonCommand) {
        Write-Error "Python 3 is required but was not found on PATH."
    }
    $PythonExe = "python"
    $PythonArgs = @()
}

Write-Host "Creating virtual environment at $ResolvedVenvDir"
& $PythonExe @PythonArgs -m venv $ResolvedVenvDir
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to create virtual environment."
}

$VenvPython = Join-Path $ResolvedVenvDir "Scripts\python.exe"

Write-Host "Upgrading pip"
& $VenvPython -m pip install --upgrade pip
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to upgrade pip."
}

$RequirementFiles = @(
    (Join-Path $RepoRoot "tests\end2end\requirements.txt"),
    (Join-Path $RepoRoot "docs\benchmarks\requirements.txt")
)

foreach ($RequirementFile in $RequirementFiles) {
    if (Test-Path $RequirementFile) {
        Write-Host "Installing dependencies from $RequirementFile"
        & $VenvPython -m pip install -r $RequirementFile
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to install dependencies from $RequirementFile"
        }
    }
}

Write-Host ""
Write-Host "Python environment ready."
Write-Host "Activate with:"
Write-Host "  $ResolvedVenvDir\Scripts\Activate.ps1"
