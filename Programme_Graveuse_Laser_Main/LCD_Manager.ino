/*
 * Gestion du LCD
 * 
 * Menus (currentScreen):
 *  - 0   Menu Principal
 *  - 1   Statistiques
 *  - 2   Selection Mode Manuel
 *  - 3   Selection Mode Paramètrage
 *  - 10  Mode Paramètrage
 *  - 40  Mode Manuel
 *  - 100 Menu Arrêt D'urgence
 */

/* Fonction qui permet l'affichage du menu actuel a l'écran */
void DisplayCurrentScreen() {
  int minutes = 0; // Variable locale pour retenir les minutes
  int seconds = 0; // Variable locale pour retenir les secondes
  char timeBuffer[6]; // Variable locale pour retenir le buffer de temps

  // On check dans quel menu on est pour afficher le bon menu a l'écran
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
      lcd.setCursor(3, 1);
      lcd.print("Manual Control");
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(6, 1);
      lcd.print("Settings");
      break;
    case 10:
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("[Settings Menu]");
      lcd.setCursor(0, 1);
      lcd.print("Speed:");
      lcd.setCursor(0, 2);
      lcd.print("Laser Power:");
      lcd.setCursor(2, 3);
      lcd.print("[PRESS] to exit");
      UpdateLCDSettingMenu(0); // On affiche les valeurs des paramètres a l'écran
      break;
    case 40:
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("[Manual Mode]");
      lcd.setCursor(2, 1);
      lcd.print("Use the joystick");
      lcd.setCursor(3, 2);
      lcd.print("for movements");
      lcd.setCursor(2, 3);
      lcd.print("[PRESS] to exit");
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

/* Fonction pour imprimer une barre de progression a un certain pourcentage a une certaine ligne de l'écran */
void PrintProgressBar(int percent, int line) {
  lcd.setCursor(0, 1);
  lcd.print("[");
  int convert = map(percent, 0, 100, 0, 18); // Map pour transformer le pourcentage en un nombre de "=" a l'écran
  int i = 0;

  // Impression des "=" a l'écran pour la barre de progression
  for (i; i < convert; i++) {
    lcd.print("=");
  }
  for (i; i < 18; i++) {
    lcd.print(" ");
  }
  lcd.print("]");
}

/* Fonction pour afficher les valeurs des paramètres sur l'écran ainsi que d'afficher le quel est sélectionné actuellement */
void UpdateLCDSettingMenu(int settingIndex) {
  if (settingIndex == 0) {
    lcd.setCursor(7, 1);
    lcd.print("<" + String(mainSpeed) + "%>  ");
    lcd.setCursor(13, 2);
    lcd.print(" " + String(mainLaserPower) + "%   ");
  } else if (settingIndex == 1) {
    lcd.setCursor(7, 1);
    lcd.print(" " + String(mainSpeed) + "%   ");
    lcd.setCursor(13, 2);
    lcd.print("<" + String(mainLaserPower) + "%>  ");
  }
}

/* Fonction pour afficher les coordonées actuelles du laser a l'écran */
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

/* Fonction pour calculer et afficher le pourcentage de la progression de la gravure sur l'écran */
void GravingPercent(float currentStep, float maxSteps) {
  int percent;
  percent = (int)((currentStep / maxSteps) * 100); // Calcul du pourcentage
  lcd.setCursor(13, 0);
  lcd.print(percent);
  lcd.print("%]");
  PrintProgressBar(percent, 1); // Affichage de la barre de progression a la ligne 1 de l'écran
}

/* Fonction pour convertir un temps en secondes en string [minutes:secondes] */
String SecsToTimeString(int totalTime) {
  int minutes = totalTime / 60; // Calcul des minutes
  int seconds = totalTime % 60; // Calcul des secondes
  char timeBuffer[6];
  sprintf(timeBuffer, "%02d:%02d", minutes, seconds); // Convertion des minutes et secondes en string [minutes:secondes]
  return timeBuffer; // On retourne le string
}
