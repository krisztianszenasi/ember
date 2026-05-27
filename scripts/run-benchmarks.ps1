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
    }
}

$ArgumentList = New-Object System.Collections.Generic.List[string]
$HasEmberCli = $false

foreach ($Argument in $Arguments) {
    if ($Argument -eq "--ember-cli") {
        $HasEmberCli = $true
        break
    }
}

if (-not $HasEmberCli) {
    $ReleaseCli = Join-Path $RepoRoot "bin\Release\ember-cli.exe"
    $DebugCli = Join-Path $RepoRoot "bin\Debug\ember-cli.exe"

    if (Test-Path $ReleaseCli) {
        $ArgumentList.Add("--ember-cli")
        $ArgumentList.Add($ReleaseCli)
    } elseif (Test-Path $DebugCli) {
        $ArgumentList.Add("--ember-cli")
        $ArgumentList.Add($DebugCli)
    }
}

foreach ($Argument in $Arguments) {
    $ArgumentList.Add($Argument)
}

Push-Location $RepoRoot

try {
    & $PythonExe @PythonPrefixArgs "docs/benchmarks/benchmark.py" @ArgumentList
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}
finally {
    Pop-Location
}
