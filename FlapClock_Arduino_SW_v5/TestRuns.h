/*  Flap-Clock 
 * 
 * Dr. Tõnis
 * Pfäffikon 2023
 * 
 * here are some test funcitons to check if everything is working
 */

void ULN_Blink(){
  // blink all the LEDs at the same time
  digitalWrite(EN_Pin, LOW);
  AllPinsON();
  delay(500);
  digitalWrite(EN_Pin, HIGH);
  AllSteppersOff();
  delay(500);
}

void move_all_steppers_8() {
  for (int j = 0; j < 8; j++) {
    MoveAllSteppers();
  }
  AllSteppersOff();
}
