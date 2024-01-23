void ReadEEPROM() {
  EEPROM.get(eepromAddress[1] * sizeof(int), device_address);
  EEPROM.get(eepromAddress[6] * sizeof(bool), heatSafeT);
  //EEPROM.get(eepromAddress[7] * sizeof(bool), coolerPower);
}

void WriteEEPROM() {
  EEPROM.put(eepromAddress[1] * sizeof(int), device_address);
  EEPROM.put(eepromAddress[6] * sizeof(bool), heatSafeT);
  //EEPROM.put(eepromAddress[7] * sizeof(bool), coolerPower);
}