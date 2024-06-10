#if USE_SDPLAYER == 1
  void initAudio() {
    xTaskCreatePinnedToCore(
      AudioTaskFcn, // Function to implement the task
      "AudioTask", // Name of the task
      10000,  // Stack size in words
      NULL,  // Task input parameter
      2 | portPRIVILEGE_BIT,  // Priority of the task: tskIDLE_PRIORITY, 1, 2
      &AudioTask,  // Task handle.
      0  // Core where the task should run (Core0 has WiFi- and BT-Stack. Check if this is a problem!)
    );
  }

  void AudioTaskFcn(void *pvParameters) {
    // init audio-system
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(AUDIO_INIT_VOLUME);

    while(true) {
      //This function needs to be called continuously.
      audio.loop();

      //yield(); // yield() yields to tasks with the same priority or higher. So it doesn't yield to the idle task
      delay(1); // give idle-task time to do its work so that the watchdog is not biting us
      //vTaskDelay(10);
    }
  }
#endif

#if USE_BLUETOOTH == 1
  // callback which is notified on update
  void bt_rssi(esp_bt_gap_cb_param_t::read_rssi_delta_param  &rssiParam){
    //Serial.print("rssi value: ");
    //Serial.println(rssiParam.rssi_delta);
  }

  // for esp_a2d_connection_state_t see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_a2dp.html#_CPPv426esp_a2d_connection_state_t
  void bt_connection_state_changed(esp_a2d_connection_state_t state, void *ptr){
  //  Serial.println(a2dp_sink.to_str(state));
  }

  // for esp_a2d_audio_state_t see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_a2dp.html#_CPPv421esp_a2d_audio_state_t
  void bt_audio_state_changed(esp_a2d_audio_state_t state, void *ptr){
  //  Serial.println(a2dp_sink.to_str(state));
  }

  // for esp_a2d_connection_state_t see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_a2dp.html#_CPPv426esp_a2d_connection_state_t
  void bt_samplerate_changed(uint16_t rate){
    if (rate != audiomixer.sampleRate) {
      // update the current samplerate
      setSampleRate(rate);
    }
  }

  void bt_avrc_metadata_callback(uint8_t id, const uint8_t *text) {
    #if USE_SDPLAYER == 1
      bool SD_playing = audio.isRunning();
    #else
      bool SD_playing = false;
    #endif

    switch(id) {
      case 0x01: // title
        if (!SD_playing) {
          currentAudioFile = (char*)text;
        }
        break;
      case 0x02: // artist
        break;
      case 0x03: // album
        break;
      case 0x04: // current track
        break;
      case 0x05: // total tracks
        break;
      case 0x06: // genre
        break;
      case 0x07: // playing-time in seconds
        if (!SD_playing) {
          currentAudioPosition = String((char*)text).toInt();
        }
        break;
    }
  }

  void initBluetooth() {
    auto cfg = i2s.defaultConfig(); // i2s.defaultConfig(RXTX_MODE), i2s.defaultConfig(RX_MODE)
    cfg.i2s_format = I2S_STD_FORMAT;
    cfg.bits_per_sample = BT_BITS;
    cfg.channels = 2;
    cfg.sample_rate = 48000;
    cfg.is_master = true;
    cfg.pin_bck = BT_I2S_BCLK;
    cfg.pin_ws = BT_I2S_LRC;
    cfg.pin_data = BT_I2S_DOUT;
    cfg.pin_data_in = I2S_PIN_NO_CHANGE;
    i2s.begin(cfg);

    // set desired bits per sample (already done above)
    //a2dp_sink.set_bits_per_sample(BT_BITS);
    
    // allow reconnections
    //a2dp_sink.set_auto_reconnect(true);

    // enable signal-strength-measurements
    //a2dp_sink.set_rssi_active(true);
    //a2dp_sink.set_rssi_callback(bt_rssi);

    // handle audio- and state-changes
    //a2dp_sink.set_on_connection_state_changed(bt_connection_state_changed);
    //a2dp_sink.set_on_audio_state_changed(bt_audio_state_changed);
    a2dp_sink.set_sample_rate_callback(bt_samplerate_changed);
    a2dp_sink.set_avrc_metadata_callback(bt_avrc_metadata_callback);
    a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_PLAYING_TIME); // only get specific meta-data

    // start bluetooth
    a2dp_sink.set_auto_reconnect(true, AUTOCONNECT_TRY_NUM);
    a2dp_sink.start(hostname.c_str()); // connect using hostname as SSID, use auto-reconnect

    // some more functions
    /*
    a2dp_sink.is_connected()
    a2dp_sink.set_volume()
    a2dp_sink.set_task_core()
    a2dp_sink.has_sound_data()
    esp_a2d_mct_t get_audio_type()
    */
  }
#endif