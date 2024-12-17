int subIndex = 0;

void DisplayCurrentScreen() {
  int minutes = 0;
  int seconds = 0;
  char timeBuffer[6];

  switch (currentScreen) {
    case 0:
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Laser Ready");
      lcd.setCursor(0, 2);
      lcd.print("[OK] Start Engraving");
      lcd.setCursor(0, 3);
      lcd.print("<-");
      lcd.setCursor(18, 3);
      lcd.print("->");
      break;
    case 1:
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("Statistics");
      lcd.setCursor(0, 1);
      lcd.print("Engr Count: ");
      lcd.print(totalEngravingCount);
      lcd.setCursor(0, 2);
      lcd.print("Engr Time: ");
      lcd.print(SecsToTimeString(totalEngravingTime));
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Manual Control");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(6, 1);
      lcd.print("Settings");
      break;
    case 10:
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Settings Menu");
      lcd.setCursor(0, 1);
      lcd.print("Speed:");
      lcd.setCursor(0, 2);
      lcd.print("Laser Power:");
      UpdateLCDSettingMenu();
      SettingsManager();
      break;
    case 40:
      lcd.clear();
      lcd.setCursor(6, 1);
      lcd.print("Manual Mode");
      break;
    case 100:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("====================");
      lcd.setCursor(2, 1);
      lcd.print("[EMERGENCY STOP]");
      lcd.setCursor(0, 2);
      lcd.print("====================");
      lcd.setCursor(2, 3);
      lcd.print("[PRESS] to clear");
      break;
  }
}

void PrintProgressBar(int percent, int line) {
  lcd.setCursor(0, 1);
  lcd.print("[");
  int convert = map(percent, 0, 100, 0, 18);
  int i = 0;
  for (i; i < convert; i++) {
    lcd.print("=");
  }
  for (i; i < 18; i++) {
    lcd.print(" ");
  }
  lcd.print("]");
}

void SettingsManager() {
  JoyInput input = CheckInputs();
  while (input != PRESS) {
    input = CheckInputs();
    Serial.println(input);

    if (input != NONE) {
      if (input == DOWN) {
        if (subIndex > 0) {
          subIndex--;
        }
        else {
          subIndex = 1;
        }
      }
      else if (input == UP) {
        if (subIndex < 1) {
          subIndex++;
        }
        else {
          subIndex = 0;
        }
      }
      else if (input == RIGHT) {
        if (subIndex == 0) {
          if (mainSpeed < 100) {
            mainSpeed ++;
          }
        }
        else if (subIndex == 1) {
          if (mainLaserPower < 100) {
            mainLaserPower ++;
          }
        }
        saveEEPROM_Data();
      }
      else if (input == LEFT) {
        if (subIndex == 0) {
          if (mainSpeed > 0) {
            mainSpeed --;
          }
        }
        else if (subIndex == 1) {
          if (mainLaserPower > 0) {
            mainLaserPower --;
          }
        }
        saveEEPROM_Data();
      }
      UpdateLCDSettingMenu();
    }
  }
  currentScreen = 0;
  DisplayCurrentScreen();
}

void UpdateLCDSettingMenu() {
  if (subIndex == 0) {
    lcd.setCursor(7, 1);
    lcd.print("<" + String(mainSpeed) + "%>  ");
    lcd.setCursor(13, 2);
    lcd.print(" " + String(mainLaserPower) + "%   ");
  } else if (subIndex == 1) {
    lcd.setCursor(7, 1);
    lcd.print(" " + String(mainSpeed) + "%   ");
    lcd.setCursor(13, 2);
    lcd.print("<" + String(mainLaserPower) + "%>  ");
  }
}

void UpdateLCDPositions() {
  lcd.setCursor(0, 2);
  lcd.print("X:");
  lcd.print(stepper_X.currentPosition());
  lcd.print("   ");
  lcd.setCursor(0, 3);
  lcd.print("Y:");
  lcd.print(stepper_Y.currentPosition());
  lcd.print("   ");
}

String SecsToTimeString(int totalTime) {
  int minutes = totalTime / 60;
  int seconds = totalTime % 60;
  char timeBuffer[6];
  sprintf(timeBuffer, "%02d:%02d", minutes, seconds);
  return timeBuffer;
}
