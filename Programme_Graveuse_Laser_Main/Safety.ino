/* Fonction interrupt pour l'arrêt d'urgence */
void ARU_INTERRUPT() {
  SystemState = false; // Mise de l'état du système a désactivé
  SetLaser(0); // Désactivation du laser
  DisableStepperMotorPower(); // Désactivation des drivers
  stepper_X.stop(); // Stop du moteur X
  stepper_Y.stop(); // Stop des moteurs Y
  currentScreen = 100; // Mise du menu actuel au menu d'arrêt d'urgence
  oldScreen = -1;
  digitalWrite(led_ARU, HIGH); // Allumage led de défaut
}

/* Fonction pour vérifier l'aquitement de l'Arrêt d'urgence */
void CheckARU_ACK() {
  JoyInput input = CheckInputs();
  if (input == PRESS && digitalRead(ARU) == HIGH) { // Si on appui sur le bouton du joystick alors on aquitte
    SystemState = true; // Mise l'état du système a activé
    digitalWrite(led_ARU, LOW); // Désactivation de la led de défaut
    EnableStepperMotorPower(); // Activation des drivers
    AutoHome(); // Auto Homing
    currentScreen = 0; // Mise au menu principal
  }
}
