void serialSend(int type) {
  if (type == 1) {
    digitalWrite(TX_ON_PIN, HIGH);
    Serial.print("~");
    Serial.print(device_address);
    Serial.print(",");
    Serial.print(heaterPower);
    Serial.print(",");
    Serial.print("(0,0,0,0),");
    if (heaterPower) Serial.print(setTempArrayInt[0]);
    else Serial.print("00");
    Serial.print(",");
    Serial.print(tempArrayInt[0]);
    Serial.print(",");
    if (coolerPower) Serial.print(setTempArrayInt[1]);
    else Serial.print("00");
    Serial.print(",");
    Serial.print(tempArrayInt[1]);
    Serial.print(",");
    if (peltierPower) Serial.print(setTempArrayInt[2]);
    else Serial.print("00");
    Serial.print(",");
    Serial.print(tempArrayInt[2]);
    Serial.print(",");
    if (ambientPower) Serial.print(setTempArrayInt[3]);
    else Serial.print("00");
    Serial.print(",");
    Serial.println(tempArrayInt[3]);
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
    Serial.print(heatSafeT);
    Serial.println("R");
    digitalWrite(TX_ON_PIN, LOW);
  }
}