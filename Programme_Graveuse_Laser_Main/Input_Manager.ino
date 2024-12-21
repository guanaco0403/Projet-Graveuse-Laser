/*
   Gestion des INPUTS du joystick et logique du mode manuel et du menu paramètres
*/

const int deadzone = 50; // deadZone du joystick
const int MaxSpeed = 4000; // vitesse max que le joystick peut faire atteindre au moteur
int joystickXVal, joystickYVal; // pour retenir les valeurs des axes du joystick
bool joyPressOldState = false; // bool pour front montant du bouton

/* Fonction qui retourne l'état actuel du joystick */
JoyInput CheckInputs() {

  // Front montant pour le bouton du joystick
  if (digitalRead(Joy_Switch) == LOW && joyPressOldState == false) {
    joyPressOldState = true;
    delay(100);
    return PRESS;
  } else if (digitalRead(Joy_Switch) == HIGH && joyPressOldState == true) {
    joyPressOldState = false;
    delay(100);
  }

  // Enregistrement des valeurs analogiques des joysticks (1023 - 512 pour arriver a la moitié du joystick la ou il est au repos comme ca il va de -512 a 512)
  joystickXVal = analogRead(JoyX) - 512;
  joystickYVal = analogRead(JoyY) - 512;

  // Détection de la direction du joystick si on le tourne avec une valeur plus grande ou plus petite que 400
  if (joystickXVal > 400) {
    delay(350);
    return LEFT;
  } else if (joystickXVal < -400) {
    delay(350);
    return RIGHT;
  } else if (joystickYVal > 400) {
    delay(350);
    return DOWN;
  } else if (joystickYVal < -400) {
    delay(350);
    return UP;
  }
  return NONE; // si il y a rien alors on retourne rien
}

/* Fonction qui gère le mode manuel */
void ManualMotorMove() {
  SetLaser(1); // on met la puissance du laser a 1 la ou il est visible et innofensif

  int joyState = digitalRead(Joy_Switch);
  while (joyState != LOW && SystemState == true) { // On reste en mode manuel tant que on a pas appuyé sur le bouton du joystick
    joyState = digitalRead(Joy_Switch);

    // Enregistrement des valeurs analogiques des joysticks (1023 - 512 pour arriver a la moitié du joystick la ou il est au repos comme ca il va de -512 a 512)
    joystickXVal = analogRead(JoyX) - 512;
    joystickYVal = (-analogRead(JoyY)) + 512;

    // Ajustement de la vitesse du moteur X en fonction de lamplitude du joystick X
    int speedX;
    if (abs(joystickXVal) > deadzone) {
      // On map la valeur dans le range de vitesse max et du joystick X
      speedX = map(joystickXVal, -512, 512, -MaxSpeed, MaxSpeed);
      stepper_X.setSpeed(speedX);
    } else {
      // Stop du moteur X si il est dans la deadzone du joystick
      stepper_X.setSpeed(0);
    }

    // Ajustement de la vitesse du moteur Y en fonction de lamplitude du joystick Y
    int speedY;
    if (abs(joystickYVal) > deadzone) {
      // On map la valeur dans le range de vitesse max et du joystick Y
      speedY = map(joystickYVal, -512, 512, -MaxSpeed, MaxSpeed);
      stepper_Y.setSpeed(speedY);
    } else {
      // Stop du moteur Y si il est dans la deadzone du joystick
      stepper_Y.setSpeed(0);
    }

    // Marche des moteurs X et Y a la vitesse demandée
    if (speedX > 0) {
      if (stepper_X.currentPosition() < MAX_X && stepper_X.currentPosition() >= 0) {
        stepper_X.runSpeed();
      }
    } else {
      if (stepper_X.currentPosition() <= MAX_X && stepper_X.currentPosition() > 0) {
        stepper_X.runSpeed();
      }
    }

    if (speedY > 0) {
      if (stepper_Y.currentPosition() < MAX_Y && stepper_Y.currentPosition() >= 0) {
        stepper_Y.runSpeed();
      }
    } else {
      if (stepper_Y.currentPosition() <= MAX_Y && stepper_Y.currentPosition() > 0) {
        stepper_Y.runSpeed();
      }
    }
  }
  // Sortie du mode manuel

  SetLaser(0); // Désactivation du laser
  Beep();
  stepper_X.stop(); // Arrêt moteur X
  stepper_Y.stop(); // Arrêt moteur Y
  currentScreen = 2; // Remise du menu actuel a celui de selection du mode manuel
  DisplayCurrentScreen(); // On affiche l'écran actuel
}

/* Fonction qui gère les paramètres de la machine */
void SettingsManager() {
  int subIndex = 0; // Sous-Index pour la selection du paramètre a modifier
  JoyInput input = CheckInputs(); // On vérifie l'état du joystick
  while (input != PRESS && SystemState == true) { // On reste en mode paramètrage tant que l'on a pas appuyer sur le bouton du joystick
    input = CheckInputs();

    // On navigue dans les paramètres grace au joystick HAUT et BAS et on change la valeur du paramètre avec GAUCHE et DROITE
    if (input != NONE) {
      if (input == DOWN) {
        if (subIndex > 0) {
          subIndex--;
        }
        else {
          subIndex = 1;
        }
        Beep2();
      }
      else if (input == UP) {
        if (subIndex < 1) {
          subIndex++;
        }
        else {
          subIndex = 0;
        }
        Beep2();
      }
      else if (input == RIGHT) {
        if (subIndex == 0) {
          if (mainSpeed < 100) {
            mainSpeed ++;
            Beep2();
          }
        }
        else if (subIndex == 1) {
          if (mainLaserPower < 100) {
            mainLaserPower ++;
            Beep2();
          }
        }
        saveEEPROM_Data(); // Sauvegarde des paramètres dans l'EEPROM
      }
      else if (input == LEFT) {
        if (subIndex == 0) {
          if (mainSpeed > 0) {
            mainSpeed --;
            Beep2();
          }
        }
        else if (subIndex == 1) {
          if (mainLaserPower > 0) {
            mainLaserPower --;
            Beep2();
          }
        }
        saveEEPROM_Data(); // Sauvegarde des paramètres dans l'EEPROM
      }
      UpdateLCDSettingMenu(subIndex); // Mise a jour des valeurs des paramètres sur le LCD
    }
  }
  // Sortie du mode manuel
  
  Beep();
  currentScreen = 3; // Remise du menu actuel a celui de selection du mode de paramètrage
  DisplayCurrentScreen(); // On affiche l'écran actuel
}
