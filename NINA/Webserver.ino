void initWebserver() {
  // register handles for dynamic content
  webserver.on("/cmd.php", httpHandleCommand);
  webserver.on("/json", httpHandleJson); // send update-data in JSON format
  webserver.on("/toc", httpHandleToc); // send TOC in JSON format

  // upload files from SD-card
  webserver.onNotFound(httpHandleNotFound);

  // Start the HTTP-Server
  webserver.begin();
}

void httpHandleCommand() {
  // lets search for some values to set
  // /cmd.php?p_max=1000&q_max=1000&u_ref=120
  for (uint8_t i = 0; i < webserver.args(); i++) {
    if (webserver.argName(i) == "cmd") {
      #if USE_SDPLAYER == 1
        if (webserver.arg(i) == "play") {
          //Serial.println("player:play");
          if (currentAudioFile.indexOf("http") > -1){
            // play stream
            audio.connecttohost(currentAudioFile.c_str());
          }else{
            // play file from SD-card
            audio.connecttoFS(SD, currentAudioFile.c_str()); // play audio-file
          }
        } else if (webserver.arg(i) == "loop") {
          //Serial.println("player:loop");
          audio.setFileLoop(true);
        } else if (webserver.arg(i) == "pause") {
          //Serial.println("player:pause");
          audio.pauseResume();
        } else if (webserver.arg(i) == "stop") {
          //Serial.println("player:stop");
          audio.setFileLoop(false);
          audio.stopSong();
        } else if (webserver.arg(i) == "restart") {
          ESP.restart(); // restart ESP
        }
      #endif
    }else{
      // pass-through all other commands to main-controller directly
      // e.g. "mixer:balance:ch2"
      executeCommand(webserver.argName(i) + "@" + webserver.arg(i));
    }
  }

  webserver.send(204, "text/plain", "No Content"); // this page doesn't send back content --> 204 = dont move away from current page
}

