void initWifi() {
  // set friendly hostname
  WiFi.setHostname(hostname.c_str());

  if (WiFiAP) {
    // enable WiFi in accesspoint-mode
    WiFi.mode(WIFI_AP);

    // configure WiFi-accesspoint to desired IP-configuration and SSID
    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP(ssid.c_str(), password.c_str()); // WiFi.softAP(ssid, password, APCHAN)
  } else {
    // enable WiFi as client (station)
    WiFi.mode(WIFI_STA);

    // if desired, use static IP
    #if USE_STATICIP == 1
      WiFi.config(ip, gateway, subnet, primaryDNS, secondaryDNS);
    #endif
    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }
}

void handleUartCommunication() {
  String command;

  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.trim();

    if (command.indexOf("OK") > -1) {
      // received "OK" from SAMD -> last command is OK
    }else if (command.indexOf("ERROR") > -1) {
      // received "ERROR" from SAMD -> last command had an error
    }else if (command.indexOf("SAMD: UNKNOWN_CMD") > -1) {
      // SAMD received some garbage or communication-problem -> ignore it for the moment
    }else if (command.indexOf("fpga:") == 0) {
      if (command.indexOf("ver?") > -1){
        // return version of fpga
        Serial.println("v" + FPGA_Version);
      }else if (command.indexOf("*IDN?") > -1){
        // return info-string of fpga
        Serial.println("f/bape FPGA v" + String(FPGA_Version));
      }else{
        // this is a command for the FPGA
        Serial1.print(command);
        Serial.println("OK"); // FPGA will not return values instead of the version up to now
      }
    }else{
      // this is a command for our command-processor
      Serial.println(executeCommand(command));
    }
  }
}

// FPGA-Receiver
void handleFPGACommunication() {
  if (Serial1.available() > 0) {
    if (debugRedirectFpgaSerialToUSB) {
      // redirect all incoming bytes to SAMD-processor
      Serial.write(Serial1.read());
    }else{
      // FPGA sends 20 messages per second

      // receiving data from FPGA AbbbbbbbCCE
      Serial1.readBytes(fpgaRxBuffer, sizeof(fpgaRxBuffer));

      // check data
      uint16_t payloadSum = fpgaRxBuffer[1] + fpgaRxBuffer[2] + fpgaRxBuffer[3] + fpgaRxBuffer[4] + fpgaRxBuffer[5] + fpgaRxBuffer[6] + fpgaRxBuffer[7];
      uint16_t ErrorCheckData = (fpgaRxBuffer[sizeof(fpgaRxBuffer)-3] << 8) + fpgaRxBuffer[sizeof(fpgaRxBuffer)-2];
      if ((fpgaRxBuffer[0] == 65) && (fpgaRxBuffer[sizeof(fpgaRxBuffer)-1] == 69) && (payloadSum == ErrorCheckData)) {
        // we received valid data
        // fpgaRxBuffer[1] contains version of FPGA * 100
        if (FPGA_Version.length() == 1) {
          FPGA_Version = String(fpgaRxBuffer[1]/100.0f, 2);
          #if USE_DISPLAY == 1
            // send version information to SAMD21
            Serial.println("samd:version:fpga@" + FPGA_Version); // send FPGA version to SAMD21
          #endif
        }else{
          FPGA_Version = String(fpgaRxBuffer[1]/100.0f, 2);
        }

		    // fpgaRxBuffer will be sent to Webserver-Clients within Webserver.ino
        // fpgaRxBuffer[2] contains information about the noisegate(s)

        // fpgaRxBuffer[3] contains information about the compressors
        if ((fpgaRxBuffer[3] & 0b00000001) || (fpgaRxBuffer[3] & 0b00000010)) {
          // audio is being compressing
        }

        // fpgaRxBuffer[4] contains information about clipping

        // fpgaRxBuffer[5] ... fpgaRxBuffer[7] contain VU-meter information for left, right and subwoofer
        for (int i=0; i<3; i++) {
          // we are taking only larger values than the current value
          uint8_t new_value = log2(fpgaRxBuffer[i+5] + 1) * (255/8);

          if (new_value > vu_meter_value[i]) {
            // convert bit-values to a linear scale
            vu_meter_value[i] = new_value; // direct update
            //vu_meter_value[i] = vu_meter_value[i] + (new_value - vu_meter_value[i])*0.2f; // slow update -> smoother transition
          }else{
            // decay the vu-meter with the given slope
            vu_meter_value[i] = vu_meter_value[i] * vumeter_decay;
          }
        }
      }else{
        // error in received data
        // at the moment we do not handle communication-errors
      }
    }
  }
}

#if USE_TCPSERVER == 1
  void handleTCPCommunication() {
    // listen for incoming clients
    if (cmdclient) {
      // client is connected

      // check if we have some data to receive
      if (cmdclient.available() > 0) {
        String command = cmdclient.readStringUntil('\n');
        command.trim();

        // execute command with our command-processor and return the answer to client
        cmdclient.println(executeCommand(command));
      }
    }else{
      // no current client connected. Try to catch a new connection
      cmdclient = cmdserver.available();
    }
  }
#endif

