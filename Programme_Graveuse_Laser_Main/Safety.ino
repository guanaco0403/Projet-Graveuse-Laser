void ARU_INTERRUPT() { // SUR LE VRAI SYSTEME IL FAUDRA UTILISER LE ENABLE ET DISABLE OUTPUT POUR LA SECU
  SystemState = false;
  SetLaser(0);
  stepper_X.stop();
  stepper_Y.stop();
  currentScreen = 100;
}
