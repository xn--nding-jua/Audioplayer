/*
  f/bape USBCtrl for Arduino MKR Vidor4000 Device
  v1.6.0 built on 10.06.2024
  Infos: https://www.github.com/xn--nding-jua/Audioplayer
  Copyright (c) 2023-2024 Dr.-Ing. Christian Nöding

  Target-Board: Arduino MKR Vidor 4000 (Atmel SAMD21 32-bit CortexM0+ controller)
  Board-package: http://downloads.arduino.cc/Hourly/samd/package_samd-hourly-build_index.json

  Description:
  =====================================================================
  This Arduino-Project loads a bitstream to the Intel Cyclone10LP FPGA and
  communicates with USB-Host and NINA-W102-Module, which implements a full
  featured MP3, WAVE and AAC SD-Card-Player with WiFi-Webserver and A2DP-
  Bluetooth-Receiver.

  Used libraries:
  =====================================================================
  only when using OLED display:
  - MCP23008 v0.3.2 by Rob Tillaart for external GPIO
  - Adafruit SSD1306 v2.5.10 by Adafruit (for I2C Display)

  only for VUmeter / Spectrum View:
  - arduinoFFT v2.0.2
  - Adafruit GFX Library v1.11.9
  - Adafruit NeoMatrix v1.3.2
  - Adafruit NeoPixel v1.3.2 (non DMA, but maybe DMA works well, too)

  License information:
  =====================================================================
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.

  Pin information:
  =====================================================================
  +------------+------------------+--------+--------+-----------------+--------+-----------------------+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  | Pin number |  MKR  Board pin  |  PIN   |  FPGA  | Notes           | Peri.A |     Peripheral B      | Perip.C | Perip.D | Peri.E | Peri.F | Periph.G | Periph.H | Usage for Audioplayer                                     |
  |            |                  |        |        |                 |   EIC  | ADC |  AC | PTC | DAC | SERCOMx | SERCOMx |  TCCx  |  TCCx  |    COM   | AC/GLCK  |                                                           |
  |            |                  |        |        |                 |(EXTINT)|(AIN)|(AIN)|     |     | (x/PAD) | (x/PAD) | (x/WO) | (x/WO) |          |          |                                                           |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  | 00         | D0               |  PA22  |  G1    |                 |  *06   |     |     | X10 |     |   3/00  |   5/00  |* TC4/0 | TCC0/4 |          | GCLK_IO6 | -> Serial2 TxD -> FPGA G1 -> VHDL -> FPGA T6 -> NINA RxD  |
  | 01         | D1               |  PA23  |  N3    |                 |  *07   |     |     | X11 |     |   3/01  |   5/01  |* TC4/1 | TCC0/5 | USB/SOF  | GCLK_IO7 | <- Serial2 RxD <- FPGA N3 <- VHDL <- FPGA E15 <- NINA TxD |
  | 02         | D2               |  PA10  |  P3    |                 |   10   | *18 |     | X02 |     |   0/02  |   2/02  |*TCC1/0 | TCC0/2 | I2S/SCK0 | GCLK_IO4 | -> PDM Channel Left                                       |
  | 03         | D3               |  PA11  |  R3    |                 |   11   | *19 |     | X03 |     |   0/03  |   2/03  |*TCC1/1 | TCC0/3 | I2S/FS0  | GCLK_IO5 | -> PDM Channel Right                                      |
  | 04         | D4               |  PB10  |  T3    |                 |  *10   |     |     |     |     |         |   4/02  |* TC5/0 | TCC0/4 | I2S/MCK1 | GCLK_IO4 | -> PDM Channel Sub                                        |
  | 05         | D5               |  PB11  |  T2    |                 |  *11   |     |     |     |     |         |   4/03  |* TC5/1 | TCC0/5 | I2S/SCK1 | GCLK_IO5 | -> SPDIF-Output (AES/EBU)                                 |
  | 06         | D6               |  PA20  |  G16   |                 |  *04   |     |     | X08 |     |   5/02  |   3/02  |        |*TCC0/6 | I2S/SCK0 | GCLK_IO4 | -> SPDIF-Input (AES/EBU)                                  |
  | 07         | D7               |  PA21  |  G15   |                 |  *05   |     |     | X09 |     |   5/03  |   3/03  |        |*TCC0/7 | I2S/FS0  | GCLK_IO5 | -> SPI-CS SD-Card NINA                                    |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  |            |       SPI        |        |        |                 |        |     |     |     |     |         |         |        |        |          |          |                                                           |
  | 08         | MOSI             |  PA16  |  F16   |                 |  *00   |     |     | X04 |     |  *1/00  |   3/00  |*TCC2/0 | TCC0/6 |          | GCLK_IO2 | -> SPI-MOSI SD-Card NINA                                  |
  | 09         | SCK              |  PA17  |  F15   |                 |  *01   |     |     | X05 |     |  *1/01  |   3/01  | TCC2/1 | TCC0/7 |          | GCLK_IO3 | -> SPI-SCK SD-Card NINA                                   |
  | 10         | MISO             |  PA19  |  C16   |                 |   03   |     |     | X07 |     |  *1/03  |   3/03  |* TC3/1 | TCC0/3 | I2S/SD0  | AC/CMP1  | -> SPI-MISO SD-Card NINA                                  |
  +------------+------------------+--------+--------+-----------------+--------------------+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  |            |       Wire       |        |        |                 |        |     |     |     |     |         |         |        |        |          |          |                                                           | 
  | 11         | SDA              |  PA08  |  C15   |                 |   NMI  | *16 |     | X00 |     |  *0/00  |   2/00  | TCC0/0 | TCC1/2 | I2S/SD1  |          | -> I2C SDA (SSD1308 Display) / NINA PIO27 (Bootmode-Sel.) |
  | 12         | SCL              |  PA09  |  B16   |                 |   09   | *17 |     | X01 |     |  *0/01  |   2/01  | TCC0/1 | TCC1/3 | I2S/MCK0 |          | -> I2C SCL (SSD1308 Display)                              |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  |            |      Serial1     |        |        |                 |        |     |     |     |     |         |         |        |        |          |          |                                                           |
  | 13         | RX               |  PB23  |  C11   |                 |   07   |     |     |     |     |         |  *5/03  |        |        |          | GCLK_IO1 | -> Unused / Serial1 RxD for comm. with external hardware  |
  | 14         | TX               |  PB22  |  A13   |                 |   06   |     |     |     |     |         |  *5/02  |        |        |          | GCLK_IO0 | -> Unused / Serial1 TxD for comm. with external hardware  |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  | 15         | A0 / DAC0        |  PA02  |  C2    |                 |   02   | *00 |     | Y00 | OUT |         |         |        |        |          |          | -> Potentiometer for Menu-Navigation                      |
  | 16         | A1               |  PB02  |  C3    |                 |  *02   | *10 |     | Y08 |     |         |   5/00  |        |        |          |          | -> UltraNet-Input A (Ultranet Ch1-8)                      |
  | 17         | A2               |  PB03  |  C6    |                 |  *03   | *11 |     | Y09 |     |         |   5/01  |        |        |          |          | -> UltraNet-Input B (Ultranet Ch9-16)                     |
  | 18         | A3               |  PA04  |  D1    |                 |   04   | *04 |  00 | Y02 |     |         |   0/00  |*TCC0/0 |        |          |          | -> I2S ADC MainClock (256x fs) / Analog in for Spectrum   |
  | 19         | A4               |  PA05  |  D3    |                 |   05   | *05 |  01 | Y03 |     |         |   0/01  |*TCC0/1 |        |          |          | -> I2S ADC SerialClock                                    |
  | 20         | A5               |  PA06  |  F3    |                 |   06   | *06 |  02 | Y04 |     |         |   0/02  | TCC1/0 |        |          |          | -> I2S ADC WordClock                                      |
  | 21         | A6               |  PA07  |  G2    |                 |   07   | *07 |  03 | Y05 |     |         |   0/03  | TCC1/1 |        | I2S/SD0  |          | -> I2S ADC SerialData                                     |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  |            |       USB        |        |        |                 |        |     |     |     |     |         |         |        |        |          |          |                                                           |
  | 22         |                  |  PA24  |        | USB N           |   12   |     |     |     |     |   3/02  |   5/02  |  TC5/0 | TCC1/2 | USB/DM   |          | -> Serial Rx/Tx via USB                                   |
  | 23         |                  |  PA25  |        | USB P           |   13   |     |     |     |     |   3/03  |   5/03  |  TC5/1 | TCC1/3 | USB/DP   |          | -> Serial Rx/Tx via USB                                   |
  | 24         |                  |  PA18  |        | USB ID          |   02   |     |     | X06 |     |   1/02  |   3/02  |  TC3/0 | TCC0/2 |          | AC/CMP0  |                                                           |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  | 25         | AREF             |  PA03  |  B1    |                 |   03   |  01 |     | Y01 |     |         |         |        |        |          |          | -> WS2815 Neopixels (either VUmeter or Spectrum-Matrix)   |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  |            | FPGA             |        |        |                 |        |     |     |     |     |         |         |        |        |          |          |                                                           |
  | 26         |                  |  PA12  |  H4    | FPGA TDI        |   12   |     |     |     |     |  *2/00  |   4/00  | TCC2/0 | TCC0/6 |          | AC/CMP0  | -> [JTAG]                                                 |
  | 27         |                  |  PA13  |  H3    | FPGA TCK        |   13   |     |     |     |     |  *2/01  |   4/01  | TCC2/1 | TCC0/7 |          | AC/CMP1  | -> [JTAG]                                                 |
  | 28         |                  |  PA14  |  J5    | FPGA TMS        |   14   |     |     |     |     |   2/02  |   4/02  |  TC3/0 | TCC0/4 |          | GCLK_IO0 | -> [JTAG]                                                 |
  | 29         |                  |  PA15  |  J4    | FPGA TDO        |   15   |     |     |     |     |  *2/03  |   4/03  |  TC3/1 | TCC0/5 |          | GCLK_IO1 | -> [JTAG]                                                 |
  | 30         |                  |  PA27  |  E2    | FPGA CLK        |   15   |     |     |     |     |         |         |        |        |          | GCLK_IO0 | -> 48 MHz Clock -> FPGA                                   |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  | 31         |                  |  PA28  |  L16   | FPGA MB INT     |   08   |     |     |     |     |         |         |        |        |          | GCLK_IO0 | -> NINA RESET_N                                           |
  | 32         |                  |  PB08  |        | LED_RED_BUILTIN |   08   |  02 |     | Y14 |     |         |   4/00  |  TC4/0 |        |          |          | -> LED                                                    |
  | 33         |                  |  PB09  |        | SAM_INT_OUT     |  *09   |  03 |     | Y15 |     |         |   4/01  |  TC4/1 |        |          |          |                                                           |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
  |            | 32768Hz Crystal  |        |        |                 |        |     |     |     |     |         |         |        |        |          |          |                                                           |
  | 34         |                  |  PA00  |        | XIN32           |   00   |     |     |     |     |         |   1/00  | TCC2/0 |        |          |          |                                                           |
  | 35         |                  |  PA01  |        | XOUT32          |   01   |     |     |     |     |         |   1/01  | TCC2/1 |        |          |          |                                                           |
  +------------+------------------+--------+--------+-----------------+--------+-----+-----+-----+-----+---------+---------+--------+--------+----------+----------+-----------------------------------------------------------+
*/

