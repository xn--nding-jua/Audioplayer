@echo off
cls
echo =======================================================
echo f/bape Audioplayer : Firmwareupdater for NINA-W102
echo Dr.-Ing. Christian Noeding
echo Infos: https://www.github.com/xn--nding-jua/Audioplayer
echo =======================================================

echo Searching for Arduino MKR Vidor 4000 device...
for /f "tokens=1* delims==" %%I in ('wmic path win32_pnpentity get caption  /format:list ^| find "Arduino MKR Vidor 4000"') do (
    goto :setCOM "%%~J"
)


echo Error: Cannot find f/bape Audioplayer device!
echo. 
echo ----------------------------------
echo. 
set /p port=Please set COM-Port manually [COMx]: 
goto :flash


:setCOM <WMIC_output_line>
:: sets _COM#=line
setlocal
set "str=%~1"
set "num=%str:*(COM=%"
set "num=%num:)=%"
set port=COM%num%
echo Found Arduino MKR Vidor 4000 on %port%...


:flash
echo samd:update:nina > %port%
echo Waiting for bootloader-mode...
timeout /t 6 /nobreak

echo. 
echo ----------------------------------
echo. 
echo Flashing LittleFS to ESP32 on %port%...
rem "%APPDATA%\..\Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1\esptool.exe" --chip esp32 --port %port% --baud 921600 write_flash -z --flash_mode dio --flash_freq 80m --flash_size 2MB 0x10000 NINA.bin
Tools\esptool.exe --chip esp32 --port %port% --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 2MB 0x1000 Update\NINA.bootloader.bin 0x8000 Update\NINA.partitions.bin 0xe000 Tools\boot_app0.bin 0x10000 Update\NINA.bin

rem Tools\esptool.exe --chip esp32 --port COM9 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 2MB 0x1000 Update\NINA.bootloader.bin 0x8000 Update\NINA.partitions.bin 0xe000 Tools\boot_app0.bin 0x10000 Update\NINA.bin

echo Update completed.
pause