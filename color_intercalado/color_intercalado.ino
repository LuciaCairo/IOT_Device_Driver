void setup() {
  pinMode(5, OUTPUT);
  pinMode(19, OUTPUT);
}

void loop() {
  digitalWrite(5, HIGH);   // Led ON
  digitalWrite(19, LOW);   // Led ON
  delay(100);              // pausa 1 seg
  digitalWrite(5, LOW);    // Led OFF
  digitalWrite(19, HIGH);    // Led OFF
  delay(100);              // pausa 1seg
}