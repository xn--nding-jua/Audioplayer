#if USE_DISPLAY == 1
  bool initADC() {
    // intialize 12 bit
    analogReadResolution(12);
  }

  void readADC() {
    uint16_t potiRaw = analogRead(A0);

    potiValue = saturate_f(((float)potiRaw / 3685.0f) - 0.05, 0.0, 1.0); // potentiometer between 0 and 3V3. Make sure, that we reach 0 and 1

    // check if value of poti has changed. We have a 16-bit ADC. Lets check for changes of 5 bit = 32
    if ((potiRaw < (potiRawOld - 32)) || (potiRaw > (potiRawOld + 32))) {
      // change detected
      menuValueChange(potiValue, true, (potiRaw < (potiRawOld - 32)));
    }
  
    potiRawOld = potiRaw; // store current value for next call
  }

  void setPotiOnValue() {
    potiOnValue = true;
    enablePotiLED();
  }

  void resetPotiOnValue() {
    potiOnValue = false;
    disablePotiLED();
  }
#endif