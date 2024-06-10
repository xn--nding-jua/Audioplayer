#include <arduinoFFT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define USE_SPECTRUM_ANALOG_INPUT 0  // use AIN0 = Pin 15 = A0 = PA02 or AIN4 = Pin 18 = A3 = PA04

// create objects and variables for FFT
ArduinoFFT<double> FFT;
const uint16_t samples = 16; // we have only 8 columns in LED matrix, so 16 samples are enough
const double samplingFrequency = 26786; // f = ( 48MHz/prescaler ) / 7 cycles/conversion = (48MHz/256) / 7 = 26786 Hz
double realComponent[samples];
double imagComponent[samples];
volatile uint16_t adc_values[samples];
volatile uint16_t sample_counter = 0;
volatile bool flagSamplingCompleted = false;

// Spectrum values
float spectrum[LED_MATRIX_WIDTH];
float spectrum_halfLife = 2.0f; // seconds
float spectrum_sampleRate = 10.0f; // effective samplerate for Spectrum
float spectrum_scalar = pow(0.5f, 1.0f / (spectrum_halfLife * spectrum_sampleRate));

// variables and objects for LEDmatrix
Adafruit_NeoMatrix ledMatrix = Adafruit_NeoMatrix(LED_MATRIX_HEIGHT, LED_MATRIX_WIDTH, LED_MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

uint32_t c = 0;

const uint16_t colors[] = { ledMatrix.Color(255, 0, 0), ledMatrix.Color(0, 255, 0), ledMatrix.Color(0, 0, 255) };

int matrix_x    = ledMatrix.width();
int matrix_pass = 0;