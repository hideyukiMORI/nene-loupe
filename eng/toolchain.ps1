Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$versions = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'tool-versions.json') -Raw | ConvertFrom-Json
if ($PSVersionTable.PSVersion -lt [version]$versions.powershellMinimum) { throw 'QLT-011: PowerShell is too old.' }
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$installation = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $installation) { throw 'QLT-011: MSVC was not found.' }
Import-Module (Join-Path $installation 'Common7/Tools/Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell -VsInstallPath $installation -SkipAutomaticLocation -DevCmdArguments "-arch=amd64 -host_arch=amd64 -vcvars_ver=$($versions.msvcToolset)" | Out-Null
$env:PATH = (Join-Path $installation 'VC/Tools/Llvm/x64/bin') + [IO.Path]::PathSeparator + $env:PATH
$env:PYTHONDONTWRITEBYTECODE = '1'
$env:PYTHONUTF8 = '1'
$env:VSLANG = '1033'
$env:CC = 'cl'
$env:CXX = 'cl'
foreach ($variable in @('CL', '_CL_', 'CFLAGS', 'CXXFLAGS', 'LDFLAGS')) {
    if ([Environment]::GetEnvironmentVariable($variable)) { throw "QLT-011: clear external build flags in $variable." }
}
$compiler = (Get-Command cl -ErrorAction Stop).Source
if ((Get-Item -LiteralPath $compiler).VersionInfo.FileVersion -ne $versions.msvcCompiler) { throw "QLT-011: MSVC must be $($versions.msvcCompiler)." }
$checks = @(
    @{ Name = 'cmake'; Expected = $versions.cmake },
    @{ Name = 'ninja'; Expected = $versions.ninja },
    @{ Name = 'clang-tidy'; Expected = $versions.llvm },
    @{ Name = 'clang-format'; Expected = $versions.llvm },
    @{ Name = 'python'; Expected = $versions.python }
)
foreach ($check in $checks) {
    $output = & $check.Name --version 2>&1 | Out-String
    if ($LASTEXITCODE -ne 0 -or $output -notmatch ('(?<![\d.])' + [regex]::Escape($check.Expected) + '(?![\d.\w-])')) {
        throw "QLT-011: $($check.Name) must be $($check.Expected); actual: $output"
    }
}
