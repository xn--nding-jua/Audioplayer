#if USE_DISPLAY == 1
  bool initExternalGPIO() {
    MCP.begin();
    if (!MCP.isConnected()) {
      return false;
    }

    // Pins 0..4 = 5x Button
    // Pin 5 = SD Card detect
    // Pin 6 = LED: Poti on Value (PoV)
    // Pin 7 = LED: Status
    MCP.pinMode8(0b00111111); //  0 = output, 1 = input
    MCP.write8(0b01000000); // enable Status LED / disable PoV LED
    MCP.setPullup8(0b00111111); // enable pullups for inputs
    
    return true;
  }

  void setExternalGPIO(uint8_t gpio, bool state) {
    MCP.write1(gpio, state);
  }

  void readExternalGPIO() {
    externalInputs.value = MCP.read8();

    externalInputs.buttonUp = !(externalInputs.value & (1 << 0)); // inverted logic
    externalInputs.buttonRight = !(externalInputs.value & (1 << 1)); // inverted logic
    externalInputs.buttonDown = !(externalInputs.value & (1 << 2)); // inverted logic
    externalInputs.buttonLeft = !(externalInputs.value & (1 << 3)); // inverted logic
    externalInputs.buttonOK = !(externalInputs.value & (1 << 4)); // inverted logic
    externalInputs.cardPresent = !(externalInputs.value & (1 << 5)); // inverted logic

    checkForKeypress();

    // copy the value for next detection
    externalInputsOld.buttonUp = externalInputs.buttonUp;
    externalInputsOld.buttonRight = externalInputs.buttonRight;
    externalInputsOld.buttonDown = externalInputs.buttonDown;
    externalInputsOld.buttonLeft = externalInputs.buttonLeft;
    externalInputsOld.buttonOK = externalInputs.buttonOK;
    externalInputsOld.cardPresent = externalInputs.cardPresent;
    externalInputsOld.value = externalInputs.value;
  }

  void enablePotiLED() {
    setExternalGPIO(6, false); // inverted logic!
  }

  void disablePotiLED() {
    setExternalGPIO(6, true); // inverted logic!
  }

  void enableStatusLED() {
    setExternalGPIO(7, false); // inverted logic!
  }

  void disableStatusLED() {
    setExternalGPIO(7, true); // inverted logic!
  }

  void checkForKeypress() {
    if ((externalInputs.value & 0b00111111) != (externalInputsOld.value & 0b00111111)) {
      // change detected. Now check if pressed or released and call relevant function

      if (externalInputs.buttonUp != externalInputsOld.buttonUp) {
        // buttonUp changed
        if (externalInputs.buttonUp) {
          // pressed
          menuNavigation(menuNavigationUp);
          prepareButtonForRepeat(menuNavigationUp);
        }else{
          // released
          buttonPressed = 0;
        }
      }

      if (externalInputs.buttonDown != externalInputsOld.buttonDown) {
        // buttonDown changed
        if (externalInputs.buttonDown) {
          // pressed
          menuNavigation(menuNavigationDown);
          prepareButtonForRepeat(menuNavigationDown);
        }else{
          // released
          buttonPressed = 0;
        }
      }

      if (externalInputs.buttonLeft != externalInputsOld.buttonLeft) {
        // buttonLeft changed
        if (externalInputs.buttonLeft) {
          // pressed
          menuNavigation(menuNavigationLeft);
          prepareButtonForRepeat(menuNavigationLeft);
        }else{
          // released
          buttonPressed = 0;
        }
      }

      if (externalInputs.buttonRight != externalInputsOld.buttonRight) {
        // buttonRight changed
        if (externalInputs.buttonRight) {
          // pressed
          menuNavigation(menuNavigationRight);
          prepareButtonForRepeat(menuNavigationRight);
        }else{
          // released
          buttonPressed = 0;
        }
      }

      if (externalInputs.buttonOK != externalInputsOld.buttonOK) {
        // buttonOK changed
        if (externalInputs.buttonOK) {
          // pressed
          menuNavigation(menuNavigationOK);
          prepareButtonForRepeat(menuNavigationOK);
        }else{
          // released
          buttonPressed = 0;
        }
      }

      if (externalInputs.cardPresent != externalInputsOld.cardPresent) {
        // cardPresent changed
        if (externalInputs.cardPresent) {
          // card inserted
		  
		  // inform MainCtrl that we have to reinitialize the SD-card
		  Serial1.println(F("system:card:init"));
		  
		  // show card logo
          displayShowLogo(0);
		  
		  // wait a bit to show logo
          delay(500);

          // request TOC from MainCtrl
          Serial1.println(F("player:samdtoc?"));
        }else{
          // card released
          displayShowLogo(0); // show card logo
          delay(500);
        }
      }
    }
  }

  void prepareButtonForRepeat(uint8_t button) {
    buttonPressed = button;
    buttonPressedCounter = 5;
  }

  void checkButtonRepeat() {
    if (buttonPressedCounter > 0) {
      buttonPressedCounter -= 1;
    }
    if ((buttonPressedCounter == 0) && (buttonPressed > 0)) {
      // repeat button-action
      menuNavigation(buttonPressed);
    }
  }
#endif