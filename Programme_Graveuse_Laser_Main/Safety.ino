void ARU_INTERRUPT() { // SUR LE VRAI SYSTEME IL FAUDRA UTILISER LE ENABLE ET DISABLE OUTPUT POUR LA SECU
  SystemState = false;
  SetLaser(0);
  DisableStepperMotorPower();
  stepper_X.stop();
  stepper_Y.stop();
  currentScreen = 100;
  oldScreen = -1;
  digitalWrite(led_ARU, HIGH);
}

void CheckARU_ACK() {
  JoyInput input = CheckInputs();
  if (input == PRESS && digitalRead(ARU) == HIGH) {
    SystemState = true;
    digitalWrite(led_ARU, LOW);
    EnableStepperMotorPower();
    AutoHome();
    currentScreen = 0;
  }
}
