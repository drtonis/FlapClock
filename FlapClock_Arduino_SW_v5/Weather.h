/*
  Dr. Tõnis
  Pfäffikon 2023
  Switzerland

  the code is from https://techtutorialsx.com/2017/05/19/esp32-http-get-requests/

  https://www.youtube.com/watch?v=pD1_b2MtJaw / https://github.com/bdash9/ESP32-OpenWeatherMap
*/

void get_Weather_data() {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
  
    HTTPClient http;

    http.begin(OPW_server); //Specify the URL
    int httpCode = http.GET(); //Make the request
  
    if (httpCode > 0) { //Check for the returning code
  
        OPW_text = http.getString();
        Serial.println(httpCode);
        Serial.println(OPW_text);
      }
  
    else {
      Serial.println("Error on HTTP request");
    }
  
    http.end(); //Free the resources
  }
}
