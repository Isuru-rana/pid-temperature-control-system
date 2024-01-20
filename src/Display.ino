void displayInit() {
  display.setTextSize(1);
  display.setTextColor(WHITE);

  display.setCursor(0, 0);
  display.print("Temperature");
  display.setCursor(90, 0);
  display.print("Set");
  display.drawLine(70, 0, 70, SCREEN_HEIGHT, WHITE);
  display.setCursor(5, 10);
  display.print("H:");
  display.setCursor(5, 25);
  display.print("C:");
  display.setCursor(5, 40);
  display.print("P:");
  display.setCursor(5, 55);
  display.print("A:");
  display.display();
}



void displayWriteData(int type) {
  if (type == 1) {
    int clearX = 15;
    int clearY = 10;
    int clearWidth = 50;
    int clearHeight = 65;

    for (int y = clearY; y < clearY + clearHeight; y++) {
      for (int x = clearX; x < clearX + clearWidth; x++) {
        display.drawPixel(x, y, BLACK);
      }
    }
    for (int i = 0; i < numControlUnits; i++) {
      display.setCursor(floatAlignRight(type, tempArray[i]), displayVPos[i]);
      display.print(tempArray[i]);
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
      display.print(setTempArray[i]);
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
  else if (num < -99.00 && num > -1000.00) space = 31;
  else if (num < -999.00) space = 20;
  for (uint8_t s = 0; s < space; s++) spaces++;
  return spaces;
}
