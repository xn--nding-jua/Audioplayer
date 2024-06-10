@echo off
cls
set /p port=Please set COM-Port manually [COMx]: 

echo Set NINA-module into update-mode...
echo samd:update:nina > %port%

echo Flashing ESP32 within NINA-module...

Tools\esptool.exe --chip esp32 --port %port% --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 2MB 0x1000 build\esp32.esp32.nina_w10\NINA.ino.bootloader.bin 0x8000 build\esp32.esp32.nina_w10\NINA.ino.partitions.bin 0xe000 Tools\boot_app0.bin 0x10000 build\esp32.esp32.nina_w10\NINA.ino.bin

rem Tools\esptool.exe --chip esp32 --port %port% --baud 921600 --before default_reset --after hard_reset write_flash -e -z --flash_mode dio --flash_freq 80m --flash_size 2MB 0x1000 build\esp32.esp32.nina_w10\NINA.ino.bootloader.bin 0x8000 build\esp32.esp32.nina_w10\NINA.ino.partitions.bin 0xe000 Tools\boot_app0.bin 0x10000 build\esp32.esp32.nina_w10\NINA.ino.bin

echo Done.
