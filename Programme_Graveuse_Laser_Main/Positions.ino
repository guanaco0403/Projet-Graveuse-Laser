class Instruction {
  public:
    int x;
    int y;
    bool engrave;
};

Instruction Lettres_IESN[] = {
  // I
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
  int Speed = mainSpeed*40;
  int numInstructions = sizeof(Lettres_IESN) / sizeof(Lettres_IESN[0]);
  int i = 0;

  // Auto Homing pour la sécurité
  stepper_X.stop();
  stepper_Y.stop();
  AutoHome();
  //

  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Engraving [0%]");
  PrintProgressBar(0, 1);

  int computedTime = ComputeEngravingTime(Speed, numInstructions, 0);
  lcd.setCursor(10, 2);
  lcd.print("ETA: ");
  lcd.print(SecsToTimeString(computedTime));

  long startMillis = millis();
  long oldUpdate = 0;

  while (i < numInstructions) {
    if (SystemState == false) { // cas d'un ARU
      return;
    }

    if (Lettres_IESN[i].engrave == true) {
      SetLaser(mainLaserPower);
      GoTo_AdaptedSpeed(Lettres_IESN[i].x, Lettres_IESN[i].y, Speed);
      SetLaser(0);
    }
    else {
      GoTo(Lettres_IESN[i].x, Lettres_IESN[i].y);
    }
    i++;
    GravingPercent(i, numInstructions);
    UpdateLCDPositions();
    if (millis() - oldUpdate >= 5000) {
      computedTime = ComputeEngravingTime(Speed, numInstructions, i);
      lcd.setCursor(15, 2);
      lcd.print(SecsToTimeString(computedTime));
      oldUpdate = millis();
    }
  }
  if (SystemState == false) { // cas d'un ARU
    return;
  }

  int engravingTime = (millis() - startMillis) / 1000; // en secondes
  totalEngravingTime = totalEngravingTime + engravingTime;
  totalEngravingCount += 1;
  saveEEPROM_Data();
  GoTo(0, 0);
  DisplayCurrentScreen();
  JobDoneSound();
}

void GravingPercent(float currentStep, float maxSteps) {
  int percent;
  percent = (int)((currentStep / maxSteps) * 100);
  lcd.setCursor(13, 0);
  lcd.print(percent);
  lcd.print("%]");
  PrintProgressBar(percent, 1);
}

int ComputeEngravingTime(float Speed, int numInstructions, int currentInstruction) {
  float k = 6.3;
  double computeTime = 0;
  for (int i = currentInstruction; i < numInstructions; i++) {
    int deltaX = abs(Lettres_IESN[i].x - Lettres_IESN[i + 1].x);
    int deltaY = abs(Lettres_IESN[i].y - Lettres_IESN[i + 1].y);
    if (deltaX > deltaY) {
      computeTime += abs((k * deltaX) / (Speed / 10));
    }
    else {
      computeTime += abs((k * deltaY) / (Speed / 10));
    }
  }

  return computeTime / 60;
}
