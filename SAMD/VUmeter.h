#include <Adafruit_NeoPixel.h>

// create objects and variables for VUmeter
Adafruit_NeoPixel ledPixels(LED_STRIPE_NUMPIXELS, LED_STRIPE_PIN, NEO_GRB + NEO_KHZ800);

#if USE_VUMETER_DECAY == 1
  float vumeter_halfLife = 2.0f; // seconds
  float vumeter_sampleRate = 10.0f; // effective samplerate for VUmeter
  float vumeter_scalar = pow(0.5f, 1.0f / (vumeter_halfLife * vumeter_sampleRate));
#endif

uint16_t vuMeterUpdateCounter = 0;
float vuMeterValue; // current value of the VUmeter. With decay it will store the value a bit longer than the actual vuMeterVolume
float vuMeterVolumeLeft;  // 0...255.0
float vuMeterVolumeRight;  // 0...255.0, not used at the moment
float vuMeterVolumeSub;  // 0...255.0, not used at the moment
