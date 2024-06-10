@echo off
cls
echo Converting TTF-file to header-file...
del bitstream.h > nul
..\FPGA\Tools\vidorcvt\vidorcvt.exe < ..\FPGA\output_files\Audioplayer.ttf > bitstream.h
echo Done.
rem pause