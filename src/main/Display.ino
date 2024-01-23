void displayInit() {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(5, 0);
  display.print("Address:");
  display.setCursor(55, 0);
  display.print(device_address);
  display.setCursor(70, 0);
  display.print("Timer:");
  display.drawLine(70, 10, 70, SCREEN_HEIGHT, WHITE);
  display.setCursor(5, 10);
  display.print("H:");
  display.setCursor(5, 25);
  display.print("C:");
  display.setCursor(5, 40);
  display.print("P:");
  display.setCursor(5, 55);
  display.print("A:");
  display.display();
  displayWriteData(3);
}



void displayWriteData(int type) {
  if (type == 1) {
    int clearX = 15;
    int clearY = 10;
    int clearWidth = 54;
    int clearHeight = 65;

    for (int y = clearY; y < clearY + clearHeight; y++) {
      for (int x = clearX; x < clearX + clearWidth; x++) {
        display.drawPixel(x, y, BLACK);
      }
    }
    for (int i = 0; i < numControlUnits; i++) {
      display.setCursor(floatAlignRight(type, temp[i]), displayVPos[i]);
      if (i == 0) {
        display.setCursor(floatAlignRight(type, temp[i]) + 15, displayVPos[i]);
        display.print(tempArrayInt[i]);
      } else {
        display.print(temp[i]);
      }
    }
  } else if (type == 0) {
    int clearX = 71;
    int clearY = 10;
    int clearWidth = 128;
    int clearHeight = 64;

    for (int y = clearY; y < clearY + clearHeight; y++) {
      for (int x = clearX; x < clearX + clearWidth; x++) {
        display.drawPixel(x, y, BLACK);
      }
    }
    for (int i = 0; i < numControlUnits; i++) {
      display.setCursor(floatAlignRight(type, setTempArray[i]), displayVPos[i]);
      if (i == 0) {
        display.setCursor(floatAlignRight(type, setTempArray[i]) + 15, displayVPos[i]);
        display.print(setTempArrayInt[i]);
      }
      if (i == 1) {
        if (!coolerPower) {
          display.setCursor(100, displayVPos[i]);
          display.print("OFF");
        } else {
          display.print(setTempArray[i]);
        }
      }
      if (i == 2) {
        if (!peltierPower) {
          display.setCursor(100, displayVPos[i]);
          display.print("OFF");
        } else {
          display.print(setTempArray[i]);
        }
      }
      if (i == 3) {
        if (!ambientPower) {
          display.setCursor(100, displayVPos[i]);
          display.print("OFF");
        } else {
          display.print(setTempArray[i]);
        }
      }
    }
  } else if (type == 2) {
    int clearX = 54;
    int clearY = 0;
    int clearWidth = 15;
    int clearHeight = 10;

    for (int y = clearY; y < clearY + clearHeight; y++) {
      for (int x = clearX; x < clearX + clearWidth; x++) {
        display.drawPixel(x, y, BLACK);
      }
    }
    display.setCursor(55, 0);
    display.print(device_address);
  } else if (type == 3) {
    int clearX = 104;
    int clearY = 10;
    int clearWidth = 20;
    int clearHeight = 10;

    for (int y = clearY; y < clearY + clearHeight; y++) {
      for (int x = clearX; x < clearX + clearWidth; x++) {
        display.drawPixel(x, y, BLACK);
      }
    }
    display.setCursor(105, 0);
    if (heatSafeT) {
      display.print("ON");
    } else {
      display.print("OFF");
    }
  }
  display.display();
}


int floatAlignRight(int place, int num) {
  int space = 0;
  int spaces = place > 0 ? 0 : 55;

  if (num > 999.00) space = 25;
  else if (num > 99.00 && num < 1000.00) space = 31;
  else if (num > 9.00 && num < 100.00) space = 37;
  else if (num < 10.00 && num >= 0.00) space = 43;
  else if (num > -10.00 && num < 0.00) space = 43;
  else if (num < -9.00 && num > -100.00) space = 31;
  else if (num < -99.00 && num > -1000.00) space = 25;
  else if (num < -999.00) space = 20;
  for (uint8_t s = 0; s < space; s++) spaces++;
  return spaces;
}
