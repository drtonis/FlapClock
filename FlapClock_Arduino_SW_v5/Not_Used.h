/*
  Dr. Tõnis
  Pfäffikon 2023
  Switzerland
*/

  int micro_steps_matrix[nr_panels][nr_steppers] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // digits seen from the front (left to right)
                                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}; // move the marked motors so many digits
                                                    
void MoveSteppersFineDuning() {
  bool required_pin_state = 0; // a help parameter
  // here we move "micro"-steps, thus we need to have a help parameter
  int state_nr = 0;
  // help parameter to check if there are any moves left
  moves_left = 1; // help parameter, if ==1 then we need to make at least one move

  // just to be sure
  int total_steps = 4;
  if (nr_steps == 8) total_steps = 8; // how many micro steps do we have
  
  while (moves_left == 1) { // do it as long we have moves left
    moves_left = 0;
    // lets check if there are any movement left
      for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
        for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
          if (micro_steps_matrix[panel_nr][motor_nr] > 0) {
            moves_left = 1;
            break; // if at least one of the stepper should move we don't need to continue          
          }
      }}

    // start writing
      digitalWrite(LAT_Pin, LOW);
    // go through all the motors and move 1 micro step
      for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
        // go through all the stepper pins 
        for (int i = 0; i < 4; i++) {
          // set shift pin to "wait"
          digitalWrite(CLK_Pin, LOW);
          // all the panels are connected in parallel
          for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
            required_pin_state = 0;
            if (micro_steps_matrix[panel_nr][motor_nr] > 0) required_pin_state = step_states[state_nr % total_steps][i];
            digitalWrite(Data_Pin[panel_nr], required_pin_state);
          }
          // rising slope -> shifting data in the register
          digitalWrite(CLK_Pin, HIGH);
        }   
      }
    // write whole register to output --> move one step
      digitalWrite(LAT_Pin, HIGH); 
    // required delay between the steps
      delay(step_delay); // should be step_delay = 2 
    // adjust the movement matrix  
      for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
        for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
//          help_val = micro_steps_matrix[panel_nr][motor_nr];
          if (micro_steps_matrix[panel_nr][motor_nr] > 0) micro_steps_matrix[panel_nr][motor_nr] -= 1;      
      }}
      state_nr += 1;
  }
  AllSteppersOff(); // turn all steppers OFF 
}

void test_function (int nr_micro_steps) {
    for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
      for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
        micro_steps_matrix[panel_nr][motor_nr] = nr_micro_steps;
    }}   
  MoveSteppersFineDuning();
}

void SerialCommands_X() {
  // check the command when a newline arrives:
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\r');
    if (inputString.length() >= 2) {
//      Serial.println(inputString);
      if (inputString[0] == 'm') move_all_steppers_8(); // move all the digits one bit further - needed for fine duning and testing
  
      else if (inputString[0] == 'h') Homing(); // perform homing
      
      else if (inputString[0] == 'c') { // change clock state
        if      (inputString[1] == '0') clock_state = 0; // State 0 - no clock
        else if (inputString[1] == '1') clock_state = 1; // State 1 - show clock - only hour and minute
        else if (inputString[1] == '2') clock_state = 2; // State 2 - show clock - State 1 +  seconds
        Serial.print("new Clock State = ");
        Serial.println(clock_state);
      }
  
//      else if (inputString[0] == 'd') { // change clock state
//        if      (inputString[1] == '0') date_state = 0; // State 0 - no date
//        else if (inputString[1] == '1') date_state = 1; // State 1 - show date
//        else if (inputString[1] == '2') date_state = 2; // State 2 - show date + weekday
//        Serial.print("new Date State = ");
//        Serial.println(date_state);
//      }
//  
//      else if (inputString[0] == 'p') { // change pattern state
//        if      (inputString[1] == '0') pattern_state = 0; // State 0 - no pattern = white
//        else if (inputString[1] == '1') pattern_state = 1; // State 1 - black
//        else if (inputString[1] == '2') pattern_state = 2; // State 2 - zigzag 1
//        else if (inputString[1] == '3') pattern_state = 3; // State 3 - zigzag 2
//        else if (inputString[1] == '4') pattern_state = 4; // State 4 - zigzag 3
//        else if (inputString[1] == '5') pattern_state = 5; // State 5 - random
//        
//        Serial.print("new Pattern State = ");
//        Serial.println(pattern_state);
//      }
//
//      else if (inputString[0] == 't') { // only for testing and fine duning
//        String number = "";
//        // get the number
//        for (int i = 1; i <= inputString.length(); i++) {
//          if (isDigit(inputString[i])) number += inputString[i];
//        } 
//        
//        Serial.print("move all steppers ");
//        Serial.print(number);
//        Serial.println(" micro steps");
//        test_function(number.toInt());
//      }

      else if (inputString[0] == 'n') { // show the NOW position
        if (inputString[1] == 'o') {
          // show the positions
          for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
            Serial.print("Row Nr. = ");
            Serial.print(panel_nr);
            Serial.print(" Positions = ");
            for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
              Serial.print(" ");
              Serial.print(position_now[panel_nr][motor_nr]);
              Serial.print(" ");
            }
            Serial.println("");  
          } 
        }
      }
      
      // clear the string:
      inputString = "";
    }
  }
}

