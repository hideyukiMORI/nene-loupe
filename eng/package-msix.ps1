[CmdletBinding()]
param(
    # Store へ出すパッケージを作る。身元は eng/store-identity.json（Partner Center の値）に固定し、署名しない。
    [switch]$Submission,
    # 手元の検証用の身元。-Submission とは併用できない。
    [string]$IdentityName = 'NeNeLoupe.LocalTest',
    [string]$Publisher = 'CN=NeNe Loupe MSIX Local Test',
    [string]$PublisherDisplayName = 'NeNe Loupe local test',
    # 省略時は exe の版に .0 を足す。Store は先頭が 0 の版を受け付けない（ADR 0008）。
    [string]$PackageVersion,
    # CurrentUser\My にある証明書の拇印。省略時は未署名の MSIX だけを作る。-Submission とは併用できない（Store が署名する）。
    [string]$CertificateThumbprint
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'toolchain.ps1')

# ADR 0008。全体ゲートの外にある。提出用は package-release.ps1 -StoreMsix が続けて呼ぶ。
# 入力は package-release.ps1 が検査を終えた stage の exe だけで、ここでは再ビルドしない。
$executable = Join-Path $repoRoot 'out/release/stage/NeNeLoupe.exe'
$msixRoot = Join-Path $repoRoot 'out/msix'
$layoutDir = Join-Path $msixRoot 'layout'
$assetsDir = Join-Path $layoutDir 'Assets'

function Remove-MsixDirectory([string]$path) {
    $root = [IO.Path]::GetFullPath($msixRoot) + [IO.Path]::DirectorySeparatorChar
    $target = [IO.Path]::GetFullPath($path) + [IO.Path]::DirectorySeparatorChar
    if (-not $target.StartsWith($root, [StringComparison]::OrdinalIgnoreCase)) {
        throw "MSIX cleanup escaped out/msix: $target"
    }
    if (Test-Path -LiteralPath $path) {
        Remove-Item -LiteralPath $path -Recurse -Force
    }
}

function Get-ZipEntryHash([string]$archive, [string]$entryName) {
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $zip = [IO.Compression.ZipFile]::OpenRead($archive)
    try {
        $entry = $zip.GetEntry($entryName)
        if ($null -eq $entry) { throw "Release archive has no $entryName." }
        $stream = $entry.Open()
        try { return [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($stream)) }
        finally { $stream.Dispose() }
    }
    finally { $zip.Dispose() }
}

function Save-Logo([Drawing.Icon]$icon, [int]$size, [string]$path) {
    $source = $icon.ToBitmap()
    $logo = [Drawing.Bitmap]::new($size, $size, [Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [Drawing.Graphics]::FromImage($logo)
    try {
        $graphics.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
        $graphics.PixelOffsetMode = [Drawing.Drawing2D.PixelOffsetMode]::HighQuality
        $graphics.Clear([Drawing.Color]::Transparent)
        $graphics.DrawImage($source, 0, 0, $size, $size)
        $logo.Save($path, [Drawing.Imaging.ImageFormat]::Png)
    }
    finally {
        $graphics.Dispose()
        $logo.Dispose()
        $source.Dispose()
    }
}

if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) {
    throw 'Release stage executable is missing. Run eng/package-release.ps1 first.'
}
$version = (Get-Item -LiteralPath $executable).VersionInfo.ProductVersion
if ($version -notmatch '^\d+\.\d+\.\d+$') { throw "Release executable version is invalid: $version" }
if (-not $PackageVersion) { $PackageVersion = "$version.0" }
if ($PackageVersion -notmatch '^\d+\.\d+\.\d+\.0$') {
    throw "MSIX version must be four parts ending in .0: $PackageVersion"
}
if ($PackageVersion.StartsWith('0.')) {
    if ($Submission) { throw "MSIX version $PackageVersion starts with 0. Microsoft Store rejects it (ADR 0008)." }
    Write-Warning "MSIX version $PackageVersion starts with 0. It installs locally, but Microsoft Store rejects it."
}

if ($Submission) {
    foreach ($name in @('IdentityName', 'Publisher', 'PublisherDisplayName', 'CertificateThumbprint')) {
        if ($PSBoundParameters.ContainsKey($name)) { throw "-$name cannot be combined with -Submission." }
    }
    $identity = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'store-identity.json') -Raw | ConvertFrom-Json
    $IdentityName = $identity.identityName
    $Publisher = $identity.publisher
    $PublisherDisplayName = $identity.publisherDisplayName
}