#if USE_MQTT == 1
  void initMQTT() {
    // configure MQTT-client/server-connection
    mqttclient.setServer(mqtt_server, mqtt_serverport);
    mqttclient.setCallback(mqttCallback);

    // connect to MQTT-broker
    if (mqttclient.connect(mqtt_id)) {
      // we have a working connection

      // subscribe to the desired topics
      mqttclient.subscribe("fbape/wifi/ssid");
      mqttclient.subscribe("fbape/wifi/password");
      mqttclient.subscribe("fbape/wifi/ip");
      mqttclient.subscribe("fbape/wifi/gateway");
      mqttclient.subscribe("fbape/wifi/subnet");
      mqttclient.subscribe("fbape/wifi/mode");
      mqttclient.subscribe("fbape/player/play");
      mqttclient.subscribe("fbape/player/loop");
      mqttclient.subscribe("fbape/player/pause");
      mqttclient.subscribe("fbape/player/stop");
      mqttclient.subscribe("fbape/player/position");
      mqttclient.subscribe("fbape/player/volume");
      mqttclient.subscribe("fbape/mixer/volume/main");
      mqttclient.subscribe("fbape/mixer/balance/main");
      mqttclient.subscribe("fbape/mixer/volume/sub");
      mqttclient.subscribe("fbape/mixer/volume/ch1");
      mqttclient.subscribe("fbape/mixer/volume/ch2");
      mqttclient.subscribe("fbape/mixer/volume/ch3");
      mqttclient.subscribe("fbape/mixer/volume/ch4");
      mqttclient.subscribe("fbape/mixer/volume/ch5");
      mqttclient.subscribe("fbape/mixer/volume/ch6");
      mqttclient.subscribe("fbape/mixer/volume/ch7");
      mqttclient.subscribe("fbape/mixer/volume/ch8");
      mqttclient.subscribe("fbape/mixer/volume/ch9");
      mqttclient.subscribe("fbape/mixer/volume/ch10");
      mqttclient.subscribe("fbape/mixer/volume/ch11");
      mqttclient.subscribe("fbape/mixer/volume/ch12");
      mqttclient.subscribe("fbape/mixer/volume/ch13");
      mqttclient.subscribe("fbape/mixer/volume/ch14");
      mqttclient.subscribe("fbape/mixer/volume/ch15");
      mqttclient.subscribe("fbape/mixer/volume/ch16");
      mqttclient.subscribe("fbape/mixer/volume/ch17");
      mqttclient.subscribe("fbape/mixer/volume/ch18");
      mqttclient.subscribe("fbape/mixer/volume/ch19");
      mqttclient.subscribe("fbape/mixer/volume/ch20");
      mqttclient.subscribe("fbape/mixer/volume/ch21");
      mqttclient.subscribe("fbape/mixer/volume/ch22");
      mqttclient.subscribe("fbape/mixer/balance/ch1");
      mqttclient.subscribe("fbape/mixer/balance/ch2");
      mqttclient.subscribe("fbape/mixer/balance/ch3");
      mqttclient.subscribe("fbape/mixer/balance/ch4");
      mqttclient.subscribe("fbape/mixer/balance/ch5");
      mqttclient.subscribe("fbape/mixer/balance/ch6");
      mqttclient.subscribe("fbape/mixer/balance/ch7");
      mqttclient.subscribe("fbape/mixer/balance/ch8");
      mqttclient.subscribe("fbape/mixer/balance/ch9");
      mqttclient.subscribe("fbape/mixer/balance/ch10");
      mqttclient.subscribe("fbape/mixer/balance/ch11");
      mqttclient.subscribe("fbape/mixer/balance/ch12");
      mqttclient.subscribe("fbape/mixer/balance/ch13");
      mqttclient.subscribe("fbape/mixer/balance/ch14");
      mqttclient.subscribe("fbape/mixer/balance/ch15");
      mqttclient.subscribe("fbape/mixer/balance/ch16");
      mqttclient.subscribe("fbape/mixer/balance/ch17");
      mqttclient.subscribe("fbape/mixer/balance/ch18");
      mqttclient.subscribe("fbape/mixer/balance/ch19");
      mqttclient.subscribe("fbape/mixer/balance/ch20");
      mqttclient.subscribe("fbape/mixer/balance/ch21");
      mqttclient.subscribe("fbape/mixer/balance/ch22");
      mqttclient.subscribe("fbape/mixer/reset");
      mqttclient.subscribe("fbape/mixer/eq/reset");
      mqttclient.subscribe("fbape/mixer/dynamics/reset");
      mqttclient.subscribe("fbape/mixer/eq/peq");
      mqttclient.subscribe("fbape/mixer/eq/lp");
      mqttclient.subscribe("fbape/mixer/eq/hp");
      mqttclient.subscribe("fbape/mixer/adc/gain1");
      mqttclient.subscribe("fbape/mixer/gate1");
      mqttclient.subscribe("fbape/mixer/comp1");
      mqttclient.subscribe("fbape/mixer/comp2");
      mqttclient.subscribe("fbape/mixer/sync");
    }
  }

  void handleMQTT() {
    // check if we are connected to WiFi
    if (WiFi.status() == WL_CONNECTED) {
      if (!mqttclient.connected()) {
        // reconnect to MQTT-broker
        initMQTT();
      }else{
        // we connected to MQTT-broker
        mqttclient.loop(); // process incoming messages
      }
    }
  }

  void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
    payload[length] = '\0'; // null-terminate byte-array

    // convert MQTT-message "str/str/str/str payload" to command "str:str:str:str@payload"
    String sTopic = String(topic);
    sTopic.replace("/", ":");
    executeCommand(sTopic + "@" + String((char*)payload));
  }

  void mqttPublish() {
    if ((WiFi.status() == WL_CONNECTED) && mqttclient.connected()) {
      #if USE_SDPLAYER == 1
        uint32_t audioPosition = audio.getAudioCurrentTime();
        uint32_t audioDuration = audio.getAudioFileDuration();
        mqttclient.publish("fbape/status/player/playing", String(audio.isRunning()).c_str());
        mqttclient.publish("fbape/status/player/filesize", String(audio.getFileSize()).c_str());
        mqttclient.publish("fbape/status/player/duration", String(audioDuration).c_str());
        mqttclient.publish("fbape/status/player/position", String(audioPosition).c_str());
        if (audioDuration > 0) {
          mqttclient.publish("fbape/status/player/progress", String(((float)audioPosition/(float)audioDuration)*100.0f, 1).c_str());
        }else{
          mqttclient.publish("fbape/status/player/progress", "0");
        }
      #endif
      mqttclient.publish("fbape/status/player/currentfile", currentAudioFile.c_str());

      mqttclient.publish("fbape/status/mixer/vumeter/left", String(vu_meter_value[0]).c_str());
      mqttclient.publish("fbape/status/mixer/vumeter/right", String(vu_meter_value[1]).c_str());
      mqttclient.publish("fbape/status/mixer/vumeter/sub", String(vu_meter_value[2]).c_str());

      mqttclient.publish("fbape/status/mixer/volume/main", String(audiomixer.mainVolume).c_str());
      mqttclient.publish("fbape/status/mixer/volume/sub", String(audiomixer.mainVolumeSub).c_str());
      
      /*
      // TODO: here we could publish more data about individual channel-volumes, EQs, Gates, Compressors, etc.
      mqttclient.publish("fbape/status/mixer/volume/ch1", String(audiomixer.chVolume[0]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch2", String(audiomixer.chVolume[1]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch3", String(audiomixer.chVolume[2]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch4", String(audiomixer.chVolume[3]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch5", String(audiomixer.chVolume[4]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch6", String(audiomixer.chVolume[5]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch7", String(audiomixer.chVolume[6]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch8", String(audiomixer.chVolume[7]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch9", String(audiomixer.chVolume[8]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch10", String(audiomixer.chVolume[9]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch11", String(audiomixer.chVolume[10]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch12", String(audiomixer.chVolume[11]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch13", String(audiomixer.chVolume[12]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch14", String(audiomixer.chVolume[13]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch15", String(audiomixer.chVolume[14]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch16", String(audiomixer.chVolume[15]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch17", String(audiomixer.chVolume[16]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch18", String(audiomixer.chVolume[17]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch19", String(audiomixer.chVolume[18]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch20", String(audiomixer.chVolume[19]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch21", String(audiomixer.chVolume[20]).c_str());
      mqttclient.publish("fbape/status/mixer/volume/ch22", String(audiomixer.chVolume[21]).c_str());
      */
    }
  }
