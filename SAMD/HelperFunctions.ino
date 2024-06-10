// saturates a value to a specific minimum and maximum value
float saturate(float value, float min, float max) {
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

float dBfs2pu(float dBfs, float min) {
  float dBfs_tmp;
  if (dBfs < min) {
    dBfs_tmp = -120;
  }else{
    dBfs_tmp = dBfs;
  }

  float pu = pow(10.0f, dBfs_tmp/100.0f); // 10^(dBfs/100)
  if (pu > 1) {
    pu = 1;
  }
  return pu;
}

#if USE_DISPLAY == 1
  String split(String s, char parser, int index) {
    String rs="";
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
#endif