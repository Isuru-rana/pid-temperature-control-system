void serialSend(int type) {
  if (type == 1) {
    digitalWrite(TX_ON_PIN, HIGH);
    Serial.print("~");
    Serial.print(device_address);
    Serial.print(",");
    Serial.print(heaterState);
    Serial.print(",");
    Serial.print("(0,0,0,0),");
    Serial.print(setTempArray[0]);
    Serial.print(",");
    Serial.print(temp[0]);
    Serial.print(",");
    if (coolerPower) {
      Serial.print(setTempArray[1]);
      Serial.print(",");
    } else {
      Serial.print("00");
      Serial.print(",");
    }
    Serial.print(temp[1]);
    Serial.print(",");
    Serial.print(setTempArray[2]);
    Serial.print(",");
    Serial.print(temp[2]);
    Serial.print(",");
    Serial.print(setTempArray[3]);
    Serial.print(",");
    Serial.println(temp[3]);
    digitalWrite(TX_ON_PIN, LOW);
  }
  if (type == 2) {
    digitalWrite(TX_ON_PIN, HIGH);
    Serial.print("~");
    Serial.print(device_address);
    Serial.println("J001");
    digitalWrite(TX_ON_PIN, LOW);
  }
  if (type == 3) {
    digitalWrite(TX_ON_PIN, HIGH);
    Serial.print("~");
    Serial.print(device_address);
    Serial.println("J101");
    digitalWrite(TX_ON_PIN, LOW);
  }
  if (type == 4) {
    digitalWrite(TX_ON_PIN, HIGH);
    Serial.print("~");
    Serial.print(device_address);
    Serial.println("R");
    digitalWrite(TX_ON_PIN, LOW);
  }
  if (type == 5) {
    digitalWrite(TX_ON_PIN, HIGH);
    Serial.print("~");
    Serial.print(device_address);
    Serial.println("R");
    digitalWrite(TX_ON_PIN, LOW);
  }
  if (type == 6) {
    digitalWrite(TX_ON_PIN, HIGH);
    Serial.print("~I");
    Serial.print(heatSFT);
    Serial.println("R");
    digitalWrite(TX_ON_PIN, LOW);
  }
}