void Homing_Full() {
  Serial.println("homing started");
  homing_disable(LOW); // enable homing magnet detection
  // one rotation = 4096 / nr_steps [steps]
  // to be sure that we have made more than one full cirlce 128 extra steps would be added
  int extra_steps = 128; // must be 2^x (2 power of value)
  for (int pulse = 0; pulse < 4096 / nr_steps + extra_steps; pulse++) {
    MoveAllSteppers();
  }
  // to avoid stopping at the wrong spot lets do another round
  homing_disable(HIGH); // disable homing magnet detection
  for (int pulse = 0; pulse < extra_steps; pulse++) {
    MoveAllSteppers();
  }
  homing_disable(LOW); // enable homing magnet detection
  for (int pulse = 0; pulse < 4096 / nr_steps + extra_steps; pulse++) {
    MoveAllSteppers();
  }
  homing_disable(HIGH); // disable homing magnet detection
  // some digits are blau and others are green. to make sure all are blue we move the green digits one digit further
    // since I'm not sure how to do it better I use here a workaround
    set_position_new(3);
    position_new[0][1] = 2; // digits seen from the front (left to right)
    position_new[0][2] = 6; // digits seen from the front (left to right)
    position_new[0][3] = 2;
    position_new[0][4] = 2;
    position_new[0][5] = 2;
    position_new[0][6] = 6;
    position_new[0][7] = 5;
    position_new[0][8] = 4;
    position_new[0][9] = 0; // digits seen from the front (left to right)
    position_new[0][10] = 4; // digits seen from the front (left to right)
    position_new[0][11] = 2;
//
    position_new[1][0] = 4;
    position_new[1][1] = 2;
    position_new[1][5] = 2;
    position_new[1][6] = 6;
    position_new[1][8] = 5;
    position_new[1][10] = 7;
    position_new[1][11] = 0;
//    
    position_new[2][0] = 2;
    position_new[2][2] = 2;
    position_new[2][4] = 0;
    position_new[2][7] = 1;
    position_new[2][8] = 4;
    position_new[2][9] = 5;
    position_new[2][10] = 4;
    position_new[2][11] = 2;

  // set the current position
    zero_position_now();
  // check if we should move
    Check_if_something_different();
  // if there are some moves to be made, then do it
    ActiveStepperMovement();
  // set the current position
    zero_position_now();
  // done
    Serial.println("homing done");

//    Serial.println("");
//    Serial.println("NEW");
//    for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
//      Serial.print("Row Nr. = ");
//      Serial.print(panel_nr);
//      Serial.print(" Positions = ");
//      for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
//        Serial.print(" ");
//        Serial.print(position_new[panel_nr][motor_nr]);
//        Serial.print(" ");
//      }
//      Serial.println("");  
//    }
//    Serial.println("");
//    Serial.println("NOW");
//    for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
//      Serial.print("Row Nr. = ");
//      Serial.print(panel_nr);
//      Serial.print(" Positions = ");
//      for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
//        Serial.print(" ");
//        Serial.print(position_now[panel_nr][motor_nr]);
//        Serial.print(" ");
//      }
//      Serial.println("");  
//    }
}


void check_time_change_second() {
  // we do something if second changes
  if (old_second != timeinfo.tm_sec) {
    // second has changed and we need to do something
    
    // save the current minute value
      old_second = timeinfo.tm_sec;
  }
}


void check_time_change_hour() {
  // we do something if hour changes
  if (old_hour != timeinfo.tm_hour) {
    // hour has changed and we need to adjust the screen

    // save the current minute value
      old_minute = timeinfo.tm_hour;
  }
}


void showTime_Serial_now() {
  // show the current time in serial monitor
    Serial.print("year: ");
    Serial.print(timeinfo.tm_year + 1900);  // years since 1900
    Serial.print(" month: ");
    Serial.print(timeinfo.tm_mon + 1);      // January = 0 (!)
    Serial.print(" day: ");
    Serial.print(timeinfo.tm_mday);         // day of month
    Serial.print(" hour: ");
    Serial.print(timeinfo.tm_hour);         // hours since midnight  0-23
    Serial.print(" min: ");
    Serial.print(timeinfo.tm_min);          // minutes after the hour  0-59
    Serial.print(" sec: ");
    Serial.print(timeinfo.tm_sec);          // seconds after the minute  0-61*
    Serial.print(" wday ");
    Serial.print(timeinfo.tm_wday);         // days since Sunday 0-6
    Serial.println();  
}

void adjust_clock_minute_base() {
  // minute has changed and we need to adjust the screen
  // show time on 2nd row: hour 3-4 / : digit 5 / minute 6-7

  // since the bit positions are not the same as the number we need a help matrix
  int help_nr = 0;
  help_nr = int(timeinfo.tm_hour / 10); // hour 
  for (int i = 0; i < 10; i++) {
    // find the matching number
    if (number_bits[i] == help_nr) {
      position_new[1][3] = help_nr;
      break; // no need to contine 
    }
  }  

  help_nr = int(timeinfo.tm_hour % 10); // hour 
  for (int i = 0; i < 10; i++) {
    // find the matching number
    if (number_bits[i] == help_nr) {
      position_new[1][4] = help_nr;
      break; // no need to contine 
    }
  } 

  help_nr = int(timeinfo.tm_min / 10); // minute 
  for (int i = 0; i < 10; i++) {
    // find the matching number
    if (number_bits[i] == help_nr) {
      position_new[1][6] = help_nr;
      break; // no need to contine 
    }
  } 

  help_nr = int(timeinfo.tm_min % 10); // minute 
  for (int i = 0; i < 10; i++) {
    // find the matching number
    if (number_bits[i] == help_nr) {
      position_new[1][7] = help_nr;
      break; // no need to contine 
    }
  }  
}
