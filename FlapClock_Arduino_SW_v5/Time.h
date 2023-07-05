/*
  Dr. Tõnis
  Pfäffikon 2023
  Switzerland

  big help from https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
  
*/

// needed for the following functions
  tm timeinfo;                              // the structure tm holds time information in a more convient way

void setTimezone(String timezone) {
  Serial.printf("  Setting Timezone to %s\n", timezone.c_str());
  setenv("TZ", timezone.c_str(), 1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void init_time_server(String timezone) {
  Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if(!getLocalTime(&timeinfo)) {
    Serial.println("  Failed to obtain time");
    return;
  }
  Serial.println("  Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

void get_time_now() {
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time 1");
    return;
  }  
}



void check_time_change_minute() {
//  time_now[3] += 1;
//  if (time_now[3] > 9) {
//    time_now[3] = 0;
//    time_now[2] += 1;
//  }
//  if (time_now[2] > 5) {
//    time_now[2] = 0;
//    time_now[1] += 1;
//  }
//  if (time_now[1] > 9) {
//    time_now[1] = 0;
//    time_now[0] += 1;
//  }
//
//  if (time_now[0] > 2) time_now[0] = 0;
//
//  for (int i = 0; i < 4; i++) { 
//    Serial.print(time_now[i]);
//    Serial.print(" ");
//  }
//Serial.println(" ");
//  // we do something if minute changes
//  if (old_minute != timeinfo.tm_min) {
//    // save the current minute value
//      old_minute = timeinfo.tm_min;
//    // adjust the values
////    time_now[0] = int(tm.tm_hour/10);
////    time_now[1] = int(tm.tm_hour%10);
////    time_now[2] = int(tm.tm_min/10);
////    time_now[3] = int(tm.tm_min%10);
//  }
}
