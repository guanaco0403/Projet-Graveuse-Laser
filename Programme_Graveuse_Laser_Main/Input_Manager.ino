/*
 * Gestion des INPUTS du joystick et logique du mode manuel et du menu paramètres
 */

const int deadzone = 50; // deadZone du joystick
const int MaxSpeed = 4000; // vitesse max que le joystick peut faire atteindre au moteur
int joystickXVal, joystickYVal; // pour retenir les valeurs des axes du joystick
bool joyPressOldState = false; // bool pour front montant du bouton

// Fonction qui retourne l'état actuel du joystick
JoyInput CheckInputs() {
  if (digitalRead(Joy_Switch) == LOW && joyPressOldState == false) {
    joyPressOldState = true;
    delay(100);
    return PRESS;
  } else if (digitalRead(Joy_Switch) == HIGH && joyPressOldState == true) {
    joyPressOldState = false;
    delay(100);
  }

  joystickXVal = analogRead(JoyX) - 512;
  joystickYVal = analogRead(JoyY) - 512;
  
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

void ManualMotorMove() {
  SetLaser(1);
  int joyState = digitalRead(Joy_Switch);
  while (joyState != LOW && SystemState == true) {
    joyState = digitalRead(Joy_Switch);
    
    joystickXVal = analogRead(JoyX) - 512;
    joystickYVal = (-analogRead(JoyY)) + 512;

    // Adjust X motor speed based on joystick input
    int speedX;
    if (abs(joystickXVal) > deadzone) {
      // Map the joystick value to a range for speed
      speedX = map(joystickXVal, -512, 512, -MaxSpeed, MaxSpeed);
      stepper_X.setSpeed(speedX);
    } else {
      // Stop the motor if within the deadzone
      stepper_X.setSpeed(0);
    }

    // Adjust Y motor speed based on joystick input
    int speedY;
    if (abs(joystickYVal) > deadzone) {
      // Map the joystick value to a range for speed
      speedY = map(joystickYVal, -512, 512, -MaxSpeed, MaxSpeed);
      stepper_Y.setSpeed(speedY);
    } else {
      // Stop the motor if within the deadzone
      stepper_Y.setSpeed(0);
    }

    // Run the motors at the set speeds
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
  SetLaser(0);
  Beep();
  stepper_X.stop();
  stepper_Y.stop();
  currentScreen = 2;
  DisplayCurrentScreen();
  /*if (digitalRead(Joy_Switch) == LOW) {
    Beep();
    SetLaser(0);
    stepper_X.stop();
    stepper_Y.stop();
    currentScreen = 0;
    }*/
}

void SettingsManager() {
  int subIndex = 0;
  JoyInput input = CheckInputs();
  while (input != PRESS && SystemState == true) {
    input = CheckInputs();

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
        saveEEPROM_Data();
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
        saveEEPROM_Data();
      }
      UpdateLCDSettingMenu(subIndex);
    }
  }
  Beep();
  currentScreen = 3;
  DisplayCurrentScreen();
}
