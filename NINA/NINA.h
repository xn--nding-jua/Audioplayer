const char* versionstring = "v1.6.0";
const char compile_date[] = __DATE__ " " __TIME__;

String hostname = "fbape";
#define FTP_USER "fbape"
#define FTP_PASSWORD "fbape"

// Setup the WiFi
// Accesspoint-Mode
bool WiFiAP = true;
String ssid     = "fbape";
String password = "f/bapePASSWORD"; // at least 8 characters
/*
// Client-Mode
bool WiFiAP = false;
String ssid     = "YourVeryOriginalSSID";
String password = "YourVerySafeWiFiPassword";
*/

// configure the system
#define MAX_AUDIO_CHANNELS  22  // the number of audio-channels must match the number of channels in the FPGA
#define MAX_EQUALIZERS      5   // number of PEQ must match number of PEQ in SAMD21 and FPGA
#define MAX_ADCS            1
#define MAX_NOISEGATES      1
#define MAX_COMPRESSORS     2
#define USE_BLUETOOTH       0   // enable the bluetooth-A2DP-receiver (takes ~ 741,104 bytes)
#define USE_SDPLAYER        1   // enable or disable SD-Card-Player-functions as the ESP32 has not enough space for both SD-Card-Playback and Bluetooth
#define USE_STATICIP        0   // only for client-mode. In AccessPoint-Mode it will use static-IP and ignores this options
#define USE_TCPSERVER       1   // enable TCP-Server (takes ~ 10029 bytes)
#define USE_MQTT            1   // enable MQTT (takes ~ 8360 bytes)
#define USE_FTP_SERVER      1   // enable FtpServer for uploading/deleting/editing files
#define USE_DISPLAY         1   // enable functions for I2C display connected to SAMD21 (takes ~ 384 bytes)
#define USE_VUMETER         1   // enable functions for VUmeter (WS2812B) connected to SAMD21

#define FPGA_IDX_MAIN_VOL   0   // main-volume
#define FPGA_IDX_CH_VOL     3   // first channel-volume
#define FPGA_IDX_XOVER      70	// cross-over
#define FPGA_IDX_PEQ        88	// parametric equalizers
#define FPGA_IDX_GATE       150	// noisegates
#define FPGA_IDX_COMP       175	// compressors
#define FPGA_IDX_AUX_CMD    200	// auxiliary commands
#define FPGA_IDX_ADC_GAIN   220	// adc gains

#include "NINA_Pinmap.h"        // define the pins for this project

// setup bluetooth
#if USE_BLUETOOTH == 1
  #define BT_I2S_MCK        NINA_PIO34 // will not be used but we have to define it
  #define BT_I2S_DOUT       NINA_PIO36
  #define BT_I2S_BCLK       NINA_PIO32
  #define BT_I2S_LRC        NINA_PIO35
  #define BT_BITS           16  // select 16 or 32 bit. Please change receiver-block in FPGA to this bitrate
#endif

// setup SD-Card
  #define SPI_CLOCK           16000000    // ESP32 supports clks up to 80 MHz (on good cables), Audio library limits to 25MHz. 16MHz seems to be enought for MP3s, for WAV use at least 20MHz
  #define AUDIO_INIT_VOLUME   21          // 0...21, so set to max on initialization
  #define SD_CS               NINA_PIO28
  #define SPI_MOSI            NINA_PIO1
  #define SPI_MISO            NINA_PIO21
  #define SPI_SCK             NINA_PIO29
  String currentAudioFile;
  uint32_t currentAudioPosition;
#if USE_SDPLAYER == 1
  #define I2S_DOUT            NINA_PIO8
  #define I2S_BCLK            NINA_PIO5
  #define I2S_LRC             NINA_PIO7
#endif

// include official Arduino-headers
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <Ticker.h> // timer-functions
#include <stdlib.h> // atoi()

// IP-Address of Access-Point
IPAddress ip(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 0, 1);
IPAddress secondaryDNS(1, 1, 1, 1);

#if USE_BLUETOOTH == 1
  // setup bluetooth
  #include "AudioTools.h"
  #include "BluetoothA2DPSink.h" // bluetooth-functions
  I2SStream i2s;
  BluetoothA2DPSink a2dp_sink(i2s);
#endif

#include "SPI.h"
#include "SD.h"
#include "FS.h"
#if USE_SDPLAYER == 1
  // includes for Audiosystem and SD-Card-Playback
  #include "Audio.h"
  Audio audio;
  TaskHandle_t AudioTask;
#endif

// initializing individual tools
WebServer webserver(80); // Webserver
#if USE_TCPSERVER == 1
  WiFiServer cmdserver(5025); // command-server
  WiFiClient cmdclient;
#endif
File configFile;

#if USE_MQTT == 1
  #define mqtt_id "fbape"
  IPAddress mqtt_server(192, 168, 0, 24);
  #define mqtt_serverport 1883

  #include <PubSubClient.h>

  WiFiClient mqttnetworkclient;
  PubSubClient mqttclient(mqttnetworkclient);
#endif

