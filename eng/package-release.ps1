[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'toolchain.ps1')

$releaseRoot = Join-Path $repoRoot 'out/release'
$buildDir = Join-Path $releaseRoot 'build'
$stageDir = Join-Path $releaseRoot 'stage'

function Remove-ReleaseDirectory([string]$path) {
    $root = [IO.Path]::GetFullPath($releaseRoot) + [IO.Path]::DirectorySeparatorChar
    $target = [IO.Path]::GetFullPath($path) + [IO.Path]::DirectorySeparatorChar
    if (-not $target.StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Release cleanup escaped out/release: $target"
    }
    if (Test-Path -LiteralPath $path) {
        Remove-Item -LiteralPath $path -Recurse -Force
    }
}

Push-Location $repoRoot
try {
    New-Item -ItemType Directory -Force -Path $releaseRoot | Out-Null
    New-Item -ItemType Directory -Force -Path (Join-Path $buildDir '.cmake/api/v1/query') | Out-Null
    New-Item -ItemType File -Force -Path (Join-Path $buildDir '.cmake/api/v1/query/codemodel-v2') | Out-Null

    & cmake --fresh -S . -B $buildDir -G Ninja -DCMAKE_BUILD_TYPE=Release
    if ($LASTEXITCODE -ne 0) { throw 'Release CMake configure failed.' }
    & cmake --build $buildDir --clean-first
    if ($LASTEXITCODE -ne 0) { throw 'Release build failed.' }
    & python eng/conformance.py --build-dir $buildDir
    if ($LASTEXITCODE -ne 0) { throw 'Release build graph verification failed.' }
    & ctest --test-dir $buildDir --output-on-failure --no-tests=error
    if ($LASTEXITCODE -ne 0) { throw 'Release C++ verification failed.' }

    $cacheBuildType = (& cmake -N -L $buildDir | Select-String '^CMAKE_BUILD_TYPE:STRING=Release$')
    if ($null -eq $cacheBuildType) { throw 'Release build directory is not configured as Release.' }
    $versionPath = Join-Path $buildDir 'NeNeLoupeVersion.txt'
    $executable = Join-Path $buildDir 'NeNeLoupe.exe'
    if (-not (Test-Path -LiteralPath $versionPath -PathType Leaf)) { throw 'Generated release version is missing.' }
    if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) { throw 'Release executable is missing.' }
    $version = (Get-Content -LiteralPath $versionPath -Raw).Trim()
    if ($version -notmatch '^\d+\.\d+\.\d+$') { throw "Generated release version is invalid: $version" }

    $versionInfo = (Get-Item -LiteralPath $executable).VersionInfo
    if ($versionInfo.ProductVersion -ne $version -or $versionInfo.FileVersion -ne "$version.0") {
        throw "Release VERSIONINFO does not match $version."
    }
    $manifestPath = Join-Path $buildDir 'NeNeLoupe.extracted.manifest'
    & mt.exe -nologo "-inputresource:$executable;#1" "-out:$manifestPath"
    if ($LASTEXITCODE -ne 0) { throw 'Release manifest extraction failed.' }
    [xml]$manifest = Get-Content -LiteralPath $manifestPath -Raw
    if ($manifest.assembly.assemblyIdentity.version -ne "$version.0") {
        throw "Release manifest does not match $version."
    }
    $pe = (& llvm-readobj --file-headers --coff-imports $executable | Out-String)
    if ($LASTEXITCODE -ne 0) { throw 'Release PE inspection failed.' }
    if ($pe -notmatch 'Machine: IMAGE_FILE_MACHINE_AMD64') { throw 'Release executable is not x64.' }
    if ($pe -match '(?i)Name: (?:MSVCP|VCRUNTIME|UCRTBASE|api-ms-win-crt-)') {
        throw 'Release executable has a dynamic Visual C++ runtime dependency.'
    }

    Remove-ReleaseDirectory $stageDir
    New-Item -ItemType Directory -Path $stageDir | Out-Null
    Copy-Item -LiteralPath $executable -Destination (Join-Path $stageDir 'NeNeLoupe.exe')
    Copy-Item -LiteralPath (Join-Path $repoRoot 'docs/release/README.txt') -Destination (Join-Path $stageDir 'README.txt')
    Copy-Item -LiteralPath (Join-Path $repoRoot 'LICENSE') -Destination (Join-Path $stageDir 'LICENSE')

    $expectedFiles = @('LICENSE', 'NeNeLoupe.exe', 'README.txt')
    $actualFiles = @(Get-ChildItem -LiteralPath $stageDir -File | Sort-Object Name | Select-Object -ExpandProperty Name)
    if (Compare-Object $expectedFiles $actualFiles) { throw 'Release stage has unexpected contents.' }

    $archiveName = "NeNeLoupe-v$version-windows-x64.zip"
    $archivePath = Join-Path $releaseRoot $archiveName
    if (Test-Path -LiteralPath $archivePath) { Remove-Item -LiteralPath $archivePath -Force }
    Compress-Archive -Path (Join-Path $stageDir '*') -DestinationPath $archivePath -CompressionLevel Optimal
    $archiveHash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash
    $checksumPath = Join-Path $releaseRoot 'SHA256SUMS'
    [IO.File]::WriteAllText($checksumPath, "$archiveHash *$archiveName`n", [Text.UTF8Encoding]::new($false))

    Write-Host "Release archive: $archivePath"
    Write-Host "SHA256: $archiveHash"
}
finally {
    Pop-Location
}
