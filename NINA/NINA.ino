/*
  f/bape MainCtrl for Arduino MKR Vidor4000 Device
  v1.6.0 built on 10.06.2024
  Infos: https://www.github.com/xn--nding-jua/Audioplayer
  Copyright (c) 2023-2024 Dr.-Ing. Christian Nöding

  Target-Board: uBlox NINA W10-series (ESP32)
  Board-package: https://espressif.github.io/arduino-esp32/package_esp32_index.json
  Flash-Frequency: 80 MHz
  Flash-Size: 2MB (16Mb, NINA W101/102)
  Arduino runs on: Core 1
  Partition Scheme: "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"
  Important: a specific, self-defined partition-scheme from partitions.csv within the NINA-folder will be used. If you want to use a different ESP32-chip, you have to delete or edit this file
  Upload Speed: up to 921600 is tested
  Current Flash-Usage: 1,441,909 bytes (73%) of program storage space. Maximum is 1,966,080 bytes.

  Description:
  =====================================================================
  This project runs on an ESP32 (NINA-W102-Module) and implements an audio-player with
  full-qualified interactive webserver using AJAX-technology for smooth update of the
  webinterface.
  The software implements Bluetooth-A2DP, SD-Card-Playback, MQTT, TCP-commandu-server,
  equalizer- and crossover-calculation and communication between FPGA and a USB-controller.

  Used libraries:
  =====================================================================
  - ESP32 Board-Libraries v3.0.0 (https://dl.espressif.com/dl/package_esp32_index.json)
  - SD-Card-Playback: ESP32-audioI2S v2.0.0 (https://github.com/schreibfaul1/ESP32-audioI2S)
  - Ticker by Stefan Staub
  - PubSubClient by Nick O'Leary
  - SimpleFTPServer by Renzo Mischianti [change SD-settings in FtpServerKey.h if you have trouble connecting to SD-card]
      Alternative: ESP-FTP-Server-Lib by Peterus (https://github.com/peterus/ESP-FTP-Server-Lib) [supports only ACTIVE ftp-mode]
  - Bluetooth-Sink: ESP32-A2DP v1.8.1 by P. Schatzmann (https://github.com/pschatzmann/ESP32-A2DP)
  - AudioTools for Bluetooth-Playback: ArduinoAudioTools v0.9.8 by P. Schatzmann (https://github.com/pschatzmann/arduino-audio-tools)

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
  +-------------+----------------+----------+------------+---------------+------------------------+---------------------------------------+
  | Arduino Pin | Arduino Analog | NINA Pin |  ESP32 Pin | FPGA Pin      |  Property              | Notes                                 |
  +-------------+----------------+----------+------------+---------------+------------------------+---------------------------------------+
  |             |                |          |    GPIO    |               |                        |                                       |
  |             |                | RESET_N  | RESET      | R1            | Reset-Pin              | RESET for NINA/ESP32                  |
  | D12         |                | PIO1     | 23         | T11           | SPI-MOSI               | SPI-MOSI (SD-Card)                    |
  | D19         | A6             | PIO2     | 34         |               |                        |                                       |
  | D21         | A3             | PIO3     | 39         |               |                        |                                       |
  | D20         | A0             | PIO4     | 36         |               |                        |                                       |
  | D16         | A4             | PIO5     | 32         | N6            |                        | SD I2S SerialClock                    |
  | D17         | A5             | PIO7     | 33         | P6            |                        | SD I2S WordClock                      |
  | D18         |                | PIO8     | 21         | N5            |                        | SD I2S SerialData                     |
  | D3          | A18            | PIO16    | 25         |               | DAC2 / LED Blue        |                                       |
  | D2          | A19            | PIO17    | 26         |               | DAC1 / LED Green       |                                       |
  | D5          | A17            | PIO18    | 27         | T5            |                        | -> UART1 TxD to FPGA                  |
  | D6          |                | PIO20    | 22         | R5            |                        | <- UART1 RxD to FPGA                  |
  | D11         |                | PIO21    | 19         | R6            | SPI-MISO               | SPI-MISO (SD-Card)                    |
  | D1          |                | PIO22    | 01         | E15           | SCI-TxD                | -> UART0 TxD to SAMD21                |
  | D0          |                | PIO23    | 03         | T6            | SCI-RxD                | <- UART0 RxD from SAMD21              |
  |             | A10            | PIO24    | 04         |               |                        |                                       |
  |             | A12            | PIO25    | 02         |               |                        |                                       |
  | D7          | A11            | PIO27    | 00         | N9            |                        | BOOT-Pin to select Bootloader-Mode    |
  | D10         |                | PIO28    | 05         | N11           | SPI-SS                 | SPI-CS (SD-Card)                      |
  | D13         |                | PIO29    | 18         | T10           | SPI-SCK                | SPI-SCK (SD-Card)                     | 
  | D9          | A16            | PIO31    | 14         | T4            |                        |                                       |
  | D8          | A13            | PIO32    | 15         | J13           |                        | BT I2S SerialClock                    | ! pin controls Uart0 TxD Debug during boot
  | D4          | A7             | PIO34    | 35         |               |                        | BT I2S MClk                           |
  | D14         | A14            | PIO35    | 13         | R4            | I2C-SCL                | BT I2S WordClock                      |
  | D15         | A15            | PIO36    | 12         | N1            | I2C-SDA                | BT I2S SerialData                     | ! pin controls flash-voltage during boot
  +-------------+----------------+----------+------------+---------------+------------------------+---------------------------------------+

  Used files for Webserver:
  =====================================================================
  - Bootstrap-Theme v5.1.3: https://getbootstrap.com
  - Free Bootstrap-Theme: Bootswatch Cyborg: https://bootswatch.com/cyborg/
  - VUmeter: https://github.com/tomnomnom/vumeter
  - Smoothie-Charts: http://smoothiecharts.org
  - RoundSlider: https://roundsliderui.com
  - Plotly: https://plotly.com
  - jQuery: https://jquery.com
  - FontAwesome: https://fontawesome.com
  - Bootstrap for ESP https://techtutorialsx.com/2019/03/04/esp32-arduino-serving-bootstrap/
  - fetch API https://werner.rothschopf.net/201809_arduino_esp8266_server_client_2.htm
*/

