const int PHOTO_PIN = A0;
const int BUTTON_PIN = 2;
const int LED_PIN = 3;
const int NUM_SAMPLES = 10;

void setup() {
  Serial.begin(9600);
  analogReadResolution(12);
  pinMode(PHOTO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    long sum = 0;

    for(int i = 0; i < NUM_SAMPLES; i++) {
      sum += analogRead(PHOTO_PIN);
      delayMicroseconds(100);
    }

    int lightLevel = sum / NUM_SAMPLES;

    Serial.print("Light Level: ");
    Serial.println(lightLevel);
    
    digitalWrite(LED_PIN, HIGH);

    delay(200); // Debounce - prevents multiple readings from one press
  } else {
    digitalWrite(LED_PIN, LOW);
  }


}
