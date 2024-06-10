#if USE_VUMETER == 1
  void initVUmeter() {
    ledPixels.begin();
    ledPixels.clear();

    // set all pixels to dark blue
    for (int i=0; i<LED_STRIPE_NUMPIXELS; i++) {
      ledPixels.setPixelColor(i, ledPixels.Color(0, 0, 15));
    }

    ledPixels.show();
  }

  void updateVUmeter() {
    vuMeterUpdateCounter += 1;
    if (vuMeterUpdateCounter > 1000) {
      vuMeterUpdateCounter = 0;

      // if necessary, convert to dBfs
      //float magnitude_dBfs = 20.0f * (log10(vuMeterVolumeLeft / 255.0f)); // vuMeterVolumeLeft = [0...255.0]
      //ledBar(dBfs2pu(magnitude_dBfs, -50));
      ledBar(vuMeterVolumeLeft / 255.0f); // NINA sends already in logarithmic dBfs between 0 and 255.0f
      ledPixels.show();
    }
  }

  void ledBar(float pu) {
    // this function will distribute a value between 0.0 and 1.0 to x pixels
    float value;
    uint8_t pixelvalue;

    #if USE_VUMETER_DECAY == 1
      float valuemax = pu * LED_STRIPE_NUMPIXELS;

      // add higher value with a small factor to allow a smoother transition
      // if value is smaller, we will use a logarithmic decay
      if (valuemax >= vuMeterValue) {
        //vuMeterValue = valuemax; // update immediately
        //vuMeterValue = vuMeterValue*0.7 + valuemax*0.3; // slow update -> smoother transition
        vuMeterValue = vuMeterValue + (valuemax - vuMeterValue)*0.2f; // slow update -> smoother transition
      }else{
        //vuMeterValue = vuMeterValue - 0.1; // linear fading
        vuMeterValue = vuMeterValue * vumeter_scalar; // exponential falling
      }
    #else
      vuMeterValue = pu * LED_STRIPE_NUMPIXELS; // direct copy of the value
    #endif

    // now show the valuer in the VUmeter
    for (int i = 0; i<LED_STRIPE_NUMPIXELS; i++) {
      value = vuMeterValue - i;
      pixelvalue = saturate(value*255, 0, 255);

      // green bars with red dots
      if ((value>0.0) && (value<=1.0)) {
        // this is the last pixel -> use red depending on value and reduce blue
        //ledPixels.setPixelColor(i, ledPixels.Color(pixelvalue, 0, 100-(pixelvalue/2.55f)));
        ledPixels.setPixelColor(i, ledPixels.Color(pixelvalue, 0, saturate(100-(pixelvalue-20), 0, 100)));
      }else if (value>1){
        // this is a pixel within the bar

        // check, if we are within the bar or the last element before red
        if (value < 2.0) {
          // last element before red -> next lines will fade the pixels to allow yellow depending on the value
          pixelvalue = saturate((value-1)*255, 0, 255);
          ledPixels.setPixelColor(i, ledPixels.Color(255-pixelvalue, pixelvalue, 0));
        }else {
          // within the bar -> full green
          ledPixels.setPixelColor(i, ledPixels.Color(0, 255, 0));
        }
      }else{
        // out of bar -> draw blue pixel
        ledPixels.setPixelColor(i, ledPixels.Color(0, 0, 100));
      }
    }
  }
#endif