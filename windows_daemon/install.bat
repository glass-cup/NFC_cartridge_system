@echo off
setlocal

echo === NFC Cartridge Daemon Installer ===
echo.

:: ── Check winget ──────────────────────────────────────────────────────
where winget >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] winget not found.
    echo         Install "App Installer" from Microsoft Store, then re-run.
    pause
    exit /b 1
)

:: ── Install PowerToys ─────────────────────────────────────────────────
echo [1/2] Microsoft PowerToys ...
winget list --id Microsoft.PowerToys --accept-source-agreements >nul 2>&1
if %errorlevel% neq 0 (
    echo       Installing...
    winget install --id Microsoft.PowerToys --silent --accept-package-agreements --accept-source-agreements
    winget list --id Microsoft.PowerToys --accept-source-agreements >nul 2>&1
    if %errorlevel% neq 0 (
        echo [ERROR] PowerToys install failed.
        pause
        exit /b 1
    )
) else (
    echo       Already installed.
)

:: ── Find PowerToys install path from registry ─────────────────────────
set "PTDIR="
for /f "tokens=2*" %%A in (
    'reg query "HKCU\Software\Microsoft\Windows\CurrentVersion\Uninstall" /s /v InstallLocation 2^>nul ^| findstr /i "PowerToys"'
) do set "PTDIR=%%B"

if "%PTDIR%"=="" (
    echo [ERROR] Could not find PowerToys install path.
    pause
    exit /b 1
)

set "PT_EXE=%PTDIR%PowerToys.exe"
set "PTRUN=%LOCALAPPDATA%\Microsoft\PowerToys\PowerToys Run"

echo       Found: %PT_EXE%

:: ── Configure PowerToys Run ───────────────────────────────────────────
echo [2/2] Configuring PowerToys Run ...

:: Kill PowerToys so it doesn't overwrite our config on exit
taskkill /IM PowerToys.exe /F >nul 2>&1
timeout /t 2 >nul

:: Write a PowerShell script to patch the config
set "PSSCRIPT=%TEMP%\configure-ptrun.ps1"

> "%PSSCRIPT%" (
    echo $ErrorActionPreference = 'Stop'
    echo $settingsPath = '%PTRUN%\settings.json'
    echo $json = Get-Content $settingsPath -Raw ^| ConvertFrom-Json
    echo.
    echo # Hotkey: Win+Shift+D ^(D key = code 68^)
    echo $json.properties.open_powerlauncher = @{
    echo     win  = $true; ctrl = $false; alt = $false
    echo     shift = $true; code = 68; key = 'd'
    echo }
    echo $json.properties.DefaultOpenPowerLauncher = $json.properties.open_powerlauncher
    echo $json.properties.clear_input_on_launch = $true
    echo.
    echo # Enable Shell plugin globally so commands run without a prefix
    echo foreach ^($p in $json.plugins^) {
    echo     if ^($p.Id -eq 'D409510CD0D2481F853690A07E6DC426'^) {
    echo         $p.Disabled = $false
    echo         $p.IsGlobal = $true
    echo         $p.ActionKeyword = '^>'
    echo     }
    echo }
    echo.
    echo # Write back without BOM
    echo $utf8 = [System.Text.UTF8Encoding]::new^($false^)
    echo [System.IO.File]::WriteAllText^($settingsPath, ^($json ^| ConvertTo-Json -Depth 10^), $utf8^)
    echo Write-Host '       Configured.'
)

powershell -NoProfile -ExecutionPolicy Bypass -File "%PSSCRIPT%"
if %errorlevel% neq 0 (
    echo [ERROR] Failed to configure PowerToys Run.
    pause
    exit /b 1
)
del "%PSSCRIPT%" >nul 2>&1

:: Restart PowerToys
start "" "%PT_EXE%"
echo.
echo === Done ===
echo.
echo   PowerToys Run shortcut: Win+Shift+D
echo   Press Win+Shift+D, type a command, press Enter.
echo   Shell plugin enabled globally - no prefix needed.
echo.
echo   To change shortcut: PowerToys Settings ^> PowerToys Run
echo.
pause
