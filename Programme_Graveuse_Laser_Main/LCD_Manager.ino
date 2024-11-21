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

      minutes = totalEngravingTime / 60;
      seconds = totalEngravingTime % 60;
      timeBuffer[6];
      sprintf(timeBuffer, "%02d:%02d", minutes, seconds);
      lcd.setCursor(0, 2);
      lcd.print("Engr Time: ");
      lcd.print(timeBuffer);
      break;
    case 2:
      Serial.println("Screen2");
      lcd.clear();
      lcd.setCursor(2, 1);
      lcd.print("Manual Control");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(6, 1);
      lcd.print("Settings");
      break;
    case 40:
      lcd.clear();
      lcd.setCursor(6, 1);
      lcd.print("Manual Mode");
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