#endif

// command-interpreter
String executeCommand(String Command) {
  String Answer;

  if (Command.length()>2){
    // we got a new command. Lets find out what we have to do today...

	// enable Blue LED
	analogWrite(LED_BLUE, 0);
	LEDHoldCounter = 3; // set HoldCounter to 3 = 300ms

    if (Command.indexOf("wifi:ssid") > -1){
      // we received "wifi:ssid@TEXT"
      ssid = Command.substring(Command.indexOf("@")+1);
      ssid.trim(); // remove CR, LF and spaces

      Answer = "OK";
    }else if (Command.indexOf("wifi:password") > -1){
      // we received "wifi:password@TEXT"
      password = Command.substring(Command.indexOf("@")+1);
      password.trim(); // remove CR, LF and spaces

      Answer = "OK";
    }else if (Command.indexOf("wifi:ip?") > -1){
      if (WiFiAP)
        Answer = WiFi.softAPIP().toString();
      else
        Answer = WiFi.localIP().toString();
    }else if (Command.indexOf("wifi:ip") > -1){
      // we received "wifi:ip@192.168.0.1"
      String ParameterString = Command.substring(Command.indexOf("@")+1) + ".";
      uint8_t ip0 = split(ParameterString, '.', 0).toInt();
      uint8_t ip1 = split(ParameterString, '.', 1).toInt();
      uint8_t ip2 = split(ParameterString, '.', 2).toInt();
      uint8_t ip3 = split(ParameterString, '.', 3).toInt();
      
      ip = IPAddress(ip0, ip1, ip2, ip3);

      Answer = "OK";
    }else if (Command.indexOf("wifi:gateway") > -1){
      // we received "wifi:gateway@192.168.0.1"
      String ParameterString = Command.substring(Command.indexOf("@")+1) + ".";
      uint8_t ip0 = split(ParameterString, '.', 0).toInt();
      uint8_t ip1 = split(ParameterString, '.', 1).toInt();
      uint8_t ip2 = split(ParameterString, '.', 2).toInt();
      uint8_t ip3 = split(ParameterString, '.', 3).toInt();
      
      gateway = IPAddress(ip0, ip1, ip2, ip3);

      Answer = "OK";
    }else if (Command.indexOf("wifi:subnet") > -1){
      // we received "wifi:subnet@192.168.0.1"
      String ParameterString = Command.substring(Command.indexOf("@")+1) + ".";
      uint8_t ip0 = split(ParameterString, '.', 0).toInt();
      uint8_t ip1 = split(ParameterString, '.', 1).toInt();
      uint8_t ip2 = split(ParameterString, '.', 2).toInt();
      uint8_t ip3 = split(ParameterString, '.', 3).toInt();
      
      subnet = IPAddress(ip0, ip1, ip2, ip3);

      Answer = "OK";
    }else if (Command.indexOf("wifi:mode") > -1){
      // we received "wifi:mode@1" 0=client, 1=accesspoint
      // change WiFi-mode
      WiFiAP = (Command.substring(Command.indexOf("@")+1).toInt() == 1);

      Answer = "OK";
    }else if (Command.indexOf("wifi:enabled") > -1){
      if (Command.substring(Command.indexOf("@")+1).toInt() == 1) {
        // enable WiFi
        initWifi();
        //wifi.resume(); // alternatively
      }else{
        // disable WiFi
        WiFi.mode(WIFI_OFF);
      }
      Answer = "OK";
    #if USE_MQTT == 1
      }else if (Command.indexOf("wifi:mqttserver") > -1){
        // we received "wifi:mqttserver@192.168.0.1"
        String ParameterString = Command.substring(Command.indexOf("@")+1) + ".";
        uint8_t ip0 = split(ParameterString, '.', 0).toInt();
        uint8_t ip1 = split(ParameterString, '.', 1).toInt();
        uint8_t ip2 = split(ParameterString, '.', 2).toInt();
        uint8_t ip3 = split(ParameterString, '.', 3).toInt();
        
        mqtt_server = IPAddress(ip0, ip1, ip2, ip3);

        initMQTT();
        Answer = "OK";
    #endif
    #if USE_SDPLAYER == 1
      }else if (Command.indexOf("player:filesize?") > -1){
        // received command "player:filesize?"
        Answer = String(audio.getFileSize());
      }else if ((Command.indexOf("player:file") > -1) || (Command.indexOf("player:stream") > -1)){
        // received command "player:file@FILENAME.WAV" or "player:stream@http://...."
        currentAudioFile = Command.substring(Command.indexOf("@")+1);
        bool response = false;
        if (currentAudioFile.indexOf("http") > -1){
          // play stream
          response = audio.connecttohost(currentAudioFile.c_str());
        }else{
          // play file from SD-card
          response = audio.connecttoFS(SD, currentAudioFile.c_str()); // play audio-file
        }
        if (response) {
          Answer = "OK";
        }else{
          Answer = "ERROR";
        }
      }else if (Command.indexOf("player:currentfile?") > -1){
        Answer = currentAudioFile;
      }else if (Command.indexOf("player:toc?") > -1){
        Answer = SD_getTOC(1); // send as CSV to USB
      }else if (Command.indexOf("player:samdtoc?") > -1){
        Answer = "samd:toc@" + SD_getTOC(1); // send as CSV to SAMD
      }else if (Command.indexOf("player:play") > -1){
        // received command "player:play"
        bool response = false;
        if (currentAudioFile.indexOf("http") > -1){
          // play stream
          response = audio.connecttohost(currentAudioFile.c_str());
        }else{
          // play file from SD-card
          response = audio.connecttoFS(SD, currentAudioFile.c_str()); // play current audio-file
        }
        if (response) {
          Answer = "OK";
        }else{
          Answer = "ERROR";
        }
      }else if (Command.indexOf("player:loop") > -1){
        // received command "player:loop@X"
        bool looping = (Command.substring(Command.indexOf("@")+1).toInt() == 1);
        if (audio.setFileLoop(looping)) {
          Answer = "OK";
        }else{
          Answer = "ERROR";
        }
      }else if (Command.indexOf("player:pause") > -1){
        // received command "player:pause"
        if (audio.pauseResume()) {
          Answer = "OK";
        }else{
          Answer = "ERROR";
        }
      }else if (Command.indexOf("player:stop") > -1){
        // received command "player:stop"
        if (audio.stopSong()) {
          Answer = "OK";
        }else{
          Answer = "ERROR";
        }
      }else if (Command.indexOf("player:duration?") > -1){
        // received command "player:duration?"
        Answer = String(audio.getAudioFileDuration());
      }else if (Command.indexOf("player:position?") > -1){
        // received command "player:position?"
        Answer = String(audio.getAudioCurrentTime());
      }else if (Command.indexOf("player:position") > -1){
        // received command "player:position@0...100"
        if (currentAudioFile.indexOf("http") > -1){
          Answer = "ERROR: Cannot set position for stream!";
        }else{
          uint32_t position = (audio.getAudioFileDuration() * Command.substring(Command.indexOf("@")+1).toInt()) / 100;
          if (audio.connecttoFS(SD, currentAudioFile.c_str(), position)) {
            Answer = "OK";
          }else{
            Answer = "ERROR";
          }
        }
      }else if (Command.indexOf("player:volume") > -1){
        // player:volume@-140dBfs...0dBfs
        audio.setVolume(pow(10, Command.substring(Command.indexOf("@")+1).toFloat()/20.0f) * 21.0f);
        Answer = "OK";
    #endif
    #if USE_BLUETOOTH == 1
      }else if (Command.indexOf("bt:enabled") > -1){
        if (Command.substring(Command.indexOf("@")+1).toInt() == 1) {
          // enable bluetooth
          initBluetooth();
        }else{
          // disable bluetooth
          a2dp_sink.end(); // call with true to free memory, but with no chance to restart bluetooth
        }
        Answer = "OK";
      }else if (Command.indexOf("bt:play") > -1){
        // received command "bt:play"
        a2dp_sink.play();
        Answer = "OK";
      }else if (Command.indexOf("bt:pause") > -1){
        // received command "bt:pause"
        a2dp_sink.pause();
        Answer = "OK";
      }else if (Command.indexOf("bt:stop") > -1){
        // received command "bt:stop"
        a2dp_sink.stop();
        Answer = "OK";
      }else if (Command.indexOf("bt:next") > -1){
        // received command "bt:next"
        a2dp_sink.next();
        Answer = "OK";
      }else if (Command.indexOf("bt:prev") > -1){
        // received command "bt:prev"
        a2dp_sink.previous();
        Answer = "OK";
      }else if (Command.indexOf("bt:ff") > -1){
        // received command "bt:ff"
        a2dp_sink.fast_forward();
        Answer = "OK";
      }else if (Command.indexOf("bt:rw") > -1){
        // received command "bt:rw"
        a2dp_sink.rewind();
        Answer = "OK";
    #endif
    }else if (Command.indexOf("mixer:volume:main") > -1){
      // received command "mixer:volume:main@value"
      audiomixer.mainVolume = Command.substring(Command.indexOf("@")+1).toFloat();
      sendVolumeToFPGA(0); // update main-channel
      Answer = "OK";
    }else if (Command.indexOf("mixer:balance:main") > -1){
      // received command "mixer:balance:main@value"
      audiomixer.mainBalance = Command.substring(Command.indexOf("@")+1).toInt();
      sendVolumeToFPGA(0); // update main-channel
      Answer = "OK";
    }else if (Command.indexOf("mixer:volume:sub") > -1){
      // received command "mixer:volume:sub@value"
      audiomixer.mainVolumeSub = Command.substring(Command.indexOf("@")+1).toFloat();
      sendVolumeToFPGA(0); // update main-channel
      Answer = "OK";
    }else if (Command.indexOf("mixer:volume:ch") > -1){
      // received command "mixer:volume:chxx@value"
      uint8_t channel = Command.substring(15, Command.indexOf("@")).toInt();
      float value = Command.substring(Command.indexOf("@")+1).toFloat();

      if ((channel>=1) && (channel<=MAX_AUDIO_CHANNELS) && (value>=-140) && (value<=6)) {
        audiomixer.chVolume[channel-1] = value;
        sendVolumeToFPGA(channel);
        Answer = "OK";
      }else{
        Answer = "ERROR: Channel or value out of range!";
      }
    }else if (Command.indexOf("mixer:balance:ch") > -1){
      // received command "mixer:balance:chxx@yyy"
      uint8_t channel = Command.substring(16, Command.indexOf("@")).toInt();
      uint8_t value = Command.substring(Command.indexOf("@")+1).toInt();

      if ((channel>=1) && (channel<=MAX_AUDIO_CHANNELS) && (value>=0) && (value<=100)) {
        audiomixer.chBalance[channel-1] = value;
        sendVolumeToFPGA(channel);
        Answer = "OK";
      }else{
        Answer = "ERROR: Channel or value out of range!";
      }
    }else if (Command.indexOf("mixer:reset") > -1){
      initAudiomixer();
      Answer = "OK";
    }else if (Command.indexOf("mixer:eq:reset") > -1){
      // this function will set eq-coefficients to some standard values
      resetAudioFilters();
      sendFiltersToFPGA();
      Answer = "OK";
    }else if (Command.indexOf("mixer:dynamics:reset") > -1){
      // this function will set data for dynamics to some standard values
      resetDynamics();
      sendDynamicsToFPGA();
      Answer = "OK";
    }else if (Command.indexOf("mixer:eq:peq") > -1){
      // received command "mixer:eq:peq1@aaa,bbb,ccc,ddd"
      uint8_t peq = Command.substring(12, Command.indexOf("@")).toInt() - 1;
      String ParameterString = Command.substring(Command.indexOf("@")+1) + ",";
      uint8_t type = split(ParameterString, ',', 0).toInt();
      float frequency = split(ParameterString, ',', 1).toFloat();
      float quality = split(ParameterString, ',', 2).toFloat();
      float gain = split(ParameterString, ',', 3).toFloat();

      if ((peq>=0) && (peq<MAX_EQUALIZERS) && (type>=0) && (type<=7)) {
        audiomixer.peq[peq].type = type;
        audiomixer.peq[peq].fc = frequency;
        audiomixer.peq[peq].Q = quality;
        audiomixer.peq[peq].gain = gain;

        recalcFilterCoefficients_PEQ(&audiomixer.peq[peq]);
        sendDataToFPGA(FPGA_IDX_AUX_CMD); // disable filters
        for (int i=0; i<3; i++) { sendDataToFPGA(FPGA_IDX_PEQ+(peq*5)+i, &audiomixer.peq[peq].a[i]); } // 3 a-coefficients
        for (int i=0; i<2; i++) { sendDataToFPGA(FPGA_IDX_PEQ+3+(peq*5)+i, &audiomixer.peq[peq].b[i+1]); } // only 2 b-coefficients
        sendDataToFPGA(FPGA_IDX_AUX_CMD+1); // enable filters
        Answer = "OK";
      }else{
        Answer = "ERROR: PEQ out of range! PEQ #" + String(peq+1) + " and Type = " + String(type);
      }
    }else if (Command.indexOf("mixer:eq:lp") > -1){
      // received command "mixer:eq:lp@yyy"
      // this function will set LowPass-frequency
      float frequency = Command.substring(Command.indexOf("@")+1).toFloat();
      
      // check if desired frequency is above lowest supported value
      // background: Q35-fixed-point-numbers will limit to this value.
      if (frequency>=15) {
        audiomixer.LR24_LP_Sub.fc = frequency;
        recalcFilterCoefficients_LR24(&audiomixer.LR24_LP_Sub);

        // transmit the calculated coefficients to FPGA
        sendDataToFPGA(FPGA_IDX_AUX_CMD); // disable filters
        for (int i=0; i<5; i++) { sendDataToFPGA(FPGA_IDX_XOVER+9+i, &audiomixer.LR24_LP_Sub.a[i]); }
        for (int i=0; i<4; i++) { sendDataToFPGA(FPGA_IDX_XOVER+14+i, &audiomixer.LR24_LP_Sub.b[i+1]); } // we are not using b0
        sendDataToFPGA(FPGA_IDX_AUX_CMD+1); // enable filters
        Answer = "OK";
      }else{
        Answer = "ERROR: Frequency out of range! f_c = " + String(frequency);
      }
    }else if (Command.indexOf("mixer:eq:hp") > -1){
      // received command "mixer:eq:hp@yyy"
      // this function will set HighPass-frequency
      float frequency = Command.substring(Command.indexOf("@")+1).toFloat();
      
      // check if desired frequency is above lowest supported value
      // background: Q35-fixed-point-numbers will limit to this value.
      if (frequency>=15) {
        audiomixer.LR24_HP_LR.fc = frequency;
        recalcFilterCoefficients_LR24(&audiomixer.LR24_HP_LR);

        // transmit the calculated coefficients to FPGA
        sendDataToFPGA(FPGA_IDX_AUX_CMD); // disable filters
        for (int i=0; i<5; i++) { sendDataToFPGA(FPGA_IDX_XOVER+i, &audiomixer.LR24_HP_LR.a[i]); }
        for (int i=0; i<4; i++) { sendDataToFPGA(FPGA_IDX_XOVER+5+i, &audiomixer.LR24_HP_LR.b[i+1]); } // we are not using b0
        sendDataToFPGA(FPGA_IDX_AUX_CMD+1); // enable filters
        Answer = "OK";
      }else{
        Answer = "ERROR: Frequency out of range! f_c = " + String(frequency);
      }
    }else if (Command.indexOf("mixer:adc:gain") > -1){
      // received command "mixer:adc:gain1@gain"
      uint8_t channel = Command.substring(14, Command.indexOf("@")).toInt() - 1;
      uint8_t gain = Command.substring(Command.indexOf("@")+1).toInt();

      setADCgain(channel, gain);
      Answer = "OK";
    }else if (Command.indexOf("mixer:gate") > -1){
      // received command "mixer:gate1@aaa,bbb,ccc,ddd,eee"
      uint8_t gate = Command.substring(10, Command.indexOf("@")).toInt() - 1;
      String ParameterString = Command.substring(Command.indexOf("@")+1) + ",";

      if ((gate>=0) && (gate < MAX_NOISEGATES)) {
        audiomixer.gates[gate].threshold = split(ParameterString, ',', 0).toFloat();
        audiomixer.gates[gate].range = split(ParameterString, ',', 1).toFloat();
        audiomixer.gates[gate].attackTime_ms = split(ParameterString, ',', 2).toFloat();
        audiomixer.gates[gate].holdTime_ms = split(ParameterString, ',', 3).toFloat();
        audiomixer.gates[gate].releaseTime_ms = split(ParameterString, ',', 4).toFloat();

        // calculate values for FPGA
        recalcNoiseGate(&audiomixer.gates[gate]);

        // send data to FPGA
        sendDataToFPGA(FPGA_IDX_GATE+0+(gate*5), &audiomixer.gates[gate].value_threshold);
        sendDataToFPGA(FPGA_IDX_GATE+1+(gate*5), &audiomixer.gates[gate].value_gainmin);
        sendDataToFPGA(FPGA_IDX_GATE+2+(gate*5), &audiomixer.gates[gate].value_coeff_attack);
        sendDataToFPGA(FPGA_IDX_GATE+3+(gate*5), &audiomixer.gates[gate].value_hold_ticks);
        sendDataToFPGA(FPGA_IDX_GATE+4+(gate*5), &audiomixer.gates[gate].value_coeff_release);

        Answer = "OK";
      }else{
        Answer = "ERROR: Gate out of range! Gate #" + String(gate+1);
      }
    }else if (Command.indexOf("mixer:comp") > -1){
      // received command "mixer:comp1@aaa,bbb,ccc,ddd,eee,fff"
      uint8_t comp = Command.substring(10, Command.indexOf("@")).toInt() - 1;
      String ParameterString = Command.substring(Command.indexOf("@")+1) + ",";
      uint8_t ratio = split(ParameterString, ',', 1).toInt();

      // check if ratio is power of 2
      // within the FPGA a bit-shift is implemented for ratio-calculation, so we allow the following ratios: 0=oo:1, 1=1:1, 2=2:1, 4=4:1, 8=8:1, 16=16:1, 32=32:1, 64=64:1 and map it to 0,1,2,3,4,5,6,7
      if ((comp>=0) && (comp < MAX_COMPRESSORS) && ((ratio<=64) && ((ratio & (ratio - 1)) == 0))) {
        audiomixer.compressors[comp].threshold = split(ParameterString, ',', 0).toFloat();
        audiomixer.compressors[comp].ratio = ratio;
        audiomixer.compressors[comp].makeup = split(ParameterString, ',', 2).toInt();
        audiomixer.compressors[comp].attackTime_ms = split(ParameterString, ',', 3).toFloat();
        audiomixer.compressors[comp].holdTime_ms = saturateMin_f(split(ParameterString, ',', 4).toFloat(), 0.6f); // limit to minimum 0.6ms. Below this we have some distortions
        audiomixer.compressors[comp].releaseTime_ms = split(ParameterString, ',', 5).toFloat();

        // calculate values for FPGA
        recalcCompressor(&audiomixer.compressors[comp]);

        // send data to FPGA
        sendDataToFPGA(FPGA_IDX_COMP+0+(comp*6), &audiomixer.compressors[comp].value_threshold);
        sendDataToFPGA(FPGA_IDX_COMP+1+(comp*6), &audiomixer.compressors[comp].value_ratio);
        sendDataToFPGA(FPGA_IDX_COMP+2+(comp*6), &audiomixer.compressors[comp].value_makeup);
        sendDataToFPGA(FPGA_IDX_COMP+3+(comp*6), &audiomixer.compressors[comp].value_coeff_attack);
        sendDataToFPGA(FPGA_IDX_COMP+4+(comp*6), &audiomixer.compressors[comp].value_hold_ticks);
        sendDataToFPGA(FPGA_IDX_COMP+5+(comp*6), &audiomixer.compressors[comp].value_coeff_release);
        
        Answer = "OK";
      }else{
        Answer = "ERROR: Invalid value for compressor! Comp #" + String(comp+1) + " Ratio = " + String(ratio) + ":1";
      }
    }else if (Command.indexOf("mixer:sync") > -1){
      // received command "mixer:sync@0"
      setSyncSource(Command.substring(Command.indexOf("@")+1).toInt());
      Answer = "OK";
    }else if (Command.indexOf("mixer:samplerate") > -1){
      // received command "mixer:samplerate@48000"
      if (setSampleRate(Command.substring(Command.indexOf("@")+1).toInt())) {
        Answer = "OK";
      }else{
        Answer = "ERROR";
      }

/*
    }else if (Command.indexOf("test:senddata") > -1){
      // received command "test:senddata@yyy"
      float value = Command.substring(Command.indexOf("@")+1).toFloat();

      data_64b data;
      data.u64 = value * pow(2, 32);
      sendDataToFPGA(254, &data);
      Answer = "OK";
*/
    }else if (Command.indexOf("ver?") > -1){
      // return version of controller
      Answer = versionstring;
    }else if (Command.indexOf("*IDN?") > -1){
      Answer = "f/bape MainCtrl " + String(versionstring) + " built on " + String(compile_date);
    }else if (Command.indexOf("info?") > -1){
      Answer = "f/bape Audioplayer " + String(versionstring) + " built on " + String(compile_date) + "\nFPGA v" + FPGA_Version + "\n(c) Dr.-Ing. Christian Noeding\nhttps://www.github.com/xn--nding-jua/Audioplayer";
    }else if (Command.indexOf("system:debug:fpga") > -1){
      // receiving command "system:debug:fpga@0"
      debugRedirectFpgaSerialToUSB = (Command.substring(Command.indexOf("@")+1).toInt() == 1);
      Answer = "OK";
    }else if (Command.indexOf("system:init") > -1){
      initSystem();
      Answer = "OK";
    }else if (Command.indexOf("system:stop") > -1){
      // set system-flag to offline
      systemOnline = false;

      // stop the timers
      TimerSeconds.detach();
      Timer100ms.detach();

      Answer = "OK";
    }else if (Command.indexOf("system:card:init") > -1){
      // reinitialize the SD-Card
      initSD();

      Answer = "OK";
    }else if (Command.indexOf("system:config:read") > -1){
      configRead("/" + Command.substring(Command.indexOf("@")+1), 100); // read maximum of 100 lines
      Answer = "OK";
    }else if (Command.indexOf("system:config:write") > -1){
      configWrite("/" + Command.substring(Command.indexOf("@")+1));
      Answer = "OK";
    }else if (Command.indexOf("system:wificonfig:write") > -1){
      configWiFiWrite("/wifi.cfg");
      Answer = "OK";
    }else if (Command.indexOf("f/bape USBCtrl") > -1){
      USBCtrlIDN = Command;
      Answer = "OK";
    }else{
      // unknown command
      Answer = "UNKNOWN_CMD: " + Command;
    }
  }else{
    Answer = "ERROR";
  }

  return Answer;
}