#include "SAMD.h"

#if USE_DISPLAY == 1
  void ticker100msFcn() {
    // do menu/GUI-relevant stuff
    readExternalGPIO();
    readADC(); // SAMD21's internal 12-bit ADC can handle up to 320ksps
    displayDrawMenu();
    checkButtonRepeat();
  }
  Ticker ticker100ms(ticker100msFcn, 100, 0, MILLIS);
#endif

void setup() {
  // initialize peripherals
  pinMode(LED_BUILTIN, OUTPUT); // red color on RGB-LED of Vidor4000

  // initialize communication
  // Serial for communication via USB
  Serial.begin(115200);
  Serial.setTimeout(1000); // Timeout for commands

  // initialize VUmeter
  #if USE_VUMETER == 1
    initVUmeter();
  #endif

  // initialize Spectrum (together with ADC in free-running-mode)
  #if USE_SPECTRUM == 1
    initSpectrum();
  #endif

  #if USE_DISPLAY == 1
    // init I2C
    Wire.begin();
    Wire.setClock(400000); // max. 400000Hz
    initExternalGPIO();
    initADC();
    initDisplay();

    displayShowLogo(1); //  show main logo
    //displayText(5, F("Init FPGA..."));
  #endif
  
  // initialize FPGA
  setup_fpga();
  delay(1000); // give FPGA a second to startup

  #if USE_DISPLAY == 1
    initDisplay(); // init display again after uploading data to FPGA
    displayShowLogo(1); // show main logo again
    //displayText(5, F("Init MainCtrl..."));
  #endif

  #if USE_SERIAL1 == 1
    Serial1.begin(SERIAL1_BAUDRATE);
    Serial1.setTimeout(1000); // Timeout for commands
  #endif

  // Serial2 for communication with NINA-module
  Serial2.begin(115200);
  Serial2.setTimeout(1000); // Timeout for commands
  pinPeripheral(0, PIO_SERCOM); //Assign TX function to pin 0
  pinPeripheral(1, PIO_SERCOM); //Assign RX function to pin 1
  Serial2.flush();
  delay(1000); // give NINA some time to startup

  // start mainsystem
  Serial.println("f/bape USBCtrl " + String(versionstring) + " | " + String(compile_date)); // send to USB
  Serial.println(F("Init MainCtrl..."));
  #if USE_SERIAL1 == 1
    Serial1.println("f/bape USBCtrl " + String(versionstring) + " | " + String(compile_date)); // send to USB
    Serial1.println(F("Init MainCtrl..."));
  #endif
  Serial2.println(F("system:init")); // initialize main-system
  
  #if USE_DISPLAY == 1
    // show main-menu
    displayDrawMenu();

    // start ticker
    ticker100ms.start();
  #endif
}

void loop() {
  // main loop
  refreshCounter += 1;

  // handle communication
  if (firmwareUpdateMode) {
    if (refreshCounter == 12000) { // fast blinking to indicate upload-readyness
      refreshCounter = 0;
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    handleNINAUpdate();
  }else{
    if (refreshCounter == 24000) { // medium-fast blinking to indicate standby
      refreshCounter = 0;
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    handleUSBCommunication(); // communication through Serial (USB)
    #if USE_SERIAL1 == 1
      handleSerial1Communication(); // communication through Serial1
    #endif
    handleNINACommunication(); // communication through Serial2

    #if USE_VUMETER == 1
      updateVUmeter(); // redraw the VUmeter
    #endif

    #if USE_SPECTRUM == 1
      updateSpectrum(); // redraw the Spectrum
    #endif

    #if USE_DISPLAY == 1
      ticker100ms.update();
    #endif
  }
}
