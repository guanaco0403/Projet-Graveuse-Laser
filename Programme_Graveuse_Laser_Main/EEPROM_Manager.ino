
void GetEEPROM_Data() {
  EEPROM.get(0, totalEngravingCount);
  EEPROM.get(3, totalEngravingTime);

  EEPROM.get(7, mainSpeed);
  EEPROM.get(11, mainLaserPower);
}

void saveEEPROM_Data() {
  EEPROM.put(0, totalEngravingCount);
  EEPROM.put(3, totalEngravingTime);

  EEPROM.put(7, mainSpeed);
  EEPROM.put(11, mainLaserPower);
}
