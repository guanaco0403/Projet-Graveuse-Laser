class Instruction {
  public:
    int x;
    int y;
    bool engrave;
};

Instruction Lettres_IESN[] = {
  // I
  /*{0, 3200, false},
    {800, 3200, true},
    {400, 3200, false},
    {400, 4000, true},
    {0, 4000, false},
    {800, 4000, true},*/

  {0, 500, false},
  {1200, 500, true},
  {1200, 700, true},
  {700, 700, true},
  {700, 1900, true},
  {1200, 1900, true},
  {1200, 2100, true},
  {0, 2100, true},
  {0, 1900, true},
  {500, 1900, true},
  {500, 700, true},
  {0, 700, true},
  {0, 500, true},

  // E
  /*{1800, 3200, false},
    {1000, 3200, true},
    {1000, 4000, true},
    {1800, 4000, true},
    {1000, 3600, false},
    {1400, 3600, true},*/

  {1600, 500, false},
  {2800, 500, true},
  {2800, 700, true},
  {1800, 700, true},
  {1800, 1200, true},
  {2400, 1200, true},
  {2400, 1400, true},
  {1800, 1400, true},
  {1800, 1900, true},
  {2800, 1900, true},
  {2800, 2100, true},
  {1600, 2100, true},
  {1600, 500, true},

  // S
  /*{2800, 3200, false},
    {2200, 3200, true},
    {2000, 3300, true},
    {2000, 3500, true},
    {2200, 3600, true},
    {2600, 3600, true},
    {2800, 3700, true},
    {2800, 3900, true},
    {2600, 4000, true},
    {2000, 4000, true},*/

  {3600, 500, false},
  {4400, 500, true},
  {4400, 700, true},
  {3600, 700, true},
  {3400, 900, true},
  {3400, 1100, true},
  {3600, 1200, true},
  {4200, 1200, true},
  {4400, 1400, true},
  {4400, 1900, true},
  {4100, 2100, true},
  {3200, 2100, true},
  {3200, 1900, true},
  {4000, 1900, true},
  {4200, 1800, true},
  {4200, 1600, true},
  {4000, 1400, true},
  {3400, 1400, true},
  {3200, 1200, true},
  {3200, 700, true},
  {3500, 500, true},

  // N
  /*{3800, 3200, false},
    {3800, 4000, true},
    {3000, 3200, true},
    {3000, 4000, true}*/

  {4800, 500, false},
  {5100, 500, true},
  {5800, 1900, true},
  {5800, 500, true},
  {6000, 500, true},
  {6000, 2100, true},
  {5700, 2100, true},
  {5000, 700, true},
  {5000, 2100, true},
  {4800, 2100, true},
  {4800, 500, true},

};

void Grave() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Engraving [0%]");
  /*lcd.setCursor(0, 1);
    lcd.print("[");
    lcd.setCursor(19, 1);
    lcd.print("]");*/
  PrintProgressBar(0, 1);

  int Speed = 1000;
  int numInstructions = sizeof(Lettres_IESN) / sizeof(Lettres_IESN[0]);
  int i = 0;
  long startMillis = millis();

  while (i < numInstructions) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }

    if (Lettres_IESN[i].engrave == true) {
      SetLaser(15);
      GoTo_AdaptedSpeed(Lettres_IESN[i].x, Lettres_IESN[i].y, Speed);
      SetLaser(0);
    }
    else {
      GoTo(Lettres_IESN[i].x, Lettres_IESN[i].y);
    }
    i++;
    GravingPercent(i, numInstructions);
  }
  if (SystemState == false) { // cas d'un ARU
    return;
  }

  int engravingTime = (millis() - startMillis) / 1000; // en secondes
  Serial.println(millis() - startMillis);
  Serial.println(engravingTime);
  totalEngravingTime = totalEngravingTime + engravingTime;
  totalEngravingCount += 1;
  saveEEPROM_Data();
  GoTo(0, 0);
  oldScreen = 255;
}

void GravingPercent(float currentStep, float maxSteps) {
  int percent;
  percent = (int)((currentStep / maxSteps) * 100);
  Serial.print("Job Percent: ");
  Serial.print(percent);
  Serial.println("%");
  lcd.setCursor(13, 0);
  lcd.print(percent);
  lcd.print("%]");
  PrintProgressBar(percent, 1);
  UpdateLCDPositions();
}
