
void GetEEPROM_Data() {
  EEPROM.get(0, totalEngravingCount);
  EEPROM.get(3, totalEngravingTime);
}

void saveEEPROM_Data() {
  EEPROM.put(0, totalEngravingCount);
  EEPROM.put(3, totalEngravingTime);
}
