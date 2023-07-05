/*  Flap-Clock 
 * 
 * Dr. Tõnis
 * Pfäffikon 2023
 * 
 * here are the needed functions for a controlled individual movements
 */

void Check_if_something_different() {
  for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      // do something only if new is different from now
      if (position_new[panel_nr][motor_nr] !=  position_now[panel_nr][motor_nr]) {
        // calculate how many moves are needed for the new position
        active_rotation[panel_nr][motor_nr] = position_new[panel_nr][motor_nr] - position_now[panel_nr][motor_nr];
        // if one needs to rotate over the 0 position
        if (active_rotation[panel_nr][motor_nr] < 0) active_rotation[panel_nr][motor_nr] += 64; // there are 64 bits!
        // adjust the now matrix
        position_now[panel_nr][motor_nr] = position_new[panel_nr][motor_nr];
      }
    }  
  } 
}

int Check_Remaining_Moves() {
  // check if any of the stepper should be moved,
  // if yes then return 1 otherwise 0
  int result = 0;
  for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      // adjust the active rotation matrix
      if (active_rotation[panel_nr][motor_nr] > 0) {
        result = 1;
        break; // if at least one of the stepper should move we don't need to continue
      }
    }  
  }
  return result;
}

void MoveSteppers() {
  bool required_pin_state = 0; // a help parameter
  // move only these stepper motors that are required to move
  for (int state = 0; state < nr_steps; state++) {
    // go through all the micro steps
    // start writing
    digitalWrite(LAT_Pin, LOW);

    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      // go through all the stepper pins 
      for (int i = 0; i < 4; i++) {
        // set shift pin to "wait"
        digitalWrite(CLK_Pin, LOW);
        // move only if the motor should be moved
        for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
          required_pin_state = 0;
          if (active_rotation[panel_nr][motor_nr] > 0) required_pin_state = step_states[state][i];
          digitalWrite(Data_Pin[panel_nr], required_pin_state);
        }
        // rising slope -> shifting data in the register
        digitalWrite(CLK_Pin, HIGH);
      }   
    }

    // write whole register to output
    digitalWrite(LAT_Pin, HIGH); 
    // required delay between the steps
    delay(step_delay); // should be step_delay = 2 
  }
}

void AdjustRemainingMoves() {
  // adjust the active_rotation matrix
  // after every movement reduce the active moves by 1
  for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      // adjust the active rotation matrix
      if (active_rotation[panel_nr][motor_nr] > 0) active_rotation[panel_nr][motor_nr] -= 1;
    }  
  }
}

void ActiveStepperMovement() {
  moves_left = 0;
  // help parameter to check if there are any moves left
  moves_left = Check_Remaining_Moves(); // help parameter, if ==1 then we need to make at least one move
  while (moves_left == 1) { // do it as long we have moves left
    // move steppers only if there are any movements left
    MoveSteppers();
    AdjustRemainingMoves(); 
    moves_left = Check_Remaining_Moves(); // check if any of the stepper should be moved 
  }
  moves_left = 0;
  AllSteppersOff(); // turn all steppers OFF
}

void Set_White() {
  Serial.println("Set all white");
  // home all
    Homing();
  // some digits are blau and others are green. to make sure all are white we move the digits until they are white
  // since I'm not sure how to do it better I use here a workaround
  // move all digits xx bits further
    set_position_new(12);
  // since there are some that needs more/less movement we adjust the manually
    // upper row
    position_new[0][2] += 2; // digits seen from the front (left to right)
    position_new[0][7] += 2; // digits seen from the front (left to right)
    position_new[0][9] -= 4; // digits seen from the front (left to right)
    // mid row
    position_new[1][6] += 2; // digits seen from the front (left to right)
    position_new[1][8] += 2; // digits seen from the front (left to right)
    position_new[1][10] += 2; // digits seen from the front (left to right)
    // bottom row  
    position_new[2][4] -= 2;
    position_new[2][7] -= 2;
    position_new[2][9] += 2;
  // check that there are no negative values
    check_position_new();
}

void Set_drtonis() {
  // home all
    Homing();
  // some digits are blau and others are green. to make sure all are white we move the digits until they are white
  // since I'm not sure how to do it better I use here a workaround
  // move all digits xx bits further
    set_position_new(12);
  // since there are some that needs more/less movement we adjust the manually
    // upper row
    position_new[0][2] += 2; // digits seen from the front (left to right)
    position_new[0][7] += 2; // digits seen from the front (left to right)
    position_new[0][9] -= 4; // digits seen from the front (left to right)

    position_new[0][2] += 54 * 8 - 4; // digits seen from the front (left to right)
    position_new[0][3] += 40 * 8 - 12; // digits seen from the front (left to right)
    position_new[0][4] += 15 * 8 - 12; // digits seen from the front (left to right)
    
    position_new[0][6] += 38 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][7] += 43 * 8 - 4; // digits seen from the front (left to right)
    position_new[0][8] += 44 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][9] += 49 * 8 - 10; // digits seen from the front (left to right)
    position_new[0][10] += 39 * 8 - 8; // digits seen from the front (left to right)

    // mid row
    position_new[1][6] += 2; // digits seen from the front (left to right)
    position_new[1][8] += 2; // digits seen from the front (left to right)
    position_new[1][10] += 2; // digits seen from the front (left to right)
    
