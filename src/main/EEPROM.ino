void ReadEEPROM() {
  EEPROM.get(eepromAddress[1] * sizeof(int), device_address);
  //EEPROM.get(eepromAddress[2] * sizeof(float), setTempArray[0]);
  //EEPROM.get(eepromAddress[3] * sizeof(float), setTempArray[1]);
  //EEPROM.get(eepromAddress[4] * sizeof(float), setTempArray[2]);
  //EEPROM.get(eepromAddress[5] * sizeof(float), setTempArray[3]);
  EEPROM.get(eepromAddress[6] * sizeof(bool), heatSafeT);
  EEPROM.get(eepromAddress[7] * sizeof(bool), coolerPower);
}

void WriteEEPROM() {
  EEPROM.put(eepromAddress[1] * sizeof(int), device_address);
  //EEPROM.put(eepromAddress[2] * sizeof(float), setTempArray[0]);
  //EEPROM.put(eepromAddress[3] * sizeof(float), setTempArray[1]);
  //EEPROM.put(eepromAddress[4] * sizeof(float), setTempArray[2]);
  //EEPROM.put(eepromAddress[5] * sizeof(float), setTempArray[3]);
  EEPROM.put(eepromAddress[6] * sizeof(bool), heatSafeT);
  EEPROM.put(eepromAddress[7] * sizeof(bool), coolerPower);
}