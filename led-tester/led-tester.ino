const int LED_PIN = 3;

void setup() {
  Serial.begin(9600);
  analogReadResolution(12);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);

}
