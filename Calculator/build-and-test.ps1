$ErrorActionPreference = 'Stop'
$qtBin = 'E:\Qt\6.8.3\mingw_64\bin'
$mingwBin = 'E:\Qt\Tools\mingw1310_64\bin'
$env:PATH = "$mingwBin;$qtBin;$env:PATH"

Push-Location $PSScriptRoot
try {
    & "$qtBin\qmake.exe" Calculator.pro
    if ($LASTEXITCODE -ne 0) { throw 'qmake failed' }
    & "$mingwBin\mingw32-make.exe" -j4
    if ($LASTEXITCODE -ne 0) { throw 'calculator build failed' }

    Push-Location tests
    try {
        & "$qtBin\qmake.exe" tests.pro
        if ($LASTEXITCODE -ne 0) { throw 'test qmake failed' }
        & "$mingwBin\mingw32-make.exe" -j4
        if ($LASTEXITCODE -ne 0) { throw 'test build failed' }
        & '.\release\CalculatorTests.exe' -o results-windows.txt,txt
        if ($LASTEXITCODE -ne 0) { throw 'tests failed' }
        Get-Content results-windows.txt -Tail 2
    } finally { Pop-Location }
} finally { Pop-Location }
