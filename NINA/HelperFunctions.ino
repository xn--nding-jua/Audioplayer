// saturates a value to a specific minimum value
uint8_t limitMin(uint8_t value, uint8_t min) {
  if (value<min) {
    return min;
  }else{
    return value;
  }
}

// saturates a value to a specific maximum value
uint8_t limitMax(uint8_t value, uint8_t max) {
  if (value>max) {
    return max;
  }else{
    return value;
  }
}

// saturates a float-value to a specific minimum value
float saturateMin_f(float value, float min) {
  if (value<min) {
    return min;
  }else{
    return value;
  }
}
// saturates a float-value to a specific maximum value
float saturateMax_f(float value, float max) {
  if (value>max) {
    return max;
  }else{
    return value;
  }
}

// saturates a value to a specific minimum and maximum value
uint8_t saturate(uint8_t value, uint8_t min, uint8_t max) {
  if (value>max) {
    return max;
  }else if (value<min) {
    return min;
  }else{
    return value;
  }
}

// saturates a float-value to a specific minimum and maximum value
float saturate_f(float value, float min, float max) {
  if (value>max) {
    return max;
  }else if (value<min) {
    return min;
  }else{
    return value;
  }
}

String formatSize(uint32_t size) {
  if (size < 1024) {
    return String(size) + " B";
  }else if (size < (1024*1024)) {
    return String(size/1024.0f, 2) + " kB";
  }else if (size < (1024*1024*1024)) {
    return String(size/(1048576.0f), 2) + " MB";
  }else{
    return String(size/(1073741824.0f), 2) + " GB";
  }
}

String formatTime(uint32_t seconds) {
  String timeString;

  uint8_t t_h = (seconds / 3600);
  uint8_t t_min = (seconds % 3600) / 60;
  uint8_t t_s = (seconds % 3600) % 60;
  
  if (seconds < 60) {
    timeString = String(t_s) + "s";
  }else if (seconds < (60*60)) { // min:s
    timeString = String(t_min) + "min " + String(t_s) + "s";
  }else if (seconds < (60*60*24)) { // h:min:s
    timeString = String(t_h) + "h " + String(t_min) + "min " + String(t_s) + "s";
  }

  return timeString;
}

// return specific element of char-separated string
String split(String s, char parser, int index) {
  String rs="";
  int parserIndex = index;
  int parserCnt=0;
  int rFromIndex=0, rToIndex=-1;
  while (index >= parserCnt) {
    rFromIndex = rToIndex+1;
    rToIndex = s.indexOf(parser,rFromIndex);
    if (index == parserCnt) {
      if (rToIndex == 0 || rToIndex == -1) return "";
      return s.substring(rFromIndex,rToIndex);
    } else parserCnt++;
  }
  return rs;
}


// converting byte-array to HEX-string
void arrayToString(uint8_t array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    uint8_t nib1 = (array[i] >> 4) & 0x0F;
    uint8_t nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}

// clear serial input-buffer
void serialFlush() {
  while (Serial.available()) {
    Serial.read();
  }
}

// convert seconds to string of hour, minute and seconds
String secondsToHMS(uint32_t seconds){
  unsigned int tme=0;
  tme = seconds;

  int hr = tme/3600;                        //Number of seconds in an hour
  int mins = (tme-hr*3600)/60;              //Remove the number of hours and calculate the minutes.
  int sec = tme-hr*3600-mins*60;            //Remove the number of hours and minutes, leaving only seconds.
  sec %= 60;
  mins %= 60;
  hr %= 24;

  if (seconds < 60) {
    // show only seconds
    return String(sec) + "s";
  }else if (seconds < 3600){
    return (String(mins) + "min " + String(sec) + "s");
  }else{
    return (String(hr) + "h " + String(mins) + "min " + String(sec) + "s");
  }
}

uint32_t wrapAround(uint32_t value, uint32_t max) {
  if (value>max) {
    return value-max;
  }else{
    return value;
  }
}

float wrapAround(float value, float max) {
  if (value>max) {
    return value-max;
  }else{
    return value;
  }
}

// converts a IP-Address to a string
String ipAddress2String(const IPAddress& address){
  return String(address[0]) + "." + address[1] + "." + address[2] + "." + address[3];
}

// converts a MAC-Address to a string
String mac2String(uint8_t ar[]) {
  String s;
  for (uint8_t i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%02X", ar[i]); // J-M-L: slight modification, added the 0 in the format for padding 
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

/*
float log2(float inputValue) {
  return log(inputValue) / log(2); 
}
*/