// sending 48-bit values to fpga
void sendDataToFPGA(uint8_t cmd, data_64b *data) {
  uint8_t SerialCommand[11];
  data_16b ErrorCheckWord;

  ErrorCheckWord.u16 = data->u8[0] + data->u8[1] + data->u8[2] + data->u8[3] + data->u8[4] + data->u8[5];

  SerialCommand[0] = 65;  // A = start of command
  SerialCommand[1] = cmd;
  SerialCommand[2] = data->u8[5]; // MSB of payload
  SerialCommand[3] = data->u8[4];
  SerialCommand[4] = data->u8[3];
  SerialCommand[5] = data->u8[2];
  SerialCommand[6] = data->u8[1];
  SerialCommand[7] = data->u8[0]; // LSB of payload
  SerialCommand[8] = ErrorCheckWord.u8[1]; // MSB
  SerialCommand[9] = ErrorCheckWord.u8[0]; // LSB
  SerialCommand[10] = 69;  // E =  end of command

  // copy signed-bit from 64-bit value to 48bit-value
  if (data->u8[7] & 0b10000000) {
    bitSet(SerialCommand[2], 7);
  }else{
    bitClear(SerialCommand[2], 7);
  }

  Serial1.write(SerialCommand, sizeof(SerialCommand));
  delay(10); // delay for 10ms
}