//    // mid row
//    position_new[1][2] += 54 * 8 - 4; // digits seen from the front (left to right)
//    position_new[1][3] += 40 * 8 - 4; // digits seen from the front (left to right)
//    position_new[1][4] += 15 * 8 - 4; // digits seen from the front (left to right)
//    
//    position_new[1][6] += 38 * 8 - 4; // digits seen from the front (left to right)
//    position_new[1][7] += 43 * 8 - 8; // digits seen from the front (left to right)
//    position_new[1][8] += 44 * 8 - 8; // digits seen from the front (left to right)
//    position_new[1][9] += 49 * 8 - 8; // digits seen from the front (left to right)
//    position_new[1][10] += 39 * 8 - 2; // digits seen from the front (left to right)
//    
//    // bottom row  
//    position_new[2][2] += 54 * 8 - 4; // digits seen from the front (left to right)
//    position_new[2][3] += 40 * 8 - 8; // digits seen from the front (left to right)
//    position_new[2][4] += 15 * 8 - 4; // digits seen from the front (left to right)
//    
//    position_new[2][6] += 38 * 8 - 4; // digits seen from the front (left to right)
//    position_new[2][7] += 43 * 8 - 8; // digits seen from the front (left to right)
//    position_new[2][8] += 44 * 8 - 4; // digits seen from the front (left to right)
//    position_new[2][9] += 49 * 8 - 6; // digits seen from the front (left to right)
//    position_new[2][10] += 39 * 8 - 2; // digits seen from the front (left to right)
  // check that there are no negative values
    check_position_new();
}

void Set_Seeed() {
  // home all
    Homing();
  // some digits are blau and others are green. to make sure all are white we move the digits until they are white
  // since I'm not sure how to do it better I use here a workaround
  // move all digits xx bits further
    set_position_new(12);
  // since there are some that needs more/less movement we adjust the manually
    // upper row
    position_new[0][2] += 38 * 8 - 4; // digits seen from the front (left to right)
    position_new[0][3] += 50 * 8 - 12; // digits seen from the front (left to right)
    position_new[0][4] += 57 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][5] += 44 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][6] += 47 * 8 - 4; // digits seen from the front (left to right)
    position_new[0][7] += 39 * 8 - 8; // digits seen from the front (left to right)

    position_new[0][9] -= 4; // digits seen from the front (left to right)
    
    // mid row
    position_new[1][0] += 39 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][1] += 53 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][2] += 53 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][3] += 53 * 8 - 8; // digits seen from the front (left to right)
    position_new[1][4] += 54 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][5] += 39 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][6] += 38 * 8 - 0; // digits seen from the front (left to right)
    position_new[1][7] += 37 * 8 - 8; // digits seen from the front (left to right)
    position_new[1][8] += 54 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][9] += 49 * 8 - 8; // digits seen from the front (left to right)
    position_new[1][10] += 43 * 8 - 2; // digits seen from the front (left to right)
    
    // bottom row  
    position_new[2][4] -= 2;
    position_new[2][7] -= 2;
    position_new[2][9] += 2;
  // check that there are no negative values
    check_position_new();
}

void Set_poweredby() {
  // home all
    Homing();
  // some digits are blau and others are green. to make sure all are white we move the digits until they are white
  // since I'm not sure how to do it better I use here a workaround
  // move all digits xx bits further
    set_position_new(12);
  // since there are some that needs more/less movement we adjust the manually
    // upper row
    position_new[0][2] += 42 * 8 - 4; // digits seen from the front (left to right)
    position_new[0][3] += 43 * 8 - 4; // digits seen from the front (left to right)
    position_new[0][4] += 35 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][5] += 53 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][6] += 40 * 8 - 0; // digits seen from the front (left to right)
    position_new[0][7] += 53 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][8] += 54 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][9] -= 4; // digits seen from the front (left to right)
    position_new[0][10] += 56 * 8 - 8; // digits seen from the front (left to right)
    position_new[0][11] += 33 * 8 - 8; // digits seen from the front (left to right)
    
    // mid row
    position_new[1][0] += 34 * 8 - 8; // digits seen from the front (left to right)
    position_new[1][1] += 49 * 8 - 8; // digits seen from the front (left to right)
    position_new[1][2] += 57 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][3] += 43 * 8 - 4; // digits seen from the front (left to right)

    position_new[1][5] += 53 * 8 - 8; // digits seen from the front (left to right)
    position_new[1][6] += 39 * 8 - 0; // digits seen from the front (left to right)
    position_new[1][7] += 42 * 8 - 8; // digits seen from the front (left to right)
    position_new[1][8] +=  9 * 8 - 4; // digits seen from the front (left to right)
    position_new[1][9] += 10 * 8 - 8; // digits seen from the front (left to right)

    position_new[1][10] += 2; // digits seen from the front (left to right)
    // bottom row  
    position_new[2][4] -= 2;
    position_new[2][7] -= 2;
    position_new[2][9] += 2;
  // check that there are no negative values
    check_position_new();
}

