# f/bape - An FPGA-based Audioplayer with Equalizer

In some weeks this repository will contain an FPGA-based MP3-Audioplayer with Audio-Input, Parametric-Equalizers, dynamic Audio-Compressors, Frequency-Crossovers and Webserver-Control for a 2.1 Stereo-/Subwoofer-Combination.

Within the first half of 2024 an Elektor-article will be published describing most of software. The hardware is an Arduino MKR Vidor 4000 with an Intel Cyclone 10 LP FPGA, a SAMD21 32-Bit-CortexM0 Controller and an ESP32-chip. More information you can find here: https://store.arduino.cc/products/arduino-mkr-vidor-4000

## Main-Features of the system
- [x] MP3-Playback via SD-Card
- [x] I2S-Input from external ADC
- [x] SPDIF-Input (Stereo) or input via Behringer UltraNet (16 channel)
- [x] 5x Parametric-Equalizer
- [x] 24dB/oct Linkwitz-Riley Crossover for tweeter and subwoofer
- [x] Noise-Gate and dynamic Audio-Compression
- [x] Volumecontrol, Balancing between left/right
- [x] Webserver for Controlling EQing and other parameters
- [x] Control via ASCII-based commands (or optional MQTT)
- [x] Analog Audio-Output via 3x PDM and SPDIF
- [x] Delay from input to output at around 20µs, hence REALtime-processing

## Overview
TBD

## Connections
TBS

## Uploading firmware
### SAMD21 with bitstream for Intel Cyclone 10-FPGA
TBD

### NINA WiFi-module based on ESP32
TBD
