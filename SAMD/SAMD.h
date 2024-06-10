const char* versionstring = "v1.6.0";
const char compile_date[] = __DATE__ " " __TIME__;

// the following defines will overwrite the standard arduino-defines from https://github.com/arduino/ArduinoCore-samd/blob/84c09b3265e2a8b548a29b141f0c9281b1baf154/variants/mkrvidor4000/variant.h
//#define I2S_INTERFACES_COUNT 1 // SAMD21 has two interfaces
//#define I2S_DEVICE 0 // select if I2S device 0 or 1
//#define I2S_CLOCK_GENERATOR 3 // select the correct clock for I2S-interface
//#define PIN_I2S_SD 21 // pin for SerialData (= A6)
//#define PIN_I2S_SCK 02 // pin for bit-clock (= D2)
//#define PIN_I2S_FS 03 // pin for FrameSelect / Wordclock (= D3)

#define USE_DISPLAY       1      // enables a SSD1308 display connected to I2C together with an external GPIO-extender and external ADC for a potentiometer
#define USE_SPECTRUM      0      // enables support for a 8x8 NeoPixel Matrix with some nice effects. This is tested only on an Arduino R4 and yet not working on the SAMD21 as there seems to be a problem with the ADC-channel
#define USE_VUMETER       1      // enables support for a WS2812B LED-Stripe connected to SAMD21. Volume-Information will get received from NINA and a nice VUmeter will be displayed
#define USE_VUMETER_DECAY 1      // NINA will calculate a decay for us already, but as we are using a very small updaterate, this will look a bit better
#define USE_SERIAL1       1      // enables the usage of Serial1 on Pins 13/14 for command-interpreter (maybe for an external display with GUI? Its up to you ;-) )
#define SERIAL1_BAUDRATE  115200 // use any baudrate here you like

// includes for FPGA
#include <wiring_private.h>
#include "jtag.h"
#include "fpga.h"

#if USE_VUMETER == 1
  // includes for VUmeter
  #define LED_STRIPE_PIN 25 // pin 25 = AREF
  #define LED_STRIPE_NUMPIXELS 8
  #include "VUmeter.h"
#endif

#if USE_SPECTRUM == 1
  // includes for Spectrum
  #define LED_MATRIX_PIN 25 // pin 25 = AREF
  #define LED_MATRIX_WIDTH 8
  #define LED_MATRIX_HEIGHT 5
  #include "Spectrum.h"
#endif

// includes for Serial2 to communicate with NINA. As Serial2 is not within the scope of Arduino,
// we have to create it using the SERCOM-system of the SAMD21
#define PIN_SERIAL2_TX       (0ul)                // Pin description number for PIO_SERCOM on D0
#define PIN_SERIAL2_RX       (1ul)                // Pin description number for PIO_SERCOM on D1
#define PAD_SERIAL2_TX       (UART_TX_PAD_0)      // SERCOM pad 0 TX
#define PAD_SERIAL2_RX       (SERCOM_RX_PAD_1)    // SERCOM pad 1 RX
Uart Serial2(&sercom3, PIN_SERIAL2_RX, PIN_SERIAL2_TX, PAD_SERIAL2_RX, PAD_SERIAL2_TX);
bool passthroughNINA = false;

#if USE_DISPLAY == 1
  #include "Ticker.h"
  #include "Wire.h"
  float potiValue; // value in p.u. between 0.0 and 1.0
  int16_t potiRawOld; // old value to detect changes
  bool potiOnValue; // check if poti sits on current value

  // includes and definitions for external GPIOs
  #include "MCP23008.h"
  MCP23008 MCP(0x20); // Address is 0100AAA = 0100000 = 0x20
  struct {
    bool buttonUp;
    bool buttonRight;
    bool buttonDown;
    bool buttonLeft;
    bool buttonOK;
    bool cardPresent;
    uint8_t value;
  }externalInputs, externalInputsOld;

  enum menuNavigation {
    menuNavigationUp = 1,
    menuNavigationDown,
    menuNavigationLeft,
    menuNavigationRight,
    menuNavigationOK
  };
  uint8_t buttonPressed = 0;
  uint8_t buttonPressedCounter = 0;

  // includes for display
  #include "Bitmaps.h"
  #include <Adafruit_GFX.h>
  #include <Adafruit_SSD1306.h>
  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  #define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
  #define SCREEN_ADDRESS 0x3C // this address does not fit to the address on the PCB!!!
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
  #define NUMBER_OF_MENUS 6
  #define NUMBER_OF_MENUOPTIONS 5
  uint8_t currentDisplayMenu = 0;
  uint8_t currentDisplayLine = 0;
  uint8_t currentDisplayOption = 0;
  String TOC;
  uint8_t TOC_Offset = 0;

  struct {
    String title = "Standby...";
    float volumeMain;
    uint8_t balanceMain;
    float volumeSub;
    float volumeAnalog;
    float volumeCard;
    float volumeBass;
    uint32_t time;
    uint32_t duration;
    uint8_t progress;
    bool WiFiEnabled = 1;
    bool BluetoothEnabled = 0;
    String MainCtrlVersion;
    String FPGAVersion;
    uint32_t samplerate;
  }playerinfo;

#endif

// some defines for NINA update mode
#define NINA_PIO27 11
#define NINA_RESET_N 31
unsigned long baud = 115200;
int rts = -1;
int dtr = -1;
bool firmwareUpdateMode = false;

// general variables
uint32_t refreshCounter = 0;