void httpHandleJson() {
  // Output: send data to browser as JSON

  String message;

  // General information
  message = F("{\"system_uptime\":");
  message += millis() / 1000;
  message += (F(",\"version_mainctrl\":\"f/bape MainCtrl "));
  message += versionstring;
  message += " | ";
  message += compile_date;
  message += (F("\",\"version_fpga\":\"v"));
  message += FPGA_Version;
  message += (F("\",\"version_usbctrl\":\""));
  message += USBCtrlIDN;

  // Main-Volume-Information
  message += (F("\",\"volume_main\":"));
  message += audiomixer.mainVolume;
  message += (F(",\"balance_main\":"));
  message += audiomixer.mainBalance;
  message += (F(",\"volume_sub\":"));
  message += audiomixer.mainVolumeSub;
  message += (F(",\"power_main\":"));
  message += ((48*48) * pow(10, audiomixer.compressors[0].threshold/20)) / 6; // calculate (V^2/threshold)/(6 Ohm)
  message += (F(",\"power_sub\":"));
  message += ((48*48) * pow(10, audiomixer.compressors[1].threshold/20)) / 6; // calculate (V^2/threshold)/(6 Ohm)

  // Player-Data
  message += (F(",\"player_isplaying\":"));
  #if USE_SDPLAYER == 1
    if (audio.isRunning()) {
      // currently playing
      currentAudioPosition = audio.getAudioCurrentTime();
      message += "1";
    }else{
      // not playing
      message += "0";
    }
    uint32_t audioDuration = audio.getAudioFileDuration();
  #else
    message += "0";
    uint32_t audioDuration = 0;
  #endif
  message += (F(",\"player_title\":\""));
  message += currentAudioFile;
  message += (F("\",\"player_position\":\""));
  message += secondsToHMS(currentAudioPosition);
  message += (F("\",\"player_duration\":\""));
  message += secondsToHMS(audioDuration);
  message += (F("\",\"player_progress\":\""));
  if (audioDuration > 0) {
    message += String(((float)currentAudioPosition/(float)audioDuration)*100.0f, 1);
    message += "%";
  }else{
    message += "STOP";
  }
  message += "\"";

  // Audiovolumes
  for (int i=0; i<MAX_AUDIO_CHANNELS; i++) {
    message += ",\"volume_ch" + String(i+1) + "\":";
    message += audiomixer.chVolume[i];
    message += ",\"balance_ch" + String(i+1) + "\":";
    message += audiomixer.chBalance[i];
  }

  // VU-meter-values
  message += ",\"vumeter_left\":";
  message += vu_meter_value[0];
  message += ",\"vumeter_right\":";
  message += vu_meter_value[1];
  message += ",\"vumeter_sub\":";
  message += vu_meter_value[2];

  // channel-clipping
  message += ",\"clip_left\":";
  message += ((fpgaRxBuffer[4] & 0b00000001) > 0);
  message += ",\"clip_right\":";
  message += ((fpgaRxBuffer[4] & 0b00000010) > 0);
  message += ",\"clip_sub\":";
  message += ((fpgaRxBuffer[4] & 0b00000100) > 0);

  // Equalizers
  for (int i=0; i<MAX_EQUALIZERS; i++) {
    message += ",\"peq" + String(i+1) + "_type\":";
    message += audiomixer.peq[i].type;
    message += ",\"peq" + String(i+1) + "_fc\":";
    message += audiomixer.peq[i].fc;
    message += ",\"peq" + String(i+1) + "_q\":";
    message += audiomixer.peq[i].Q;
    message += ",\"peq" + String(i+1) + "_gain\":";
    message += audiomixer.peq[i].gain;
  }

  // Crossover-Filter
  message += (F(",\"lp_fc\":"));
  message += audiomixer.LR24_LP_Sub.fc;
  message += (F(",\"hp_fc\":"));
  message += audiomixer.LR24_HP_LR.fc;

  // ADC-Gains
  for (int i=0; i<MAX_ADCS; i++) {
    message += ",\"adc" + String(i+1) + "_gain\":";
    message += audiomixer.adcGain[i]; // in dB
  }

  // Noise-Gates
  for (int i=0; i<MAX_NOISEGATES; i++) {
    message += ",\"gate" + String(i+1) + "_thresh\":";
    message += audiomixer.gates[i].threshold; // in dB
    message += ",\"gate" + String(i+1) + "_range\":";
    message += audiomixer.gates[i].range; // in dB
    message += ",\"gate" + String(i+1) + "_att\":";
    message += audiomixer.gates[i].attackTime_ms;
    message += ",\"gate" + String(i+1) + "_hold\":";
    message += audiomixer.gates[i].holdTime_ms;
    message += ",\"gate" + String(i+1) + "_rel\":";
    message += audiomixer.gates[i].releaseTime_ms;
    message += ",\"gate" + String(i+1) + "_closed\":";
    message += ((fpgaRxBuffer[2] & (1 << i)) > 0);
  }

  // Dynamic-Compressors / Limiters
  for (int i=0; i<MAX_COMPRESSORS; i++) {
    message += ",\"comp" + String(i+1) + "_thresh\":";
    message += audiomixer.compressors[i].threshold; // in dB
    message += ",\"comp" + String(i+1) + "_ratio\":";
    message += audiomixer.compressors[i].ratio; // oo:1, 1:1, 2:1, 4:1, 8:1, 16:1, ...
    message += ",\"comp" + String(i+1) + "_makeup\":";
    message += audiomixer.compressors[i].makeup; // in dB
    message += ",\"comp" + String(i+1) + "_att\":";
    message += audiomixer.compressors[i].attackTime_ms;
    message += ",\"comp" + String(i+1) + "_hold\":";
    message += audiomixer.compressors[i].holdTime_ms;
    message += ",\"comp" + String(i+1) + "_rel\":";
    message += audiomixer.compressors[i].releaseTime_ms;
    message += ",\"comp" + String(i+1) + "_active\":";
    message += ((fpgaRxBuffer[3] & (1 << i)) > 0);
  }

  message += (F("}")); // End of JSON

  webserver.send(200, "application/json", message);
}

void httpHandleToc() {
  // Output: send TOC to browser as JSON

  String message;

  message = (F("{")); // End of JSON
  message += SD_getTOC(0); // list all Wave and MP3-files with file-size
  message += (F("}")); // End of JSON

  webserver.send(200, "application/json", message);
}

void httpHandleNotFound() {
  if (loadFromSdCard(webserver.uri())) {
    // we could serve the desired file from SD-card
    return;
  }

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webserver.uri();
  message += "\nMethod: ";
  message += (webserver.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webserver.args();
  message += "\n";
  for (uint8_t i = 0; i < webserver.args(); i++) {
    message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "\n";
  }
  webserver.send(404, "text/plain", message);
}

bool loadFromSdCard(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) {
    path += "index.htm";
  }

  if (path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if (path.endsWith(".htm")) {
    dataType = "text/html";
  } else if (path.endsWith(".css")) {
    dataType = "text/css";
  } else if (path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if (path.endsWith(".png")) {
    dataType = "image/png";
  } else if (path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if (path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if (path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if (path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if (path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if (path.endsWith(".zip")) {
    dataType = "application/zip";
  }

  File dataFile = SD.open(path.c_str());
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile) {
    return false;
  }

  if (webserver.hasArg("download")) {
    dataType = "application/octet-stream";
  }

  if (webserver.streamFile(dataFile, dataType) != dataFile.size()) {
    // Sent less data than expected!
  }

  dataFile.close();
  return true;
}