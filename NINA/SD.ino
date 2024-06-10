bool initSD() {
  // setup the chip-select-pin
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // init SPI
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI); // (int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1)
  SPI.setFrequency(SPI_CLOCK);

  // init SD-card
  if (SD.begin(SD_CS, SPI, SPI_CLOCK)) { //(uint8_t ssPin=SS, SPIClass &spi=SPI, uint32_t frequency=4000000, const char * mountpoint="/sd", uint8_t max_files=5, bool format_if_empty=false);
    // init done successfully
    return true;
  }else{
    // an error occurred during SD-card-initialization
    return false;
  }
}

uint16_t SD_getNumberOfFiles(File dir, String filter) {
  // at the moment we are supporting only files in the root-directory

  uint16_t fileCount = 0;

  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    if (!entry.isDirectory()) {
      if ((filter.length() == 0) || (String(entry.name()).indexOf(filter) > -1)) {
        fileCount += 1;
      }
    }
    entry.close();
  }
  
  return fileCount;
}

String SD_getFileName(File dir, String filter, uint16_t fileNumber) {
  // at the moment we are supporting only files in the root-directory
  uint16_t fileCount = 0;
  String s;

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      return "ERROR";
    }
    if (!entry.isDirectory()) {
      s = entry.name();
      if ((filter.length() == 0) || (s.indexOf(filter) > -1)) {
        if (fileCount == fileNumber) {
          return s;
        }
        fileCount += 1;
      }
    }
    entry.close();
  }
}

String SD_getFileSize(File dir, String filter, uint16_t fileNumber) {
  // at the moment we are supporting only files in the root-directory
  uint16_t fileCount = 0;
  String s;

  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      return "ERROR";
    }
    if (!entry.isDirectory()) {
      s = entry.name();
      if ((filter.length() == 0) || (s.indexOf(filter) > -1)) {
        if (fileCount == fileNumber) {
          return formatSize(entry.size());
        }
        fileCount += 1;
      }
    }
    entry.close();
  }
}

String SD_getTOC(uint8_t format) { // 0=json, 1=csv
  // at the moment we are supporting only files in the root-directory
  String s;
  String TOC = "";
  File root = SD.open("/");
  bool running = true;

  while (running) {
    File entry = root.openNextFile();

    if (!entry) {
      running = false;
    }else{
      if (!entry.isDirectory()) {
        // we have a valid file. Check file-type

        s = entry.name();
        if ((s.indexOf(".mp3") > -1) || (s.indexOf(".wav") > -1)) {
          // we found a mp3 or wave-file
          if (format == 0) {
            // JSON
            if (TOC.length()>0) {
              TOC += ",\"";
            }else{
              TOC += "\"";
            }
            TOC += s;
            TOC += "\":\"";
            TOC += formatSize(entry.size());
            TOC += "\"";
          }else if (format == 1) {
            // CSV
            if (TOC.length()>0) {
              TOC += ",";
            }
            TOC += s;
          }
        }
      }
      entry.close();
    }
  }

  return TOC;
}