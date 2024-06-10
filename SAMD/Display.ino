#if USE_DISPLAY == 1
  bool initDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      return false;
    }

    display.setTextSize(1);
    display.setTextColor(WHITE);

    return true;
  }

  void displayShowLogo(uint8_t logo) {
    display.clearDisplay();
    if (logo == 0) {
      display.drawBitmap(0, 0, bitmap_card_logo, 128, 64, 1);
    }
    if (logo == 1) {
      display.drawBitmap(0, 0, fbape_logo, 128, 64, 1);
    }
    display.display();
  }

  void displayText(uint8_t line, String text) {
    display.setCursor(0, line*11);
    display.setTextColor(WHITE);
    display.print(text);
    display.display();
  }

  void displayPrintLn(String text) {
    if (currentDisplayLine==6) {
      display.clearDisplay();
      currentDisplayLine = 0;
    }

    display.setCursor(0, currentDisplayLine*11);
    display.print(text);
    display.display();

    // increment current display-line for next text-line
    currentDisplayLine += 1;
  }

  void displayPrintMenuOption(uint8_t displayOption, String text) {
    if (displayOption == currentDisplayOption) {
      // invert color to highlight this option
      display.setTextColor(BLACK, WHITE);
      display.print(text);
      display.setTextColor(WHITE);
    }else{
      display.print(text);
    }
  }

  // Online-Display-Designer: https://github.com/rickkas7/DisplayGenerator
  // Online Bitmap-Converter for SD1308: https://javl.github.io/image2cpp
  void displayDrawMenu() {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    if (currentDisplayMenu == 0) {
      // main-menu (Title, Progress, Volume)
      display.print(F("f/bape>"));
      displayPrintMenuOption(0, F("Main"));
      display.drawLine(0, 10, 128, 10, 1);
      display.setCursor(0, 15);
      display.print("T:" + playerinfo.title);
      display.setCursor(0, 27);
      display.print(secondsToHMS(playerinfo.time) + " / " + secondsToHMS(playerinfo.duration));
      display.fillRect(0, 40, playerinfo.progress, 7, 1);
      display.setCursor(104, 40);
      display.print(String(playerinfo.progress) + "%");
      display.drawLine(0, 50, 128, 50, 1);
      display.setCursor(0, 55);
      display.print("LR");
      displayPrintMenuOption(1, String(playerinfo.volumeMain, 1) + "dB");
      display.setCursor(64, 55);
      display.print("Sub");
      displayPrintMenuOption(2, String(playerinfo.volumeSub, 1) + "dB");
    }else if (currentDisplayMenu == 1) {
      // playlist
      display.print(F("f/bape>"));
      displayPrintMenuOption(0, F("Playlist"));
      display.drawLine(0, 10, 128, 10, 1);

      display.setCursor(0, 15);
      displayPrintMenuOption(1, split(TOC, ',', 0 + TOC_Offset));

      display.setCursor(0, 25);
      displayPrintMenuOption(2, split(TOC, ',', 1 + TOC_Offset));

      display.setCursor(0, 35);
      displayPrintMenuOption(3, split(TOC, ',', 2 + TOC_Offset));

      display.setCursor(0, 45);
      displayPrintMenuOption(4, split(TOC, ',', 3 + TOC_Offset));

      display.setCursor(0, 55);
      displayPrintMenuOption(5, split(TOC, ',', 4 + TOC_Offset));
    }else if (currentDisplayMenu == 2) {
      // volume
      display.print(F("f/bape>"));
      displayPrintMenuOption(0, F("Volume"));
      display.drawLine(0, 10, 128, 10, 1);

      display.setCursor(0, 15);
      display.print(F("Left/Right"));
      display.setCursor(85, 15);
      displayPrintMenuOption(1, String(playerinfo.volumeMain, 1) + "dB");

      display.setCursor(0, 25);
      display.print(F("Balance"));
      display.setCursor(85, 25);
      displayPrintMenuOption(2, String(playerinfo.balanceMain) + "%");

      display.setCursor(0, 35);
      display.print(F("Subwoofer"));
      display.setCursor(85, 35);
      displayPrintMenuOption(3, String(playerinfo.volumeSub, 1) + "dB");

      display.setCursor(0, 45);
      display.print(F("Analog Input"));
      display.setCursor(85, 45);
      displayPrintMenuOption(4, String(playerinfo.volumeAnalog, 1) + "dB");

      display.setCursor(0, 55);
      display.print(F("SDCard Input"));
      display.setCursor(85, 55);
      displayPrintMenuOption(5, String(playerinfo.volumeCard, 1) + "dB");
    }else if (currentDisplayMenu == 3) {
      // setup
      display.print(F("f/bape>"));
      displayPrintMenuOption(0, F("Setup"));
      display.drawLine(0, 10, 128, 10, 1);

      display.setCursor(0, 15);
      display.print(F("WiFi"));
      display.setCursor(85, 15);
      displayPrintMenuOption(1, String(playerinfo.WiFiEnabled));

      display.setCursor(0, 25);
      display.print(F("Bluetooth"));
      display.setCursor(85, 25);
      displayPrintMenuOption(2, String(playerinfo.BluetoothEnabled));

      display.setCursor(0, 35);
      display.print(F("Samplerate"));
      display.setCursor(85, 35);
      displayPrintMenuOption(3, String(playerinfo.samplerate)); // 44100, 48000, 96000

  /*
      display.setCursor(0, 45);
      display.print(F("Option 4"));
      display.setCursor(85, 45);
      displayPrintMenuOption(4, "0");

      display.setCursor(0, 55);
      display.print(F("Option 5"));
      display.setCursor(85, 55);
      displayPrintMenuOption(5, "0");
  */
    }else if (currentDisplayMenu == 4) {
      // setup
      display.print(F("f/bape>"));
      displayPrintMenuOption(0, F("Status"));
      display.drawLine(0, 10, 128, 10, 1);

      display.setCursor(0, 15);
      display.print(F("MainCtrl"));
      display.setCursor(85, 15);
      display.print(playerinfo.MainCtrlVersion);

      display.setCursor(0, 25);
      display.print(F("USBCtrl"));
      display.setCursor(85, 25);
      display.print(String(versionstring));

      display.setCursor(0, 35);
      display.print(F("FPGA"));
      display.setCursor(85, 35);
      display.print(playerinfo.FPGAVersion);

      display.setCursor(0, 45);
      display.print(F("www.github.com/"));

      display.setCursor(0, 55);
      display.print(F("  xn--nding-jua"));
    }else if (currentDisplayMenu == 5) {
      // debug
      display.print(F("f/bape>"));
      displayPrintMenuOption(0, F("Debug"));
      display.drawLine(0, 10, 128, 10, 1);

      display.setCursor(0, 15);
      display.print(F("External GPIO"));
      display.setCursor(85, 15);
      display.print(String(externalInputs.value));

      display.setCursor(0, 25);
      display.print(F("Poti [p.u.]"));
      display.setCursor(85, 25);
      display.print(String(potiValue, 3));

/*
      display.setCursor(0, 35);
      display.print("");
      display.setCursor(85, 35);
      display.print("");

      display.setCursor(0, 45);
      display.print(F(""));
      display.setCursor(85, 45);
      display.print("");

      display.setCursor(0, 55);
      display.print("");
      display.setCursor(85, 55);
      display.print("");
*/
    }else{
      display.println(F("Error showing"));
      display.print(F("menu #"));
      display.println(String(currentDisplayMenu));
    }
    display.display();
  }
#endif