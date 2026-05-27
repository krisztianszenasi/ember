param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Arguments
)

$ErrorActionPreference = "Stop"
$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent $ScriptRoot
$VenvPython = Join-Path $RepoRoot ".venv\Scripts\python.exe"
$PythonPrefixArgs = @()

if (Test-Path $VenvPython) {
    $PythonExe = $VenvPython
} else {
    $PythonCommand = Get-Command py -ErrorAction SilentlyContinue
    if ($null -ne $PythonCommand) {
        $PythonExe = "py"
        $PythonPrefixArgs = @("-3")
    } else {
        $PythonCommand = Get-Command python -ErrorAction SilentlyContinue
        if ($null -eq $PythonCommand) {
            Write-Error "Python 3 is required. Run scripts/setup-python.ps1 first."
        }
        $PythonExe = "python"
        $PythonPrefixArgs = @()
    }
}

Push-Location $RepoRoot

try {
    & $PythonExe @PythonPrefixArgs "tests/end2end/run.py" @Arguments
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}
finally {
    Pop-Location
}
