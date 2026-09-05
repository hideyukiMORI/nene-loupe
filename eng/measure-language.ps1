[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'toolchain.ps1')
$probeRoot = Join-Path $repoRoot 'out/phase0'
New-Item -ItemType Directory -Force -Path $probeRoot | Out-Null
$tidy = (Get-Command clang-tidy -ErrorAction Stop).Source
$cases = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'probes/language.json') -Raw | ConvertFrom-Json
$results = @()
foreach ($case in $cases) {
    $source = Join-Path $probeRoot ($case.id + '.cpp')
    Set-Content -LiteralPath $source -Value $case.source -Encoding utf8NoBOM
    $arguments = @('/nologo', '/std:c++latest', '/W4', '/WX', '/permissive-', '/EHsc', '/utf-8', '/c', $source, "/Fo$probeRoot/$($case.id).obj") + @($case.flags)
    $output = (& cl @arguments 2>&1 | Out-String)
    $code = $LASTEXITCODE
    $results += [ordered]@{ id = $case.id; tool = 'MSVC'; exitCode = $code; output = $output.Trim() }
    Write-Host "$($case.id): exit=$code"
    Write-Host $output
    if (($code -eq 0) -ne $case.compiles) { throw "Unexpected compiler result: $($case.id)" }
}
$source = Join-Path $probeRoot 'M2-public.cpp'
$output = (& $tidy $source '--checks=-*,misc-non-private-member-variables-in-classes' '--warnings-as-errors=*' '--' '-std=c++23' 2>&1 | Out-String)
$code = $LASTEXITCODE
$results += [ordered]@{ id = 'M2-tidy-aggregate-hole'; tool = 'clang-tidy'; exitCode = $code; output = $output.Trim() }
if ($code -ne 0) { throw 'M2 aggregate probe changed; review the recorded limitation.' }
$source = Join-Path $probeRoot 'M2-public-method.cpp'
$output = (& $tidy $source '--checks=-*,misc-non-private-member-variables-in-classes' '--warnings-as-errors=*' '--' '-std=c++23' 2>&1 | Out-String)
$code = $LASTEXITCODE
$results += [ordered]@{ id = 'M2-tidy'; tool = 'clang-tidy'; exitCode = $code; output = $output.Trim() }
Write-Host $output
if ($code -eq 0 -or $output -notmatch 'misc-non-private-member-variables-in-classes') { throw 'M2-tidy did not reject public state.' }
$results | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $probeRoot 'results.json') -Encoding utf8NoBOM
Write-Host 'Phase 0 compiler probes matched every expected result.'
