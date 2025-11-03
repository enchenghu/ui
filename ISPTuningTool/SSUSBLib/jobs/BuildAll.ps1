$SolutionDir = $env:CI_PROJECT_DIR
$installationPath = @(vswhere -prerelease -latest -property installationPath)
if ($installationPath -and (test-path "$installationPath\Common7\Tools\vsdevcmd.bat"))
{
    & "${env:COMSPEC}" /s /c "`"$installationPath\Common7\Tools\vsdevcmd.bat`" -no_logo && set" | foreach-object {
        $name, $value = $_ -split '=', 2
        # if name starts with CI or empty, skip it
        if ($name -match '^CI' -or $name -eq '') { return }
        Write-Output "Setting $name=$value"
        set-content env:\$name $value
    }
}

 msbuild "$SolutionDir\jobs\BuildALl.proj"
