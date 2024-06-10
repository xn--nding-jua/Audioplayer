String configReadUntil(char endChar, uint16_t maxChars) {
  String received = "";
  char receivedChar;

  uint16_t charCounter = 0;
  while (configFile.available()) {
    // read single character
    receivedChar = configFile.read();

    // if character is NewLine, return string, otherwise append to string
    if (receivedChar == endChar) {
      // we received the end of the line
      received.trim();  // remove \r or additional spaces
      return received;
    } else {
      // receiving regular character. Append to new value
      received += receivedChar;
    }

    charCounter += 1;
    if (charCounter > maxChars) {
      // more than maxChars characters without new-line-char are not expected/supported -> break while-loop
      return received;
    }
  }

  received.trim();
  return received;  // return received string. Maybe there is no NewLine at the last line
}

// function for reading config-file
bool configRead(String configFileName, uint8_t maxLines) {
  configFile = SD.open(configFileName);

  if (configFile) {
    // read settings from config-file and

    uint8_t lineCounter = 0;
    while (configFile.available() > 4) {
      // at least 4 characters to read -> read line from opened file and process the file through the command-processor

      executeCommand(configReadUntil('\n', 40)); // we are not supporting more than 40 chars per line
      lineCounter += 1;
      if (lineCounter > maxLines) {
        // configuration with more than maxLines is not expected/supported
        break;
      }
    }

    // done
    configFile.close();
    return true;
  } else {
    return false;
  }
}

// function for writing wifi-information to desired config-file
bool configWrite(String configFileName) {
  // delete old config-file
  SD.remove(configFileName);

  // create new one
  configFile = SD.open(configFileName, FILE_WRITE); // FILE_WRITE will always APPEND without seek()-option. Use O_RDWR to use seek(). More info here: https://forum.arduino.cc/t/sdfat-do-not-overwrite/373174/4
  String tmp_str;

  if (configFile) {
    // write settings to individual lines
    //configFile.println("player:file@" + currentAudioFile); // uncomment this line to load the last audiofile on next start
    
    // audio-settings
    configFile.println(String("mixer:volume:main@" + String(audiomixer.mainVolume, 2)).c_str());
    configFile.println("mixer:balance:main@" + String(audiomixer.mainBalance));
    configFile.println("mixer:volume:sub@" + String(audiomixer.mainVolumeSub, 2));
    for (int i=1; i<=MAX_AUDIO_CHANNELS; i++) {
      configFile.println("mixer:volume:ch" + String(i) + "@" + String(audiomixer.chVolume[i-1], 2));
      configFile.println("mixer:balance:ch" + String(i) + "@" + String(audiomixer.chBalance[i-1]));
    }
    configFile.println("mixer:sync@" + String(audiomixer.audioSyncSource));
    configFile.println("mixer:samplerate@" + String(audiomixer.sampleRate));

    for (int i=1; i<=MAX_EQUALIZERS; i++) {
      configFile.println("eq:peq" + String(i) + "@" + String(audiomixer.peq[i-1].type) + "," + String(audiomixer.peq[i-1].fc, 2) + "," + String(audiomixer.peq[i-1].Q, 2) + "," + String(audiomixer.peq[i-1].gain, 2));
    }

    configFile.println("mixer:eq:lp@" + String(audiomixer.LR24_LP_Sub.fc, 2));
    configFile.println("mixer:eq:hp@" + String(audiomixer.LR24_HP_LR.fc, 2));

    for (int i=1; i<=MAX_ADCS; i++) {
      configFile.println("mixer:adc:gain" + String(i) + "@" + String(audiomixer.adcGain[i-1]));
    }

    for (int i=1; i<=MAX_NOISEGATES; i++) {
      configFile.println("mixer:gate" + String(i) + "@" + String(audiomixer.gates[i-1].threshold, 2) + "," + String(audiomixer.gates[i-1].range, 2) + "," + String(audiomixer.gates[i-1].attackTime_ms, 2) + "," + String(audiomixer.gates[i-1].holdTime_ms, 2) + "," + String(audiomixer.gates[i-1].releaseTime_ms, 2));
    }

    for (int i=1; i<=MAX_COMPRESSORS; i++) {
      configFile.println("mixer:comp" + String(i) + "@" + String(audiomixer.compressors[i-1].threshold, 2) + "," + String(audiomixer.compressors[i-1].ratio, 2) + "," + String(audiomixer.compressors[i-1].makeup, 2) + "," + String(audiomixer.compressors[i-1].attackTime_ms, 2) + "," + String(audiomixer.compressors[i-1].holdTime_ms, 2) + "," + String(audiomixer.compressors[i-1].releaseTime_ms, 2));
    }

    // done
    configFile.close();
    return true;
  } else {
    return false;
  }
}

// function for writing wifi-information to desired config-file
bool configWiFiWrite(String configFileName) {
  // delete old config-file
  SD.remove(configFileName);

  // create new one
  configFile = SD.open(configFileName, FILE_WRITE); // FILE_WRITE will always APPEND without seek()-option. Use O_RDWR to use seek(). More info here: https://forum.arduino.cc/t/sdfat-do-not-overwrite/373174/4
  String tmp_str;

  if (configFile) {
    // write settings to individual lines
    // WiFi-settings
    configFile.println("wifi:ssid@" + ssid);
    configFile.println("wifi:password@" + password);
    configFile.println("wifi:ip@" + ipAddress2String(ip));
    configFile.println("wifi:gateway@" + ipAddress2String(gateway));
    configFile.println("wifi:subnet@" + ipAddress2String(subnet));
    configFile.println("wifi:mode@" + WiFiAP);

    // done
    configFile.close();
    return true;
  } else {
    return false;
  }
}