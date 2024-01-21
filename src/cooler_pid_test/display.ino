//=== OLED Display positions data ===

#define Title_1_POS_X 5
#define Title_1_POS_Y 0

#define Tempi_1_POS_X 8
#define Tempi_1_POS_Y 16

#define Tempd_1_POS_X 13
#define Tempd_1_POS_Y 32

#define Set_1_POS_X 10
#define Set_1_POS_Y (OLED_Height / 2+15)

#define setTemp_1_POS_X 5
#define setTemp_1_POS_Y (OLED_Height - 8)



void feedDisplay(int sensor, float value) {

  int intTemp = static_cast<int>(value);
  int decTemp = static_cast<int>((value - intTemp) * 100 + 1);


  display.setTextColor(SSD1306_WHITE);

  if (sensor == 1) {
    display.setTextSize(2);
    display.setCursor(Tempi_1_POS_X, Tempi_1_POS_Y);
    display.print(intTemp);
    display.setTextSize(1);
    display.setCursor(Tempd_1_POS_X, Tempd_1_POS_Y);
    display.print(decTemp);
  }
  
  display.display();
}


void displayInt() {


  display.drawLine(OLED_Width / 3, 0, OLED_Width / 3, OLED_Height, SSD1306_WHITE);
  display.drawLine(2 * OLED_Width / 3, 0, 2 * OLED_Width / 3, OLED_Height, SSD1306_WHITE);


  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(Title_1_POS_X, Title_1_POS_Y);
  display.print(sensor_1_name);

  //setNonTemp(1, 1, 1);

  display.setTextSize(1);
  display.setCursor(Set_1_POS_X, Set_1_POS_Y);
  display.print(F("Set"));


  display.setCursor(setTemp_1_POS_X, setTemp_1_POS_Y);
  display.print(set_temp1);

  display.display();
}

