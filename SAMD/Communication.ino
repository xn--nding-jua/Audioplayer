// USB-CMD-Receiver
void handleUSBCommunication() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // we are using both CR/LF but we have to read until LF
    command.trim();

    if (command.indexOf("samd:") == 0) {
      // command begins with "samd:" so we have to interprete it
      Serial.println(executeCommand(command));
    }else{
      // we received a command for NINA-module or FPGA -> passthrough command via Serial2 to NINA-Module
      Serial2.println(command); // "\n" has been truncated from command, so we have to use println() again
      Serial.println(Serial2.readStringUntil('\n')); // receive answer
    }
  }
}

#if USE_SERIAL1 == 1
  // Serial-CMD-Receiver
  void handleSerial1Communication() {
    if (Serial1.available() > 0) {
      String command = Serial1.readStringUntil('\n'); // we are using both CR/LF but we have to read until LF
      command.trim();

      if (command.indexOf("samd:") == 0) {
        // command begins with "samd:" so we have to interprete it
        Serial1.println(executeCommand(command));
      }else{
        // we received a command for NINA-module or FPGA -> passthrough command via Serial2 to NINA-Module
        Serial2.println(command); // "\n" has been truncated from command, so we have to use println() again
        Serial1.println(Serial2.readStringUntil('\n')); // receive answer
      }
    }
  }
#endif

// NINA-CMD-Receiver
void handleNINACommunication() {
  // passthrough all incoming data to USB-serial
  if (Serial2.available() > 0) {
    if (passthroughNINA) {
      //Serial.write(Serial2.read()); // this prevents us from receiving commands from NINA
      String command = Serial2.readStringUntil('\n');
      command.trim();
      
      if (command.indexOf(F("samd:")) == 0) {
        // command begins with "samd:" so we have to interprete it
        Serial2.println(executeCommand(command));
      }else{
        // passthrough command to USB
        Serial.println(command);

        #if USE_SERIAL1 == 1
          // passthrough command to Serial1
          //Serial1.println(command); // if Serial1 is much slower than 115200 baud, this could cause some problems. Uncomment if you like this function
        #endif
      }
    }else{
      String command = Serial2.readStringUntil('\n');
      command.trim();

      if (command.indexOf("OK") > -1) {
        // received "OK" from NINA -> last command is OK
      }else if (command.indexOf("ERROR") > -1) {
        // received "ERROR" from NINA -> last command had an error
      }else if (command.indexOf("UNKNOWN_CMD") > -1) {
        // NINA received some garbage or communication-problem -> ignore it for the moment
      }else{
        // this is a command for our command-processor
        Serial2.println(executeCommand(command));
      }
    }
  }
}

