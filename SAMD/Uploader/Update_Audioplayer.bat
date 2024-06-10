@echo off
setlocal

cls
echo =======================================================
echo f/bape Audioplayer Firmware-Updater v1.0
echo Dr.-Ing. Christian Noeding
echo Infos: https://www.github.com/xn--nding-jua/Audioplayer
echo =======================================================

for /f "tokens=1* delims==" %%I in ('wmic path win32_pnpentity get caption  /format:list ^| find "Arduino MKR Vidor 4000"') do (
    call :resetCOM "%%~J"
)

:continue

:: wmic /format:list strips trailing spaces (at least for path win32_pnpentity)
for /f "tokens=1* delims==" %%I in ('wmic path win32_pnpentity get caption  /format:list ^| find "Arduino MKR Vidor 4000 bootloader"') do (
    call :setCOM "%%~J"
)

:: end main batch
echo !ERROR: Cannot find f/bape Audioplayer device!
echo Update aborted!
goto :EOF

:resetCOM <WMIC_output_line>
:: sets _COM#=line
setlocal
set "str=%~1"
set "num=%str:*(COM=%"
set "num=%num:)=%"
set port=COM%num%
echo Requesting bootloader-mode on %port%...
mode %port%: BAUD=1200 parity=N data=8 stop=1 > nul
echo Waiting for bootloader-mode...
timeout /t 3 /nobreak
goto :continue

:setCOM <WMIC_output_line>
:: sets _COM#=line
setlocal
set "str=%~1"
set "num=%str:*(COM=%"
set "num=%num:)=%"
set port=COM%num%
echo Programming device on %port%...
goto :flash

:flash
bossac.exe --port=%port% -I -U true -e -w Audioplayer.ino.bin -R
echo Update completed.
pause
