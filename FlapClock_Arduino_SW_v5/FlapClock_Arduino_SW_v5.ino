/*  Flap-Clock 
 * 
 * Dr. Tõnis
 * Pfäffikon 2023
 * 
 */

// OTA libraries and parameters
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
  #include <ESPmDNS.h>
  #include <Update.h>

  WebServer server(80);
  
  // WiFi Login Data
    #include "credentials.h" // in the libraries folder add credentials/credentials.h with the ssid and pw
    // main wifi
      const char *ssid1     = my_ssid1;
      const char *password1 = my_password1;
    // backup network
      const char *ssid2     = my_ssid2;
      const char *password2 = my_password2;  
    // host anem
      const char *host = "flapclock";
    // openweather
      const char *OPW_server = OpenWeatherFullLink;

// weather
  #include <ArduinoJson.h>
  #include <HTTPClient.h>
  String OPW_text;

// time libraries and functions
  #include <time.h>                   // time() ctime()
  #define MY_TZ "CET-1CEST,M3.5.0,M10.5.0/3"        // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
  
  unsigned long previousMillis = 0;        // will store last time Time was updated
  const long interval = 1000; // update time after every 1 second
  int old_hour = 0; // help parameter to save the last hour value
  int old_minute = 0; // help parameter to save the last minute value
  int old_second = 0; // help parameter to save the last second value
  
// Motor Pins
  const int CLK_Pin   = D1; // connected to SHCP
  const int LAT_Pin   = D0; // connected to STCP
  const int Data_Pin[5] = {D2, D3, D4, D5, D6}; // connected to DS (data in3) {H1(-H13-H1-H12), H2(-H12), H3 (pico), H14(-H12), H15(-H1-H13-H12)}
  const int EN_Pin = D7; // enable pin (pin to define if the homing is active or not)

// Motor movement parameters
  #define nr_steps 8 // 8 or 4 steps per movement
  #define nr_panels 3 // how many dial panels do we have - one panel has 12 digits
  
  #if nr_steps == 8
    // micro stepping
    const int step_states[8][4] = {{1, 0, 0, 1},
                                   {1, 0, 0, 0},
                                   {1, 0, 0, 0},
                                   {0, 1, 0, 0},
                                   {0, 1, 1, 0},
                                   {0, 1, 1, 0},
                                   {0, 0, 1, 1},
                                   {0, 0, 0, 1}};                               
  #else
    // full step
    const int step_states[4][4] = {{1, 0, 0, 1},
                                   {1, 1, 0, 0},
                                   {0, 1, 1, 0},
                                   {0, 0, 1, 1}};
  #endif
                       
  #define nr_steppers 12 // number of steppers connected in series to the shift registers (defined by the PCB)
  #define step_delay 2 // delay between steps, min 2. the higher values mean slower movement

  int active_rotation[nr_panels][nr_steppers] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // digits seen from the front (left to right)
                                                 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}; // move the marked motors so many digits

  int position_now[nr_panels][nr_steppers] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // digits seen from the front (left to right)
                                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}; // move the marked motors so many digits
  int position_new[nr_panels][nr_steppers] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // digits seen from the front (left to right)
                                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}; // move the marked motors so many digits
  int homing_digit[nr_panels][nr_steppers] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // digits seen from the front (left to right)
                                              {0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
                                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}; // home only the marked ones

  bool moves_left = 0; // help parameter, if ==1 then we need to make at least one move   

  int time_now[4] = {2, 3, 5, 4};
  
  // function states
  int clock_state = 0;
  int date_state = 0;
  int pattern_state = 0;
// Serial reading parameters
  String inputString = "";          // a String to hold incoming data
  bool stringComplete = false;      // whether the string is complete
  
// include other functions
  #include "Move_Basics.h"
  #include "Move_Active.h"
  #include "TestRuns.h" // not needed, only for testing purposes
  #include "Time.h"
  #include "Initialization.h"
//  #include "OTA_files.h"
  #include "SerialFunctions.h"
//  #include "Weather.h"

void setup() {
  // initiate all the pins
    setup_init();
  // connect to the WiFi
    setup_wifi();
//  // set up OTA
//    OTA_setup();
  // connect time server
    init_time_server(MY_TZ);  

//    get_Weather_data();
  // enable the outputs
    homing_disable(HIGH); // disable homing magnet detection
  // turn all Steppers Off
    AllSteppersOff();
  // perform homing
//    Homing();
}

void loop () {
    
//  // OTA server
//    server.handleClient(); // keep it here
    delay(1);
  // check if there are some new command in Serial
    SerialCommands();
  
  // update the current time
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time we checked time
        previousMillis = currentMillis;
      // show the current time
        get_time_now();
    }
  // check if we need to change the time
//    delay(10000);
    check_time_change_minute();
    
  // check if there are some differences between position_now and position_new
    Check_if_something_different();
    
  // check if we should move and make the required movement.
  // !!! during this time OTA server is not responding !!!
    ActiveStepperMovement();
}