// command-interpreter
String executeCommand(String Command) {
  String Answer;

  if (Command.length()>2){
    // we got a new command. Lets find out what we have to do today...

    #if USE_DISPLAY == 1
      if (Command.indexOf(F("samd:toc")) > -1){
        TOC = Command.substring(Command.indexOf("@")+1);
        Answer = "OK";
      }else if (Command.indexOf(F("samd:player:title")) > -1){
        playerinfo.title = Command.substring(Command.indexOf("@")+1);
        Answer = "OK";
      }else if (Command.indexOf("samd:player:volume:main") > -1){
        playerinfo.volumeMain = Command.substring(Command.indexOf("@")+1).toFloat();
        Answer = "OK";
      }else if (Command.indexOf("samd:player:balance:main") > -1){
        playerinfo.balanceMain = Command.substring(Command.indexOf("@")+1).toInt();
        Answer = "OK";
      }else if (Command.indexOf("samd:player:volume:sub") > -1){
        playerinfo.volumeSub = Command.substring(Command.indexOf("@")+1).toFloat();
        Answer = "OK";
      }else if (Command.indexOf(F("samd:player:volume:analog")) > -1){
        playerinfo.volumeAnalog = Command.substring(Command.indexOf("@")+1).toFloat();
        Answer = "OK";
      }else if (Command.indexOf(F("samd:player:volume:card")) > -1){
        playerinfo.volumeCard = Command.substring(Command.indexOf("@")+1).toFloat();
        Answer = "OK";
      }else if (Command.indexOf(F("samd:player:samplerate")) > -1){
        playerinfo.samplerate = Command.substring(Command.indexOf("@")+1).toInt();
        Answer = "OK";
      }else if (Command.indexOf("samd:player:time") > -1){
        playerinfo.time = Command.substring(Command.indexOf("@")+1).toInt();
        Answer = "OK";
      }else if (Command.indexOf("samd:player:duration") > -1){
        playerinfo.duration = Command.substring(Command.indexOf("@")+1).toInt();
        Answer = "OK";
      }else if (Command.indexOf("samd:player:progress") > -1){
        playerinfo.progress = Command.substring(Command.indexOf("@")+1).toInt();
        Answer = "OK";
      }else if (Command.indexOf("samd:player:updatedisplay") > -1){
        displayDrawMenu();
        Answer = "OK";
      }else if (Command.indexOf("samd:version:main") > -1){
        playerinfo.MainCtrlVersion = Command.substring(Command.indexOf("@")+1);
        Answer = "OK";
      }else if (Command.indexOf("samd:version:fpga") > -1){
        playerinfo.FPGAVersion = Command.substring(Command.indexOf("@")+1);
        Answer = "OK";
      }else if (Command.indexOf("samd:ver?") > -1){
    #else
      if (Command.indexOf("samd:ver?") > -1){
    #endif
      // return version of controller
      Answer = versionstring;
    }else if (Command.indexOf("samd:*IDN?") > -1){
      // return some short info-text for the GUI
      Answer = "f/bape USBCtrl " + String(versionstring) + " built on " + String(compile_date);
    }else if (Command.indexOf("samd:info?") > -1){
      // return some info-text for the GUI
      Answer = "f/bape USB-Controller " + String(versionstring) + " built on " + String(compile_date) + "\n(c) Dr.-Ing. Christian Noeding\nhttps://www.github.com/xn--nding-jua/Audioplayer";
    }else if (Command.indexOf("samd:reset:fpga") > -1){
      setup_fpga();
      Answer = "OK";
    }else if (Command.indexOf("samd:reset:nina") > -1){
      resetNina();
      Answer = "OK";
    }else if (Command.indexOf(F("samd:update:nina")) > -1){
      enterNinaUpdateMode(true);
      Answer = F("Entered NINA-Update-Mode...\nPlease close serial-port and upload new firmware via Arduino or esptool.py.\n\nReboot system to return to normal mode.");
    }else if (Command.indexOf("samd:passthrough:nina") > -1){
      passthroughNINA = (Command.substring(Command.indexOf("@")+1).toInt() == 1);
      Answer = "OK";
    #if USE_VUMETER == 1
      }else if (Command.indexOf("samd:vumeter:left") > -1){
        vuMeterVolumeLeft = Command.substring(Command.indexOf("@")+1).toFloat(); // 0...255.0
        Answer = "OK";
      }else if (Command.indexOf("samd:vumeter:right") > -1){
        vuMeterVolumeRight = Command.substring(Command.indexOf("@")+1).toFloat(); // 0...255.0
        Answer = "OK";
      }else if (Command.indexOf("samd:vumeter:sub") > -1){
        vuMeterVolumeSub = Command.substring(Command.indexOf("@")+1).toFloat(); // 0...255.0
        Answer = "OK";
    #endif
    }else{
      // unknown command
      Answer = "SAMD: UNKNOWN_CMD";
    }
  }else{
    Answer = "SAMD: ERROR";
  }

  return Answer;
}

// Interrupt handler for SERCOM3
void SERCOM3_Handler()
{
  Serial2.IrqHandler();
}

void resetNina() {
  pinMode(NINA_RESET_N, OUTPUT);
  digitalWrite(NINA_RESET_N, HIGH);
  delay(100);
  digitalWrite(NINA_RESET_N, LOW);
  delay(100);
  digitalWrite(NINA_RESET_N, HIGH);
  delay(100);
  pinMode(NINA_RESET_N, INPUT);
}

void enterNinaUpdateMode(bool performReset) {
  #if USE_DISPLAY == 1
    // disable ticker that access I2C
    ticker100ms.stop();

    // show update-information on display
    display.clearDisplay();
    currentDisplayLine = 0;
    displayPrintLn(F("< NINA Updatemode >"));
    displayPrintLn("");
    displayPrintLn(F("Please upload new"));
    displayPrintLn(F("Firmware and reboot"));
    displayPrintLn(F("the system..."));

    // stop I2C as SDA is connected to Bootselect-Pin of NINA-W102
    Wire.end();
    delay(1000);
  #endif
  
  // set reset- and bootmode-pins into output-mode
  pinMode(NINA_PIO27, OUTPUT);
  pinMode(NINA_RESET_N, OUTPUT);
  digitalWrite(NINA_PIO27, LOW); // PIO27 = LOW = Bootstrap pin for NINA-W102 -> put into Bootloader-Mode (Factory Boot)
  digitalWrite(NINA_RESET_N, HIGH);

  if (performReset) {
    // perform a reset of the NINA-module
    delay(100);
    digitalWrite(NINA_RESET_N, LOW);
    delay(100);
    digitalWrite(NINA_RESET_N, HIGH);
    delay(100);
  }

  firmwareUpdateMode = true; // entering NINA-Update-Mode
}

void handleNINAUpdate() {
  // use RTS to RESET the NINA-module
  if (rts != Serial.rts()) {
    digitalWrite(NINA_RESET_N, (Serial.rts() == 1) ? LOW : HIGH);
    rts = Serial.rts();
  }

  // use DTR to select/deselect Bootloader-Mode
  if (dtr != Serial.dtr()) {
    digitalWrite(NINA_PIO27, (Serial.dtr() == 1) ? LOW : HIGH);
    dtr = Serial.dtr();
  }

  // copy bytes from USB to NINA
  if (Serial.available()) {
    Serial2.write(Serial.read());
  }
  // copy bytes from NINA to USB
  if (Serial2.available()) {
    Serial.write(Serial2.read());
  }

  // check if the USB virtual serial wants a new baud rate
  if (Serial.baud() != baud) {
    rts = -1;
    dtr = -1;

    baud = Serial.baud();
    Serial2.flush();
    Serial2.begin(baud);
    pinPeripheral(0, PIO_SERCOM); //Assign TX function to pin 0
    pinPeripheral(1, PIO_SERCOM); //Assign RX function to pin 1
  }
}