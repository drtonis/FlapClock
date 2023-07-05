/*  Flap-Clock 
 * 
 * Dr. Tõnis
 * Pfäffikon 2023
 * 
 * here are all the basic movements listed
 * all the digits are moved at the same time
 */


/*
  bit nr - color/letter
  0  - blue
  1  - white 
  2  - 0 (zero)
  3  - 9
  4  - 8
  5  - 7
  6  - 6
  7  - 5
  8  + 4
  9  - 3
  10 - 2
  11 - 1
  12 - /
  13 - deg symbol
  14 - ?
  15 - .
  16 - ,
  17 - %
  18 - "
  19 - ' apostrophe 
  20 - :
  21 - ;
  22 - =
  23 - +
  24 - -
  25 - )
  26 - (
  27 - &
  28 - $
  29 - #
  30 - @
  31 - !
  32 - Z
  33 - Y
  34 - X
  35 - W
  36 - V
  37 - U
  38 - T
  39 - S
  40 - R
  41 - Q
  42 - P
  43 - O
  44 - N
  45 - M
  46 - L
  47 - K
  48 - J
  49 - I
  50 - H
  51 - G
  52 - F
  53 - E
  54 - D
  55 - C
  56 - B
  57 - A
  58 - black
  59 - purple
  60 - red
  61 - orange
  62 - yellow
  63 - green
*/
const int number_bits[10] = {2, 11, 10, 9, 8, 7, 6, 5, 4, 3}; // 0 1 2 3 4 5 6 7 8 9
void homing_disable(int state) {
  // if EN_pin == LOW we detect homing magnet
  // in normal operation EN_pin == HIGH
  
  if (state) {
    digitalWrite(EN_Pin, HIGH); // negative logic!
    Serial.println("homing detector OFF");
  }
  else {
    digitalWrite(EN_Pin, LOW); // negative logic!
    Serial.println("homing detector ON");
  }
}

void AllSteppersOff() {
  // turn all the shift register outputs LOW
  // start writing
  digitalWrite(LAT_Pin, LOW);
  // go through all the stepper pins
  for (int i = 0; i < nr_steppers * 4; i++) {
    // set shift pin to "wait"
    digitalWrite(CLK_Pin, LOW);
    // writing to data pin
    for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
      // move steppers on all the panels at the same time
      digitalWrite(Data_Pin[panel_nr], LOW); // output pin state
    }
    // rising slope -> shifting data in the register
    digitalWrite(CLK_Pin, HIGH);
  }
  // write whole register to output
  digitalWrite(LAT_Pin, HIGH);  
}

void AllPinsON() {
  // turn all the shift register outputs LOW
  
  // start writing
  digitalWrite(LAT_Pin, LOW);
  // go through all the stepper pins
  for (int i = 0; i < nr_steppers * 4; i++) {
    // set shift pin to "wait"
    digitalWrite(CLK_Pin, LOW);
    // writing to data pin
    for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
      // move steppers on all the panels at the same time
      digitalWrite(Data_Pin[panel_nr], HIGH); // output pin state
    }
    // rising slope -> shifting data in the register
    digitalWrite(CLK_Pin, HIGH);
  }
  // write whole register to output
  digitalWrite(LAT_Pin, HIGH);  
}

void zero_position_now() {
  for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      position_now[panel_nr][motor_nr] = 0;  
    }
  }
}

void zero_position_new() {
  for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      position_new[panel_nr][motor_nr] = 0;  
    }
  }
}

void set_position_new(int nr) {
  for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      position_new[panel_nr][motor_nr] = nr;  
    }
  }
}

void check_position_new() {
  for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      if (position_new[panel_nr][motor_nr] < 0) position_new[panel_nr][motor_nr] = 0;  
    }
  }
}

void MoveAllSteppers() {
  // move all the steppers at once "nr_steps" microsteps
  
  for (int state = 0; state < nr_steps; state++) {
    // start writing
    digitalWrite(LAT_Pin, LOW);
    // go through all the stepper pins
    for (int i = 0; i < nr_steppers * 4; i++) {
      // set shift pin to "wait"
      digitalWrite(CLK_Pin, LOW);
      // writing to data pin
      for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
        // move steppers on all the panels at the same time
        digitalWrite(Data_Pin[panel_nr], step_states[state][i % 4]); // output pin state
      }
      // rising slope -> shifting data in the register
      digitalWrite(CLK_Pin, HIGH);
    }
    // write whole register to output
    digitalWrite(LAT_Pin, HIGH); 
    // required delay between the steps
    delay(step_delay); // should be step_delay = 2 
  } 
}

void Homing() {
  // inform that the homing is started
    Serial.println("homing started");
  // 
    homing_disable(LOW); // enable homing magnet detection
  // one rotation = 4096 / nr_steps [steps]
  // to be sure that we have made more than one full cirlce 128 extra steps would be added
    int extra_steps = 128; // must be 2^x (2 power of value)
    for (int pulse = 0; pulse < 4096 / nr_steps + extra_steps; pulse++) {
      MoveAllSteppers();
    }
  // to avoid stopping at the wrong spot lets do another round
    homing_disable(HIGH); // disable homing magnet detection
  // set the current position
    zero_position_now();
  // done
    Serial.println("homing done");
}

void MoveSelectedSteppers() {
  // move all the steppers at once "nr_steps" microsteps
  
  for (int state = 0; state < nr_steps; state++) {
    // start writing
    digitalWrite(LAT_Pin, LOW);
    for (int motor_nr = 0; motor_nr < nr_steppers; motor_nr++) {
      // go through all the stepper pins 
      for (int i = 0; i < 4; i++) {
        // set shift pin to "wait"
        digitalWrite(CLK_Pin, LOW);
        // move only if the motor should be moved
        for (int panel_nr = 0; panel_nr < nr_panels; panel_nr++) {
          if(homing_digit[panel_nr][motor_nr] > 0) digitalWrite(Data_Pin[panel_nr], step_states[state][i % 4]); // output pin state
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

void HomingSelected() {
  // inform that the homing is started
    Serial.println("HomingSelected started");
  // 
    homing_disable(LOW); // enable homing magnet detection
  // one rotation = 4096 / nr_steps [steps]
  // to be sure that we have made more than one full cirlce 128 extra steps would be added
    int extra_steps = 0; // must be 2^x (2 power of value)
    for (int pulse = 0; pulse < 4096 / nr_steps + extra_steps; pulse++) {
      MoveSelectedSteppers();
    }
  // to avoid stopping at the wrong spot lets do another round
    homing_disable(HIGH); // disable homing magnet detection
  // set the current position
    zero_position_now();
  // done
    Serial.println("homing done");
}