// some overloaded functions for smaller data. For better handling with signed values
// within the FPGA, we will shift all data to the left to use the upper bits in FPGA
void sendDataToFPGA(uint8_t cmd, data_32b *data) {
  data_64b data64;
  data64.u64 = ((uint64_t)data->u32 << 16); // converting 32bit to 48bit

  // copy the signed-bit of 32-bit value to 64-bit-value to be able to use original sendDataToFPGA() function that expects 64-bit-values
  if (data64.u8[5] & 0b10000000) {
    bitSet(data64.u8[7], 7);
  }else{
    bitClear(data64.u8[7], 7);
  }

  sendDataToFPGA(cmd, &data64);
}
void sendDataToFPGA(uint8_t cmd, data_16b *data) {
  data_32b data32;
  data32.u32 = ((uint32_t)data->u16 << 16);
  sendDataToFPGA(cmd, &data32);
}
void sendDataToFPGA(uint8_t cmd) {
  data_64b data64; // some dummy-information
  sendDataToFPGA(cmd, &data64);
}

#if USE_DISPLAY == 1
  void updateSAMDDisplay() {
    Serial.println("samd:player:title@" + currentAudioFile);
    Serial.println("samd:player:volume:main@" + String(audiomixer.mainVolume, 1));
	  Serial.println("samd:player:balance:main@" + String(audiomixer.mainBalance));
    Serial.println("samd:player:volume:sub@" + String(audiomixer.mainVolumeSub, 1));
    Serial.println("samd:player:samplerate@" + String(audiomixer.sampleRate));

    #if USE_VUMETER == 1
      Serial.println("samd:vumeter:left@" + String(vu_meter_value[0], 1));
      //Serial.println("samd:vumeter:right@" + String(vu_meter_value[1], 1));
      //Serial.println("samd:vumeter:sub@" + String(vu_meter_value[2], 1));
    #endif

	  #if USE_SDPLAYER == 1
      uint32_t audioTime = audio.getAudioCurrentTime();
      uint32_t audioDuration = audio.getAudioFileDuration();
      Serial.println("samd:player:time@" + String(audioTime));
      Serial.println("samd:player:duration@" + String(audioDuration));
      if (audioDuration > 0) {
        Serial.println("samd:player:progress@" + String(((float)audioTime/(float)audioDuration)*100.0f, 0));
      }else{
        Serial.println("samd:player:progress@0");
      }
	  #endif
    //Serial.println("samd:player:updatedisplay"); // SAMD will update display every 100ms automatically
  }
#endif
 