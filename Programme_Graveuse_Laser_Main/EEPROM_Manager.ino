/*
 * Gestion des données de l'EEPROM
 *  - Temps total de gravure
 *  - Nombre total de gravures
 *  - Vitesse max de gravure
 *  - Puissance du laser
 */

/* Fonction pour sauvegarder les données dans l'EEPROM */
void GetEEPROM_Data() {
  EEPROM.get(0, totalEngravingCount);
  EEPROM.get(3, totalEngravingTime);

  EEPROM.get(7, mainSpeed);
  EEPROM.get(11, mainLaserPower);
}

/* Fonction pour récupèrer les données dans l'EEPROM */
void saveEEPROM_Data() {
  EEPROM.put(0, totalEngravingCount);
  EEPROM.put(3, totalEngravingTime);

  EEPROM.put(7, mainSpeed);
  EEPROM.put(11, mainLaserPower);
}

/* Fonction pour réinistialiser les données de l'EEPROM a l'état d'usine */
void CheckResetFactory() {
  if (digitalRead(Joy_Switch) == LOW) {
    totalEngravingCount = 0;
    totalEngravingTime = 0;
    mainSpeed = 30;
    mainLaserPower = 25;
    saveEEPROM_Data(); // Appel a la sauvegarde des données

    // Affichage de la réinitialisation a l'état d'usine sur l'écran
    lcd.clear();
    lcd.setCursor(3, 1);
    lcd.print("Reset Factory");
    lcd.setCursor(3, 2);
    lcd.print("In Progress...");
    delay(3000);
  }
}
