@echo off
cls
echo =======================================================
echo f/bape Audioplayer : Data-folder-Conversion to LittleFS
echo Dr.-Ing. Christian Noeding
echo Infos: https://www.github.com/xn--nding-jua/Audioplayer
echo =======================================================
echo Deleting old file...
del littlefs.bin

rem Block Size: 4096 (standard for SPI Flash)
rem Page Size: 256 (standard for SPI Flash)
rem Image Size: Size of the partition in bytes (can be obtained from a partition table)
rem Partition Offset: Starting address of the partition (can be obtained from a partition table)
rem more Information: https://github.com/earlephilhower/mklittlefs

rem At the moment we can use this partition-scheme for NINA-W102:
rem 1.3MB APP / 700kB SPIFFS (More information here: https://github.com/espressif/arduino-esp32/blob/master/tools/partitions/minimal.csv)
rem
rem This means:
rem
rem Description	Offset	     Size
rem ==================================
rem NVS		0x009000 ... 0x00DFFF
rem OTADATA	0x00E000 ... 0x00FFFF
rem APP0	0x010000 ... 0x14FFFF
rem SPIFFS	0x150000 ... 0x1EFFFF	<- here we can save our LittleFS-data
rem CoreDump	0x1F0000 ... 0x1FFFFF

echo. 
echo ----------------------------------
echo. 

echo Compiling data-directory into 640kByte file-system...
Tools\mklittlefs.exe -c data -b 4096 -p 256 -s 0xA0000 littlefs.bin

echo. 
echo ----------------------------------
echo. 

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
echo system:update:nina > %port%
echo Waiting for bootloader-mode...
timeout /t 6 /nobreak

echo. 
echo ----------------------------------
echo. 
echo Flashing LittleFS to ESP32 on %port%...
rem "%APPDATA%\..\Local\Arduino15\packages\esp32\tools\esptool_py\4.5.1\esptool.exe" --chip esp32 --port %port% --baud 921600 write_flash -z --flash_mode dio --flash_freq 80m --flash_size 2MB 0x150000 littlefs.bin
Tools\esptool.exe --chip esp32 --port %port% --baud 921600 write_flash -z --flash_mode dio --flash_freq 80m --flash_size 2MB 0x150000 littlefs.bin
echo Update completed.

pause
