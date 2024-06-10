#if USE_DISPLAY == 1
  void menuNavigation(uint8_t option) {
    if ((option == menuNavigationUp) || (option == menuNavigationDown)) {
      uint8_t maxOptions = 0;
      switch (currentDisplayMenu) {
        case 0: // menuMain
          maxOptions = 3; // menu-selection, volumeMain, volumeSub
          break;
        case 1: // menuPlaylist
          maxOptions = 6; // menu-selection, 5 titles
          break;
        case 2: // menuVolume
          maxOptions = 6; // menu-selection + 5 options
          break;
        case 3: // menuSetup
          maxOptions = 4; // menu-selection + 3 option
          break;
        case 4: // menuInfo
          maxOptions = 1; // only menu-selection
          break;
        case 5: // menuDebug
          maxOptions = 1; // only menu-selection
          break;
      }

      // select menu-options
      if (option == menuNavigationUp) {
        if (currentDisplayOption == 0) {
          currentDisplayOption = maxOptions - 1;
        }else{
          currentDisplayOption -= 1;
        }
      }  
      if (option == menuNavigationDown) {
        if (currentDisplayOption == maxOptions - 1) {
          currentDisplayOption = 0;
        }else{
          currentDisplayOption += 1;
        }
      }

      // an option has changed -> reset PotiOnValue
      resetPotiOnValue();
    }


    // Menu-Navigation (Left/Right)
    if ((option == menuNavigationLeft) || (option == menuNavigationRight)) {
      if (currentDisplayOption == 0) {
        if (option == menuNavigationLeft) {
          // swipe through menus counter-clockwise
          if (currentDisplayMenu == 0) {
            currentDisplayMenu = NUMBER_OF_MENUS - 1;
          }else{
            currentDisplayMenu -= 1;
          }
        }else if (option == menuNavigationRight){
          // swipe through menus clockwise
          if (currentDisplayMenu == NUMBER_OF_MENUS - 1) {
            currentDisplayMenu = 0;
          }else{
            currentDisplayMenu += 1;
          }
        }

        if (currentDisplayMenu == 1) {
          // recall the TOC from NINA
          Serial2.println(F("player:samdtoc?"));
        }

        // the menu has changed -> reset PotiOnValue
        resetPotiOnValue();
      }else{
        // other options -> do incremental change on the value in 5% steps
        if (option == menuNavigationLeft) {
          menuValueChange(-0.05, false, false); // incremental change
        }else if (option == menuNavigationRight){
          menuValueChange(0.05, false, false); // incremental change
        }
      }
    }

    if (option == menuNavigationOK) {
      if ((currentDisplayMenu == 1) && (currentDisplayOption > 0)) {
        // playlist -> select new title
        Serial2.println("player:file@" + split(TOC, ',', currentDisplayOption - 1 + TOC_Offset));
      }else{
        // all other menus -> play/pause
        Serial2.println(F("player:pause")); // toggle play/pause
      }
    }
  }

  void menuValueChange(float value_pu, bool absolute, bool CCW) {
    float potiValue;

    switch (currentDisplayMenu) {
      case 0: // menuMain
        switch (currentDisplayOption) {
          case 1: // volumeMain
            if (absolute) {
              // check if the Poti is on the value
              potiValue = -48.0f + (54.0f * value_pu); // 0.0 = -48dBfs / 1.0 = +6dBfs
              if (potiOnValue || ((potiValue > (playerinfo.volumeMain - 2)) && (potiValue < (playerinfo.volumeMain + 2)))) {
                // Poti is within +/-2dBfs area -> take snap the Poti onto this option
                setPotiOnValue();
                playerinfo.volumeMain = saturate_f(potiValue, -48, 6);
                Serial2.println("mixer:volume:main@" + String(playerinfo.volumeMain, 1));
              }
            }else{
              // incremental volume change
              playerinfo.volumeMain = saturate_f(playerinfo.volumeMain + (value_pu * 40.0f), -48, 6);
              Serial2.println("mixer:volume:main@" + String(playerinfo.volumeMain, 1)); // increase/decrese in 2dB steps
            }
            break;
          case 2: // volumeSub
            if (absolute) {
              // check if the Poti is on the value
              potiValue = -48.0f + (54.0f * value_pu); // 0.0 = -48dBfs / 1.0 = +6dBfs
              if (potiOnValue || ((potiValue > (playerinfo.volumeSub - 2)) && (potiValue < (playerinfo.volumeSub + 2)))) {
                // Poti is within +/-2dBfs area -> take snap the Poti onto this option
                setPotiOnValue();
                playerinfo.volumeSub = saturate_f(potiValue, -48, 6);
                Serial2.println("mixer:volume:sub@" + String(playerinfo.volumeSub, 1));
              }
            }else{
              // incremental volume change
              playerinfo.volumeSub = saturate_f(playerinfo.volumeSub + (value_pu * 40.0f), -48, 6);
              Serial2.println("mixer:volume:sub@" + String(playerinfo.volumeSub, 1)); // increase/decrese in 2dB steps
            }
            break;
          case 3: // free
            break;
          case 4: // free
            break;
          case 5: // free
            break;
        }
        break;
      case 1: // menuPlaylist
        if (absolute) {
          // increment by one on turning the Poti
          if (CCW) {
            TOC_Offset -= 1;
          } else {
            TOC_Offset += 1;
          }
        }else{
          if (value_pu < 0) {
            TOC_Offset -= 5;
          }else{
            TOC_Offset += 5;
          }
        }
        break;
      case 2: // menuVolume
        switch (currentDisplayOption) {
          case 1: // volumeMain
            if (absolute) {
              // check if the Poti is on the value
              potiValue = -48.0f + (54.0f * value_pu); // 0.0 = -48dBfs / 1.0 = +6dBfs
              if (potiOnValue || ((potiValue > (playerinfo.volumeMain - 2)) && (potiValue < (playerinfo.volumeMain + 2)))) {
                // Poti is within +/-2dBfs area -> take snap the Poti onto this option
                setPotiOnValue();
                playerinfo.volumeMain = saturate_f(potiValue, -48, 6);
                Serial2.println("mixer:volume:main@" + String(playerinfo.volumeMain, 1));
              }
            }else{
              // incremental volume change
              playerinfo.volumeMain = saturate_f(playerinfo.volumeMain + (value_pu * 40.0f), -48, 6);
              Serial2.println("mixer:volume:main@" + String(playerinfo.volumeMain, 1)); // increase/decrese in 2dB steps
            }
            break;
          case 2: // balanceMain
            if (absolute) {
              // check if the Poti is on the value
              potiValue = (value_pu * 100.0f);
              if (potiOnValue || ((potiValue > (playerinfo.balanceMain - 5)) && (potiValue < (playerinfo.balanceMain + 5)))) {
                // Poti is within +/-5% area -> take snap the Poti onto this option
                setPotiOnValue();
                playerinfo.balanceMain = saturate_f(potiValue, 0, 100);
                Serial2.println("mixer:balance:main@" + String(playerinfo.balanceMain));
              }
            }else{
              // incremental volume change
              playerinfo.balanceMain = saturate_f(playerinfo.balanceMain + (value_pu * 100.0f), 0, 100);
              Serial2.println("mixer:balance:main@" + String(playerinfo.balanceMain)); // increase/decrese in 5% steps
            }
            break;
          case 3: // volumeSub
            if (absolute) {
              // check if the Poti is on the value
              potiValue = -48.0f + (54.0f * value_pu); // 0.0 = -48dBfs / 1.0 = +6dBfs
              if (potiOnValue || ((potiValue > (playerinfo.volumeSub - 2)) && (potiValue < (playerinfo.volumeSub + 2)))) {
                // Poti is within +/-2dBfs area -> take snap the Poti onto this option
                setPotiOnValue();
                playerinfo.volumeSub = saturate_f(potiValue, -48, 6);
                Serial2.println("mixer:volume:sub@" + String(playerinfo.volumeSub, 1));
              }
            }else{
              // incremental volume change
              playerinfo.volumeSub = saturate_f(playerinfo.volumeSub + (value_pu * 40.0f), -48, 6);
              Serial2.println("mixer:volume:sub@" + String(playerinfo.volumeSub, 1)); // increase/decrese in 2dB steps
            }
            break;
          case 4: // Analog Input
            if (absolute) {
              // check if the Poti is on the value
              potiValue = -48.0f + (54.0f * value_pu); // 0.0 = -48dBfs / 1.0 = +6dBfs
              potiValue = saturate_f(potiValue, -48, 6);
              if (potiOnValue || ((potiValue > (playerinfo.volumeAnalog - 2)) && (potiValue < (playerinfo.volumeAnalog + 2)))) {
                // Poti is within +/-2dBfs area -> take snap the Poti onto this option
                setPotiOnValue();
                playerinfo.volumeAnalog = saturate_f(potiValue, -48, 6);
                //Serial2.println("mixer:volume:analogin@" + String(playerinfo.volumeAnalog, 1));
              }
            }else{
              // incremental volume change
              playerinfo.volumeAnalog = saturate_f(playerinfo.volumeAnalog + (value_pu * 40.0f), -48, 6);
              //Serial2.println("mixer:volume:analogin@" + String(playerinfo.volumeAnalog, 1)); // increase/decrese in 2dB steps
            }
            break;
          case 5: // SD Card Input
            if (absolute) {
              // check if the Poti is on the value
              potiValue = -48.0f + (54.0f * value_pu); // 0.0 = -48dBfs / 1.0 = +6dBfs
              if (potiOnValue || ((potiValue > (playerinfo.volumeCard - 2)) && (potiValue < (playerinfo.volumeCard + 2)))) {
                // Poti is within +/-2dBfs area -> take snap the Poti onto this option
                setPotiOnValue();
                playerinfo.volumeCard = saturate_f(potiValue, -48, 6);
                Serial2.println("player:volume@" + String(playerinfo.volumeCard, 1));
              }
            }else{
              // incremental volume change
              playerinfo.volumeCard = saturate_f(playerinfo.volumeCard + (value_pu * 40.0f), -48, 6);
              Serial2.println("player:volume@" + String(playerinfo.volumeCard, 1)); // increase/decrese in 2dB steps
            }
            break;
        }
        break;
      case 3: // menuSetup
        switch (currentDisplayOption) {
          case 1: // option 1
            if (!absolute) {
              if (playerinfo.WiFiEnabled) {
                // disable WiFi
                playerinfo.WiFiEnabled = false;
                Serial2.println("wifi:enabled@0");
              }else{
                // enable WiFi
                playerinfo.WiFiEnabled = true;
                Serial2.println("wifi:enabled@1");
              }
            }
            break;
          case 2: // option 2
            if (!absolute) {
              if (playerinfo.BluetoothEnabled) {
                // disable bluetooth
                playerinfo.BluetoothEnabled = false;
                Serial2.println("bt:enabled@0");
              }else{
                // enable bluetooth
                playerinfo.BluetoothEnabled = true;
                Serial2.println("bt:enabled@1");
              }
            }
            break;
          case 3: // option 3
          if (CCW) {
            if (playerinfo.samplerate == 44100) {
              playerinfo.samplerate = 96000;
              Serial1.println("mixer:samplerate@96000");
            }else if (playerinfo.samplerate == 48000) {
              playerinfo.samplerate = 44100;
              Serial1.println("mixer:samplerate@44100");
            }else if (playerinfo.samplerate == 96000) {
              playerinfo.samplerate = 48000;
              Serial1.println("mixer:samplerate@48000");
            }else{
              playerinfo.samplerate = 48000;
              Serial1.println("mixer:samplerate@48000");
            }
          }else{
            if (playerinfo.samplerate == 44100) {
              playerinfo.samplerate = 48000;
              Serial1.println("mixer:samplerate@48000");
            }else if (playerinfo.samplerate == 48000) {
              playerinfo.samplerate = 96000;
              Serial1.println("mixer:samplerate@96000");
            }else if (playerinfo.samplerate == 96000) {
              playerinfo.samplerate = 44100;
              Serial1.println("mixer:samplerate@44100");
            }else{
              playerinfo.samplerate = 48000;
              Serial1.println("mixer:samplerate@48000");
            }
          }
            break;
          case 4: // option 4
            break;
          case 5: // option 5
            break;
        }
        break;
      case 4: // menuInfo
        // no additional options
        break;
      case 5: // menuDebug
        // no additional options
        break;
    }
  }
#endif