# MSIX に入れる exe は、ZIP で配る exe と同じものでなければならない（ADR 0008）。
$archivePath = Join-Path $repoRoot "out/release/NeNeLoupe-v$version-windows-x64.zip"
if (-not (Test-Path -LiteralPath $archivePath -PathType Leaf)) {
    throw 'Release archive is missing. Run eng/package-release.ps1 first.'
}
$executableHash = (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash
if ((Get-ZipEntryHash $archivePath 'NeNeLoupe.exe') -ne $executableHash) {
    throw 'Release stage executable differs from the executable inside the release archive.'
}

Remove-MsixDirectory $layoutDir
New-Item -ItemType Directory -Force -Path $assetsDir | Out-Null
Copy-Item -LiteralPath $executable -Destination (Join-Path $layoutDir 'NeNeLoupe.exe')

Add-Type -AssemblyName System.Drawing
$icon = [Drawing.Icon]::new((Join-Path $repoRoot 'src/app/NeNeLoupe.ico'), 256, 256)
try {
    Save-Logo $icon 44 (Join-Path $assetsDir 'Square44x44Logo.png')
    Save-Logo $icon 150 (Join-Path $assetsDir 'Square150x150Logo.png')
    Save-Logo $icon 50 (Join-Path $assetsDir 'StoreLogo.png')
}
finally {
    $icon.Dispose()
}

# MinVersion は ADR 0005 の対応環境（Windows 10 version 2004）と同じ。
$manifest = @"
<?xml version="1.0" encoding="utf-8"?>
<Package xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
         xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
         xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities"
         IgnorableNamespaces="uap rescap">
  <Identity Name="$([Security.SecurityElement]::Escape($IdentityName))"
            Publisher="$([Security.SecurityElement]::Escape($Publisher))"
            Version="$PackageVersion"
            ProcessorArchitecture="x64" />
  <Properties>
    <DisplayName>NeNe Loupe</DisplayName>
    <PublisherDisplayName>$([Security.SecurityElement]::Escape($PublisherDisplayName))</PublisherDisplayName>
    <Logo>Assets\StoreLogo.png</Logo>
  </Properties>
  <Dependencies>
    <TargetDeviceFamily Name="Windows.Desktop" MinVersion="10.0.19041.0" MaxVersionTested="10.0.26100.0" />
  </Dependencies>
  <Resources>
    <Resource Language="en-us" />
  </Resources>
  <Applications>
    <Application Id="NeNeLoupe" Executable="NeNeLoupe.exe" EntryPoint="Windows.FullTrustApplication">
      <uap:VisualElements DisplayName="NeNe Loupe"
                          Description="A tiny frameless screen loupe and colour picker."
                          BackgroundColor="transparent"
                          Square150x150Logo="Assets\Square150x150Logo.png"
                          Square44x44Logo="Assets\Square44x44Logo.png" />
    </Application>
  </Applications>
  <Capabilities>
    <rescap:Capability Name="runFullTrust" />
  </Capabilities>
</Package>
"@
[IO.File]::WriteAllText((Join-Path $layoutDir 'AppxManifest.xml'), $manifest, [Text.UTF8Encoding]::new($false))

$packageSuffix = if ($Submission) { '-store' } else { '' }
$packagePath = Join-Path $msixRoot "NeNeLoupe-v$version-windows-x64$packageSuffix.msix"
& makeappx.exe pack /o /d $layoutDir /p $packagePath
if ($LASTEXITCODE -ne 0) { throw 'MSIX packaging failed.' }

$verifyDir = Join-Path $msixRoot 'verify'
Remove-MsixDirectory $verifyDir
& makeappx.exe unpack /o /p $packagePath /d $verifyDir | Out-Null
if ($LASTEXITCODE -ne 0) { throw 'MSIX unpack for verification failed.' }
if ((Get-FileHash -LiteralPath (Join-Path $verifyDir 'NeNeLoupe.exe') -Algorithm SHA256).Hash -ne $executableHash) {
    throw 'MSIX executable differs from the release executable.'
}
Write-Host "Identity: $IdentityName / $Publisher / $PublisherDisplayName"
Write-Host "Executable SHA256 (stage = ZIP = MSIX): $executableHash"

if ($CertificateThumbprint) {
    & signtool.exe sign /fd SHA256 /s My /sha1 $CertificateThumbprint $packagePath
    if ($LASTEXITCODE -ne 0) { throw 'MSIX signing failed.' }
    Write-Host "Signed MSIX: $packagePath"
}
else {
    Write-Host "Unsigned MSIX: $packagePath"
}
Write-Host "SHA256: $((Get-FileHash -LiteralPath $packagePath -Algorithm SHA256).Hash)"
