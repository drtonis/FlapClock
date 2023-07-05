/*  Flap-Clock 
 * 
 * Dr. Tõnis
 * Pfäffikon 2023
*/

void SerialCommands() {
  // check the command when a newline arrives:
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\r');
    if (inputString.length() >= 2) {
//      Serial.println(inputString);
      if (inputString[0] == 'm') move_all_steppers_8(); // move all the digits one bit further - needed for fine duning and testing
  
      else if (inputString[0] == 'h') Homing(); // perform homing
      else if (inputString[0] == 'q') HomingSelected(); // perform HomingSelected

      else if (inputString[0] == 'w') Set_White(); // all white

      else if (inputString[0] == 'd') Set_drtonis(); // 

      else if (inputString[0] == 's') Set_Seeed(); // 
      else if (inputString[0] == 'p') Set_poweredby(); //
      else if (inputString[0] == 't') Set_Time(); //
      
//      else if (inputString[0] == 'c') { // change clock state
//        if      (inputString[1] == '0') clock_state = 0; // State 0 - no clock
//        else if (inputString[1] == '1') clock_state = 1; // State 1 - show clock - only hour and minute
//        else if (inputString[1] == '2') clock_state = 2; // State 2 - show clock - State 1 +  seconds
//        Serial.print("new Clock State = ");
//        Serial.println(clock_state);
//      }

      
      // clear the string:
      inputString = "";
    }
  }
}
