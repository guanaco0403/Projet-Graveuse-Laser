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