#include "NINA.h"

void timerSecondsFcn() {
  digitalWrite(LED_GREEN, !digitalRead(LED_GREEN)); // toggle green led

  #if USE_MQTT == 1
    mqttPublish();
  #endif
  
  #if USE_DISPLAY == 1
    updateSAMDDisplay(); // update the display of the SAMD with current information
  #endif
}

void timer100msFcn() {
  updateTimer--;

  if (updateTimer == 0) {
    // reload updateTimer to 10x 100ms = 1 second
    updateTimer = 10;
  }

  if (LEDHoldCounter>0) {
    LEDHoldCounter--;

    if (LEDHoldCounter == 0) {
      // turn off blue LED
      LEDFadeCounter = 0; // set counter to zero
      //digitalWrite(LED_BLUE, HIGH); // hard turn off
    }
  }
  if (LEDFadeCounter<255) {
    // fade LED off

    if (LEDFadeCounter >= 250) { // 255/10
      // make sure to count to full 255
      LEDFadeCounter = 255;
    }else{
      LEDFadeCounter+=25; // to get 10x0.1s = 1 second = 255/10
    }
    analogWrite(LED_BLUE, LEDFadeCounter);
  }

  #if USE_SDPLAYER == 1
    // check audio-samplerate and update sample-rate if nescessary
    if (audio.isRunning() && (audio.getSampleRate() != audiomixer.sampleRate)) {
      // update the current samplerate
      setSampleRate(audio.getSampleRate());
    }
  #endif
}

void setup() {
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  analogWrite(LED_BLUE, 255); // turn off blue LED
  digitalWrite(LED_GREEN, LOW); // turn on green LED

  // init communication with SAMD21/USB
  Serial.begin(115200, SERIAL_8N1, 3, 1, false, 1000); // BaudRate, Config, RxPin, TxPin, Invert, TimeoutMs, rxfifo_full_thrhd

  // init communication with FPGA
  Serial1.begin(4000000, SERIAL_8N1, 22, 27, false, 1000); // BaudRate, Config, RxPin, TxPin, Invert, TimeoutMs, rxfifo_full_thrhd
}

void loop() {
  if (systemOnline) {
    // handle incoming messages from FPGA
    handleFPGACommunication(); // communication through software-serial

    // handle webserver
    webserver.handleClient();

    #if USE_TCPSERVER == 1
      handleTCPCommunication();
    #endif

    #if USE_MQTT == 1
      handleMQTT();
    #endif

    #if USE_FTP_SERVER == 1
      ftp.handleFTP();
    #elif USE_FTP_SERVER == 2
      ftp.handle();
    #endif
  }

  // handle incoming messages via serial-port
  handleUartCommunication(); // communication through hardware-serial
}

void initSystem() {
  // request identification from SAMD21
  Serial.println("samd:*IDN?");

  #if USE_DISPLAY == 1
    // send version information to SAMD21
    Serial.println("samd:version:main@" + String(versionstring)); // send MainCtrl version to SAMD21
  #endif
  
  // activate passthrough via SAMD21 to USB
  Serial.println("samd:passthrough:nina@1");
  delay(100);
  // send welcome-text to USB
  Serial.println("f/bape MainCtrl " + String(versionstring) + " built on " + String(compile_date));

  // init SD-Card
  initSD();

  // initWifi
  configRead("/wifi.cfg", 10); // read WiFi-settings with maximum of 10 lines (if file is available)
  initWifi(); // initialize WiFi

  /*
  // Bluetooth cannot coexist with our WiFi at the moment
  // so we will activate bluetooth manually with following commands
  // wifi:enabled@0
  // bt:enabled@1
  #if USE_BLUETOOTH == 1
    // init Bluetooth
    initBluetooth();
  #endif
  */

  #if USE_SDPLAYER == 1
    // init Audioplayer
    initAudio();
  #endif

  // init Webserver
  initWebserver(); // starting regular HTTP-server

  // init command-server
  #if USE_TCPSERVER == 1
    cmdserver.begin();
  #endif

  #if USE_MQTT == 1
    initMQTT();
  #endif

  #if USE_FTP_SERVER == 1
    ftp.begin(FTP_USER, FTP_PASSWORD);
  #elif USE_FTP_SERVER == 2
    ftp.addUser(FTP_USER, FTP_PASSWORD);
    ftp.addFilesystem("SD", &SD); // Alternative: ftp.addFilesystem("SPIFFS", &SPIFFS);
    ftp.begin();
  #endif

  // Initiate the timers
  TimerSeconds.attach_ms(1000, timerSecondsFcn);
  Timer100ms.attach_ms(100, timer100msFcn);

  // initialize the Audiomixer. Try to read configuration from SD
  //if (!configRead("/fbape.cfg", 100)) { // read config-file with maxmimum of 100 lines - uncomment if you like to load configuration-file on startup
    // reading config-file failed -> load defaults
    initAudiomixer();
  //}

  // set system-flag to online
  systemOnline = true;
}