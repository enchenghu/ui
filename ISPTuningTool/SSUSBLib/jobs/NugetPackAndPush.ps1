$SolutionDir = $env:CI_PROJECT_DIR
# Create the NugetPackage
Invoke-WebRequest -Uri "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" -OutFile "$PSScriptRoot\nuget.exe"
New-Item -ItemType Directory -Path NugetPackageOutput -Force
$Result = Start-Process -FilePath "$PSScriptRoot\nuget.exe" -ArgumentList @("pack", "$SolutionDir\NugetPackage\SSUSBLib.nuspec", "-OutputDirectory", "NugetPackageOutput") -Wait -NoNewWindow -PassThru
if ($Result.ExitCode -ne 0)
{
    Write-Error "Failed to create the NugetPackage"
    exit 1
}

@"
<?xml version="1.0" encoding="utf-8"?>
<configuration>
  <packageSources>
    <add key="gitlab" value="${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/nuget/index.json" allowInsecureConnections="true" />
    <add key="AeNuget" value="$env:AE_NUGET_URL" allowInsecureConnections="true" />
  </packageSources>
</configuration>
"@ | Out-File -FilePath $PSScriptRoot\NuGet.Config -Encoding utf8

# Push the NugetPackage for each nupkg file
$NupkgFiles = Get-ChildItem -Path NugetPackageOutput -Filter *.nupkg
foreach ($NupkgFile in $NupkgFiles)
{
    $Result = Start-Process -FilePath "$PSScriptRoot\nuget.exe" -ArgumentList @("push" , $NupkgFile.FullName, "-Source", "${CI_API_V4_URL}/projects/${CI_PROJECT_ID}/packages/nuget/index.json", "-ApiKey", "$CI_JOB_TOKEN", "-ConfigFile", "$PSScriptRoot\NuGet.Config") -Wait -NoNewWindow -PassThru
    if ($Result.ExitCode -ne 0)
    {
        Write-Error "Failed to push the NugetPackage"
        exit 1
    }
    $Result = Start-Process -FilePath "$PSScriptRoot\nuget.exe" -ArgumentList @("push" , $NupkgFile.FullName, "-Source", "$env:AE_NUGET_URL", "-ApiKey", "$env:AE_NUGET_APIKEY", "-ConfigFile", "$PSScriptRoot\NuGet.Config") -Wait -NoNewWindow -PassThru
    if ($Result.ExitCode -ne 0)
    {
        Write-Error "Failed to push the NugetPackage"
        exit 1
    }
}