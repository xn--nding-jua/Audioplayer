#if USE_SPECTRUM == 1
  void initSpectrum() {
    // init LEDmatrix
    ledMatrix.begin();
    ledMatrix.setBrightness(20);

    initLedMatrixADC();
  }

  void updateSpectrum() {
    if (flagSamplingCompleted) {
      c += 1;

      // copy adc-values to FFT-array
      for (int i=0; i<samples; i++) {
        realComponent[i] = adc_values[i];
        imagComponent[i] = 0;
      }
      // initialize FFT-library
      FFT = ArduinoFFT<double>(realComponent, imagComponent, samples, samplingFrequency);
      // remove DC-parts
      FFT.dcRemoval(); // removes the DC-part of the signal
      // perform some windowing using Hamming-Window
      FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      // compute the FFT
      FFT.compute(FFT_FORWARD);
      // convert results to magnitude
      FFT.complexToMagnitude();

      // do something with the calculated FFT-values
      //Serial.print("Sample = " + String(c) + " | Peak = " + String(FFT.MajorPeak()) + " Hz");

      uint16_t frequency;
      float magnitude_dBfs = 0;
      for (int i=0; i<LED_MATRIX_WIDTH; i++) {
        frequency = ((i * samplingFrequency) / samples);
        magnitude_dBfs = 20.0f * (log10(realComponent[i] / 1024.0f)); // Fullscale = 10 bit = 2^10 = 1024
        ledMatrixBar(i, dBfs2pu(magnitude_dBfs, -50));

        //Serial.print(frequency + " Hz = " + magnitude_dBfs + " dBfs | ");
      }

      ledMatrix.show();

      // set flag for new data
      flagSamplingCompleted = false;
    }
  }

  void ledMatrixBar(uint8_t column, float pu) {
    // this function will distribute a value between 0.0 and 1.0 to x pixels
    float value;
    uint8_t pixelvalue;
    float valuemax = pu * LED_MATRIX_HEIGHT;

    // add higher value with a small factor to allow a smoother transition
    // if value is smaller, we will use a logarithmic decay
    if (valuemax >= spectrum[column]) {
      //spectrum[column] = valuemax; // update immediately
      //spectrum[column] = spectrum[column]*0.7 + valuemax*0.3; // slow update -> smoother transition
      spectrum[column] = spectrum[column] + (valuemax - spectrum[column])*0.2f; // slow update -> smoother transition
    }else{
      //spectrum[column] = spectrum[column] - 0.1; // linear fading
      spectrum[column] = spectrum[column] * spectrum_scalar; // exponential falling
    }

    // now show the column in the matrix
    for (int i = 0; i<LED_MATRIX_HEIGHT; i++) {
      value = spectrum[column] - i;
      pixelvalue = saturate(value*255, 0, 255);

      // green bars with red dots
      if ((value>0.0) && (value<=1.0)) {
        // this is the last pixel -> use red depending on value and reduce blue
        //ledMatrix.drawPixel(i, column, ledMatrix.Color(pixelvalue, 0, 100-(pixelvalue/2.55f)));
        ledMatrix.drawPixel(i, column, ledMatrix.Color(pixelvalue, 0, saturate(100-(pixelvalue-20), 0, 100)));
      }else if (value>1){
        // this is a pixel within the bar

        // check, if we are within the bar or the last element before red
        if (value < 2.0) {
          // last element before red -> next lines will fade the pixels to allow yellow depending on the value
          pixelvalue = saturate((value-1)*255, 0, 255);
          ledMatrix.drawPixel(i, column, ledMatrix.Color(255-pixelvalue, pixelvalue, 0));
        }else {
          // within the bar -> full green
          ledMatrix.drawPixel(i, column, ledMatrix.Color(0, 255, 0));
        }
      }else{
        // out of bar -> draw blue pixel
        ledMatrix.drawPixel(i, column, ledMatrix.Color(0, 0, 100));
      }
    }
  }

  void initLedMatrixADC() {
    // for more details see:
    // https://forcetronic.blogspot.com/2018/06/speeding-up-adc-on-arduino-samd21.html
    // https://github.com/arduino/ArduinoCore-samd/blob/master/cores/arduino/wiring_analog.c
    // https://forcetronic.blogspot.com/2016/10/utilizing-advanced-adc-capabilities-on.html

    // Init ADC free-run mode; f = ( 48MHz/prescaler ) / 7 cycles/conversion
    // for prescaler of 256: (48MHz/256) / 7 = 26786 Hz


    // Configure pin configuration ------------------------
    #if USE_SPECTRUM_ANALOG_INPUT == 0
      // Input pin for ADC Arduino PA04
      REG_PORT_DIRCLR1 = PORT_PA02;
      //PORT->Group[PORTA].DIRCLR = PORT_PA02;
      // Enable multiplexing on PA02
      PORT->Group[PORTA].PINCFG[PIN_PA02].bit.PMUXEN = 1;
      PORT->Group[PORTA].PMUX[1].reg = PORT_PMUX_PMUXE_B | PORT_PMUX_PMUXO_B; // select pin for ADC
    #elif USE_SPECTRUM_ANALOG_INPUT == 4
      // Input pin for ADC Arduino PA04
      REG_PORT_DIRCLR1 = PORT_PA04;
      //PORT->Group[PORTA].DIRCLR = PORT_PA04;
      // Enable multiplexing on PA04
      PORT->Group[PORTA].PINCFG[PIN_PA04].bit.PMUXEN = 1;
      PORT->Group[PORTA].PMUX[1].reg = PORT_PMUX_PMUXE_B | PORT_PMUX_PMUXO_B; // select pin for ADC
    #endif




    // Configure clock source and clock generators (gclk.h)------------------------   

    // we could use an internal 8MHz oscillator, but we will use the 48MHz main-clock
    //SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE; // enable the 8MHz clock used for the ADC

    GCLK->GENDIV.reg =  GCLK_GENDIV_ID(3) |   // Select GLCK3
                        GCLK_GENDIV_DIV(1);   // Select prescaler 1
    while (GCLK->STATUS.bit.SYNCBUSY);        // Wait for SYNC

    //GCLK->GENCTRL.reg |= GCLK_GENCTRL_GENEN | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_ID(3) | GCLK_GENCTRL_DIVSEL; // use 8MHz clock
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(3) |        // Select GCLK3
                        GCLK_GENCTRL_SRC_DFLL48M |  // Select GCLK SRC
                        GCLK_GENCTRL_IDC |          // Improve duty cycle for odd div factors
                        GCLK_GENCTRL_GENEN;         // Enable GCLK5 
    while (GCLK->STATUS.bit.SYNCBUSY);        // Wait for SYNC

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(3) |  // Select GLCK3
                        GCLK_CLKCTRL_ID_ADC |     // Connect to ADC
                        GCLK_CLKCTRL_CLKEN;
    while (GCLK->STATUS.bit.SYNCBUSY);        // Wait for SYNC
    



    // Configure power manager (pm.h)-------------------------------------------
    PM->APBCMASK.reg |= PM_APBCMASK_ADC;   // Setup Power manager for ADC peripheral
    PM->APBCSEL.reg = PM_APBCSEL_APBCDIV_DIV1; // Select prescaler 1




    // ADC configuration (adc.h) ----------------------------------------------
    ADC->CTRLA.reg &= ~ADC_CTRLA_ENABLE; // Disable ADC peripheral
    while(ADC->STATUS.bit.SYNCBUSY);  // Wait of sync
    
    ADC->REFCTRL.reg |= ADC_REFCTRL_REFSEL_INTVCC1; // Use the internal VCC reference. This is 1/2 of what's on VCCA. Since VCCA is typically 3.3v, this is 1.65v. Combine with gain DIV2 for full VCC range
    while(ADC->STATUS.bit.SYNCBUSY);

    ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1; // only capture one sample. The ADC can actually capture and average multiple samples for better accuracy, but there's no need to do that here
    while(ADC->STATUS.bit.SYNCBUSY);
    
    ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(0); // Sampling time, no extra sampling half clock-cycles
    while(ADC->STATUS.bit.SYNCBUSY);

    // Set the clock prescaler to 256, which will run the ADC at 48 Mhz / 256 = 187.5 kHz. Set the resolution to 12bit. Set ADC to FreeRun-Mode
    ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV256 | // ADC CLK prescaler 256
                      ADC_CTRLB_RESSEL_12BIT | // 12-bit ADC result
                      ADC_CTRLB_FREERUN; // ADC free run mode                     
    while(ADC->STATUS.bit.SYNCBUSY);

    ADC->WINCTRL.reg = ADC_WINCTRL_WINMODE_DISABLE; // Disable window monitor mode
    while(ADC->STATUS.bit.SYNCBUSY);

    ADC->EVCTRL.reg |= ADC_EVCTRL_STARTEI; //start ADC when event occurs
    while (ADC->STATUS.bit.SYNCBUSY);




    #if USE_SPECTRUM_ANALOG_INPUT == 0
      // Configure the input parameters
      ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2 | // ADC gain factor x0.5 to acchieve full VCC range
                            ADC_INPUTCTRL_INPUTSCAN(0x0)| // Number of channels included in scan
                            ADC_INPUTCTRL_MUXPOS_PIN2 | // Positive MUX input selection. See SAMD21 Datasheet's "Port Function Multiplexing" table, in section (chapter) 7 and Chapter 33.8.8 Input Control
                            ADC_INPUTCTRL_MUXNEG_GND; // ADC should compare the input value to GND
      while(ADC->STATUS.bit.SYNCBUSY);
    #elif USE_SPECTRUM_ANALOG_INPUT == 4
      // Configure the input parameters
      ADC->INPUTCTRL.reg = ADC_INPUTCTRL_GAIN_DIV2 | // ADC gain factor x0.5 to acchieve full VCC range
                            ADC_INPUTCTRL_INPUTSCAN(0x0)| // Number of channels included in scan
                            ADC_INPUTCTRL_MUXPOS_PIN4 | // Positive MUX input selection. See SAMD21 Datasheet's "Port Function Multiplexing" table, in section (chapter) 7 and Chapter 33.8.8 Input Control
                            ADC_INPUTCTRL_MUXNEG_GND; // ADC should compare the input value to GND
      while(ADC->STATUS.bit.SYNCBUSY);
    #endif




    // Configure ADC interrupts ------------------------------------------
    ADC->INTENCLR.reg = ADC_INTENCLR_RESRDY; // Clear result ready interrupt
    while(ADC->STATUS.bit.SYNCBUSY);

    ADC->INTENSET.reg = ADC_INTENSET_RESRDY; // Set result ready interrupt
    while(ADC->STATUS.bit.SYNCBUSY);

    NVIC_EnableIRQ(ADC_IRQn);         // Connect ADC to Nested Vector Interrupt Controller (NVIC)
    NVIC_SetPriority(ADC_IRQn, 3);    // Set the Nested Vector Interrupt Controller (NVIC) priority for TCC1 to 0 (highest) or lower values




    // Enable ADC--------------------------------------------------------------
    ADC->CTRLA.reg = ADC_CTRLA_ENABLE; // Enable ADC peripheral
    while(ADC->STATUS.bit.SYNCBUSY);  // Wait of sync

    ADC->SWTRIG.bit.START = 1; // Start ADC conversion
    while(ADC->STATUS.bit.SYNCBUSY); // Wait for sync


    // start first conversion via software. From then on the ADC will run in FreeRun-Mode continuously
    ADC->SWTRIG.bit.START = 1;
  }

  // interrupt service routine (ISR) for ADC
  void ADC_Handler() {
    if (ADC->INTFLAG.bit.RESRDY) {
      if (!flagSamplingCompleted) {
        adc_values[sample_counter] = ADC->RESULT.reg;

        sample_counter += 1;
        if (sample_counter >= samples) {
          sample_counter = 0;
          flagSamplingCompleted = true;
        }
      }

      // reset interrupt flag
      ADC->INTFLAG.bit.RESRDY = 1;
    }
  }
#endif