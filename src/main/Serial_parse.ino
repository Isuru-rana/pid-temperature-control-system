void parseCommand(String command) {
  if (command.startsWith("/")) {
    String systemAddressStr = command.substring(1, 2);
    int systemAddress = systemAddressStr.toInt();
    int wIndex = command.indexOf('W') + 1;
    int rIndex = command.indexOf('R');
    int iIndex = command.indexOf('I') + 1;
    int lIndex = command.indexOf('L') + 1;
    int fIndex = command.indexOf('F') + 1;
    int dIndex = command.indexOf('D') + 1;

    if (systemAddress == device_address && !wIndex && !iIndex) {
      int hIndex = command.indexOf('H') + 1;
      int cIndex = command.indexOf('C') + 1;
      int pIndex = command.indexOf('P') + 1;
      int aIndex = command.indexOf('A') + 1;
      int jIndex = command.indexOf('J') + 1;


      if ((systemPower) && (hIndex || cIndex || pIndex || aIndex)) {
        String cBuff = command.substring(cIndex, cIndex + 2);
        String hBuff = command.substring(hIndex, hIndex + 2);
        String pBuff = command.substring(pIndex, pIndex + 2);
        String aBuff = command.substring(aIndex, aIndex + 2);

        if (cIndex >0 && cBuff == ("00")) {
          coolerPower = false;
          setTempArray[1] = 0;
        } else {
          coolerPower = true;
          setTempArray[1] = cIndex > 0 ? command.substring(cIndex, (pIndex > 0 ? pIndex : aIndex) > 0 ? (pIndex > 0 ? pIndex : aIndex) : rIndex).toInt() : setTempArray[1];
        }
        if (hIndex > 0 && hBuff == ("00")) {
          heaterPower = false;
          setTempArray[0] = 0;
        } else {
          heaterPower = true;
          setTempArray[0] = hIndex > 0 ? command.substring(hIndex, (cIndex > 0 ? cIndex : (pIndex > 0 ? pIndex : (aIndex > 0 ? aIndex : rIndex)))).toInt() : setTempArray[0];
        }
        if (pIndex > 0 && pBuff == ("00")) {
          peltierPower = false;
          setTempArray[2] = 0;
        } else {
          peltierPower = true;
          setTempArray[2] = pIndex > 0 ? command.substring(pIndex, aIndex > 0 ? aIndex : rIndex).toInt() : setTempArray[2];
        }
        if (aIndex > 0 && aBuff == ("00")) {
          ambientPower = false;
          setTempArray[3] = 0;
        } else {
          ambientPower = true;
          setTempArray[3] = aIndex > 0 ? command.substring(aIndex, rIndex).toInt() : setTempArray[3];
        }


        for (int i = 0; i < numControlUnits; i++) {
          setTempArrayInt[i] = int(setTempArray[i]);
        }

        displayWriteData(0);
        serialSend(1);

      } else if (jIndex > 0 && rIndex > 0) {

        char jVal = command.charAt(jIndex);

        if (jVal == '1') {
          if (!systemPower) {
            serialSend(2);
            digitalWrite(POWER_ON_PIN, HIGH);
            systemPower = true;
            serialConState = true;
          } else {
            systemPower = false;
            serialSend(2);
            digitalWrite(POWER_ON_PIN, LOW);
            serialConState = false;
            serialSync();
          }
        } else if (systemPower && (jVal == '2')) {
          serialSend(7);
          if (!J2) {
            digitalWrite(J2COMMAND_PIN, HIGH);
            J2 = 1;
          } else {
            digitalWrite(J2COMMAND_PIN, LOW);
            J2 = 0;
          }
        } else if (serialConState && jVal == '5') {
          //serialTimer = millis();
          heaterPower = 1;
          setTempArray[0] = 0;
          displayWriteData(0);
          digitalWrite(HEATER_Heat_PIN, heaterPower);

          serialSend(3);
        }
      } else if (systemPower && (fIndex > 0 && rIndex > 0)) {
        String fStr = command.substring(fIndex, rIndex);
        valveVoltage = fStr.toInt() / 1000.0;
        setValveVoltage(1);
        serialSend(8);
      } else if (systemPower && (dIndex > 0 && rIndex > 0)) {
        String fStr = command.substring(dIndex, rIndex);
        dcMotorVoltage = fStr.toInt() / 1000.0;
        setValveVoltage(2);
        serialSend(9);
      } else if (systemPower && (lIndex > 0 && rIndex > 0)) {
        char lVal = command.charAt(lIndex);
        if (isdigit(lVal)) {
          int stateIndex = lVal - '0';
          if (stateIndex >= 0 && stateIndex < 16) {
            for (int i = 0; i < 4; i++) {
              digitalWrite(servopinno[i], pinArray[stateIndex][i] ? HIGH : LOW);
              Lbuff[i] = pinArray[stateIndex][i] ? '1' : '0';
            }
          }
        }
        serialSend(0);
      }
    } else if (wIndex > 0 && rIndex > 0 && !iIndex) {

      if (command.substring(wIndex + 1, rIndex).equals("?")) {
        serialSend(4);
      } else {
        device_address = command.substring(wIndex + 1, rIndex).toInt();
        EEPROM.put(eepromAddress[1] * sizeof(int), device_address);
        displayWriteData(2);
        serialSend(5);
      }
    } else if (systemPower && (iIndex > 0 && rIndex > 0 && !wIndex)) {

      if (command.substring(iIndex + 1, rIndex).equals("1")) {
        heatSafeT = 1;
        displayWriteData(3);
        EEPROM.put(eepromAddress[7] * sizeof(bool), heatSafeT);
        heaterTimer = millis();
        serialSend(6);
      } else if (command.substring(iIndex + 1, rIndex).equals("0")) {
        heatSafeT = 0;
        displayWriteData(3);
        EEPROM.put(eepromAddress[7] * sizeof(bool), heatSafeT);
        serialSend(6);
      }
    }

    else {
      Serial.flush();
    }
  }
}
