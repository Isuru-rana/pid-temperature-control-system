void parseCommand(String command) {
  if (command.startsWith("/")) {
    String systemAddressStr = command.substring(1, 2);
    int systemAddress = systemAddressStr.toInt();
    int wIndex = command.indexOf('W') + 1;
    int rIndex = command.indexOf('R');
    int iIndex = command.indexOf('I') + 1;

    if (systemAddress == device_address && !wIndex && !iIndex) {
      int hIndex = command.indexOf('H') + 1;
      int cIndex = command.indexOf('C') + 1;
      int pIndex = command.indexOf('P') + 1;
      int aIndex = command.indexOf('A') + 1;
      int jIndex = command.indexOf('J') + 1;


      if ((systemPower) && (hIndex || cIndex || pIndex || aIndex)) {
        String cBuff = command.substring(cIndex, cIndex + 2);
        if (cBuff == ("00")) {
          coolerPower = false;
        } else {
          coolerPower = true;
          setTempArray[1] = cIndex > 0 ? command.substring(cIndex, (pIndex > 0 ? pIndex : aIndex) > 0 ? (pIndex > 0 ? pIndex : aIndex) : rIndex).toInt() : setTempArray[1];
        }
        setTempArray[0] = hIndex > 0 ? command.substring(hIndex, (cIndex > 0 ? cIndex : (pIndex > 0 ? pIndex : (aIndex > 0 ? aIndex : rIndex)))).toInt() : setTempArray[0];
        setTempArray[2] = pIndex > 0 ? command.substring(pIndex, aIndex > 0 ? aIndex : rIndex).toInt() : setTempArray[2];
        setTempArray[3] = aIndex > 0 ? command.substring(aIndex, rIndex).toInt() : setTempArray[3];

        for (int i = 0; i < numControlUnits; i++) {
          setTempArrayInt[i] = int(setTempArray[i]);
        }
        //WriteEEPROM();
        displayWriteData(0);
        serialSend(1);

      } else if (jIndex > 0 && rIndex > 0) {

        char jVal = command.charAt(jIndex);
        
        if (jVal == '1') {
          if (!systemPower) serialSend(2);
          else systemPower = false;
        } else if (jVal == '5') {
          serialTimer = millis();
          systemPower = true;
          digitalWrite(POWER_ON_PIN, systemPower);

          serialSend(3);
        }
      }
    } else if (wIndex > 0 && rIndex > 0 && !iIndex) {

      if (command.substring(wIndex + 1, rIndex).equals("?")) {
        serialSend(4);
      } else {
        device_address = command.substring(wIndex + 1, rIndex).toInt();
        EEPROM.put(eepromAddress[1] * sizeof(int), device_address);
        serialSend(5);
      }
    } else if ((iIndex > 0 && rIndex > 0 && !wIndex)&& systemPower) {

      if (command.substring(iIndex + 1, rIndex).equals("1")) {
        heatSafeT = 1;
        EEPROM.put(eepromAddress[7] * sizeof(bool), heatSafeT);
        heaterTimer = millis();
        serialSend(6);
      } else if (command.substring(iIndex + 1, rIndex).equals("0")) {
        heatSafeT = 0;
        EEPROM.put(eepromAddress[7] * sizeof(bool), heatSafeT);
        serialSend(6);
      }
    }

    else {
      Serial.flush();
    }
  }
}
