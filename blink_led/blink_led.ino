#define LED27 27
#define LED25 25
void setup() {
  pinMode(LED27, OUTPUT);
  pinMode(LED25, OUTPUT);
}

void loop() {
  digitalWrite(LED27, HIGH);
  delay(1000);
  digitalWrite(LED27, LOW);
  digitalWrite(LED25, HIGH);
  delay(1000);
  digitalWrite(LED25, LOW);
}