void Set_Time() {
  HomingSelected();
  // move all digits xx bits further
    set_position_new(0);
  // since there are some that needs more/less movement we adjust the manually  
    int row = 1;
    int pos = 2;
    // set tenth of hour
    if      (time_now[0] == 0) position_new[row][pos + 0] += 12 + 0 * 8 + 8; // digits seen from the front (left to right)
    else if (time_now[0] == 1) position_new[row][pos + 0] += 12 + 9 * 8 + 8; // digits seen from the front (left to right)
    else if (time_now[0] == 2) position_new[row][pos + 0] += 12 + 8 * 8 + 8; // digits seen from the front (left to right)
  
    // set hour
    if      (time_now[1] == 0) position_new[row][pos + 1] += 12 + 0 * 8 + 8; // digits seen from the front (left to right)
    else if (time_now[1] == 1) position_new[row][pos + 1] += 12 + 9 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 2) position_new[row][pos + 1] += 12 + 8 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 3) position_new[row][pos + 1] += 12 + 7 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 4) position_new[row][pos + 1] += 12 + 6 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 5) position_new[row][pos + 1] += 12 + 5 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 6) position_new[row][pos + 1] += 12 + 4 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 7) position_new[row][pos + 1] += 12 + 3 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 8) position_new[row][pos + 1] += 12 + 2 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[1] == 9) position_new[row][pos + 1] += 12 + 1 * 8 + 12; // digits seen from the front (left to right)
  
    // set tenth of minute
    if      (time_now[2] == 0) position_new[row][pos + 3] += 12 + 0 * 8 + 6; // digits seen from the front (left to right)
    else if (time_now[2] == 1) position_new[row][pos + 3] += 12 + 9 * 8 + 8; // digits seen from the front (left to right)
    else if (time_now[2] == 2) position_new[row][pos + 3] += 12 + 8 * 8 + 4; // digits seen from the front (left to right)
    else if (time_now[2] == 3) position_new[row][pos + 3] += 12 + 7 * 8 + 4; // digits seen from the front (left to right)
    else if (time_now[2] == 4) position_new[row][pos + 3] += 12 + 6 * 8 + 4; // digits seen from the front (left to right)
    else if (time_now[2] == 5) position_new[row][pos + 3] += 12 + 5 * 8 + 6; // digits seen from the front (left to right)
  
    // set minute
    if      (time_now[3] == 0) position_new[row][pos + 4] += 12 + 0 * 8 + 12; // digits seen from the front (left to right)
    else if (time_now[3] == 1) position_new[row][pos + 4] += 12 + 9 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 2) position_new[row][pos + 4] += 12 + 8 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 3) position_new[row][pos + 4] += 12 + 7 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 4) position_new[row][pos + 4] += 12 + 6 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 5) position_new[row][pos + 4] += 12 + 5 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 6) position_new[row][pos + 4] += 12 + 4 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 7) position_new[row][pos + 4] += 12 + 3 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 8) position_new[row][pos + 4] += 12 + 2 * 8 + 14; // digits seen from the front (left to right)
    else if (time_now[3] == 9) position_new[row][pos + 4] += 12 + 1 * 8 + 14; // digits seen from the front (left to right)
  // check that there are no negative values
    check_position_new();

    time_now[3] += 1;
    if (time_now[3] > 9) {
      time_now[3] = 0;
      time_now[2] += 1;
    }
    if (time_now[2] > 5) {
      time_now[2] = 0;
      time_now[1] += 1;
    }
    if (time_now[1] > 9) {
      time_now[1] = 0;
      time_now[0] += 1;
    }
    
    if (time_now[0] > 2) time_now[0] = 0;

//    time_now[0] += 1;
//    if (time_now[0] > 2) time_now[0] = 0;
//    time_now[1] += 1;
//    if (time_now[1] > 9) time_now[1] = 0;
//    time_now[2] += 1;
//    if (time_now[2] > 5) time_now[2] = 0;
//    time_now[3] += 1;
//    if (time_now[3] > 9) time_now[3] = 0;
    for (int i = 0; i < 4; i++) { 
      Serial.print(time_now[i]);
      Serial.print(" ");
    }
    Serial.println("");
}