#if USE_FTP_SERVER == 1
  #include <FtpServerKey.h>
  // redefine the SD-Card Settings
  #define DEFAULT_STORAGE_TYPE_ESP32 STORAGE_SD
  // include the library
  #include <SimpleFTPServer.h>

  FtpServer ftp;
#elif USE_FTP_SERVER == 2
  #include "ESP-FTP-Server-Lib.h"
  #include "FTPFilesystem.h"

  FTPServer ftp;
#endif

// includes for Ticker
#include <Ticker.h>
Ticker TimerSeconds;
Ticker Timer100ms;
uint8_t updateTimer = 50;

// general variables
bool systemOnline = false;
String USBCtrlIDN = "0";
String FPGA_Version = "0";
bool debugRedirectFpgaSerialToUSB = false;
uint8_t fpgaRxBuffer[11];

// setup audiosystem
uint8_t LEDHoldCounter = 0;
uint8_t LEDFadeCounter = 255;

// vu-meter
#define vumeter_halfLife 0.25f // seconds
#define vumeter_sampleRate 20.0f // effective samplerate for VuMeter (FPGA sends 20 messages per second)
float vumeter_decay = pow(0.5f, 1.0f / (vumeter_halfLife * vumeter_sampleRate));
float vu_meter_value[3]; // vu-meter for left, right and sub

// define own datatypes
typedef union 
{
    uint64_t u64;
    int64_t s64;
    uint32_t u32[2];
    int32_t s32[2];
    uint16_t u16[4];
    int16_t s16[4];
    uint8_t u8[8];
    int8_t s8[8];
    double d;
}data_64b;

typedef union 
{
    uint32_t u32;
    int32_t s32;
    uint16_t u16[2];
    int16_t s16[2];
    uint8_t u8[4];
    int8_t s8[4];
    float   f;
}data_32b;

typedef union 
{
    uint16_t u16;
    int16_t s16;
    uint8_t u8[2];
    int8_t s8[2];
}data_16b;

// variables
struct sPEQ {
  // user-settings
  uint8_t type = 0; // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
  float fc = 400; // center-frequency of PEQ
  float Q = 2; // Quality of PEQ (bandwidth)
  float gain = 1; // gain of PEQ

  data_32b a[3];
  data_32b b[3];
};

struct sLR12 {
  // user-settings
  float fc = 100; // cutoff-frequency for high- or lowpass
  bool isHighpass = false; // choose if Highpass or Lowpass

  // filter-coefficients
  data_64b a[3];
  data_64b b[3];
};

struct sLR24 {
  // user-settings
  float fc = 100; // cutoff-frequency for high- or lowpass
  bool isHighpass = false; // choose if Highpass or Lowpass

  // filter-coefficients
  data_64b a[5];
  data_64b b[5];
};

struct sNoisegate {
  // user-settings
  float threshold = -80; // value between -80 dBfs (no gate) and 0 dBfs (full gate) -> 2^23..2^13.33
  float range = 48; // value between 48dB (full range) and 3dB (minimum effect) 
  float attackTime_ms = 10;
  float holdTime_ms = 50;
  float releaseTime_ms = 258;

  // filter-data
  const float audio_bitwidth = 24; // depends on implementation of FPGA
  data_32b value_threshold;
  const float gainmin_bitwidth = 8; // depends on implementation of FPGA
  data_16b value_gainmin;
  data_16b value_coeff_attack;
  data_16b value_hold_ticks;
  data_16b value_coeff_release;
};

struct sCompressor {
  // user-settings
  float threshold = 0; // value between 0 dBfs (no compression) and -80 dBfs (full compression) -> 2^23..2^13.33
  uint8_t ratio = 1; // value between 0=oo:1, 1=1:1, 2=2:1, 4=4:1, 8=8:1, 16=16:1, 32=32:1, 64=64:1
  uint8_t makeup = 0; // value between 0dB, 6dB, 12dB, 18dB, 24dB, 30dB, 36dB, 42dB, 48dB
  float attackTime_ms = 10;
  float holdTime_ms = 10;
  float releaseTime_ms = 151;

  // filter-data
  const float audio_bitwidth = 24; // depends on implementation of FPGA
  data_32b value_threshold;
  data_16b value_ratio;
  data_16b value_makeup;
  data_16b value_coeff_attack;
  data_16b value_hold_ticks;
  data_16b value_coeff_release;
};

// struct for audiomixer
struct {
  float mainVolume;
  uint8_t mainBalance;
  float mainVolumeSub;

  float chVolume[MAX_AUDIO_CHANNELS];
  uint8_t chBalance[MAX_AUDIO_CHANNELS];

  uint8_t audioSyncSource = 1; // SD-Card as standard
  uint32_t sampleRate = 48000; // 48kHz as standard

  sPEQ peq[MAX_EQUALIZERS];
  sLR24 LR24_LP_Sub; // <=100Hz
  sLR24 LR24_HP_LR; // >=100Hz

  uint16_t adcGain[MAX_ADCS];
  sNoisegate gates[MAX_NOISEGATES];
  sCompressor compressors[MAX_COMPRESSORS]; // left/right, sub
}audiomixer;
