/*  Flap-Clock 
 * 
 * Dr. Tõnis
 * Pfäffikon 2023
 * 
 */
void setup_wifi() {
  // Connect to WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid1);

    WiFi.begin(ssid1, password1);
    int time_start = 0;
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      time_start += 1;
      if (time_start == 30) break; // break after 30 seconds
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi 1 connected");    
      Serial.print("IP: ");
      Serial.print(WiFi.localIP());
      Serial.println("");
    }
    else {
      Serial.println("try the second ssid");
      WiFi.begin(ssid2, password2);    
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("");
      Serial.println("WiFi 2 connected");    
      Serial.print("IP: ");
      Serial.print(WiFi.localIP());
      Serial.println("");  
    } 

}

void setup_init() {
  // initialize serial:
    // define the baud rate
      Serial.begin(115200);
    // wait a little
      delay(100);
    // write to the serial
      Serial.println("Serial Ready");
  
  // define output pins
    pinMode(LAT_Pin, OUTPUT);
    pinMode(CLK_Pin, OUTPUT);
    pinMode(EN_Pin, OUTPUT);

    for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
      // define all panel control pins at the same time
      pinMode(Data_Pin[panel_nr], OUTPUT);
    }
      
  // write to serial     
    Serial.println("All the input/output pins initiated");
}
