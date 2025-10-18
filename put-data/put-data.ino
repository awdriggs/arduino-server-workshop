#include "secret.hpp"
#include <WiFiNINA.h>

const int PHOTO_PIN = A0;
const int BUTTON_PIN = 2;
const int NUM_SAMPLES = 10;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

const char* server = "62be2b50b344.ngrok-free.app";
const int port = 80;  // Change to 80
const char* endpoint = "/data";

WiFiClient client;  // Change back to WiFiClient (not SSL)



void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  analogReadResolution(12);
  pinMode(PHOTO_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  connectWiFi();
}

void loop() {
  // Check WiFi and update LED
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW); // LED off when disconnected
    Serial.println("WiFi disconnected! Reconnecting...");
    connectWiFi();
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // LED on when connected
  }
  
  if (digitalRead(BUTTON_PIN) == HIGH) {
    long sum = 0;
    
    for(int i = 0; i < NUM_SAMPLES; i++) {
      sum += analogRead(PHOTO_PIN);
      delayMicroseconds(100);
    }
    
    int lightLevel = sum / NUM_SAMPLES;
    
    Serial.print("Light Level: ");
    Serial.println(lightLevel);
    
    sendLightReading(lightLevel);
    
    delay(500);
  }
  
  delay(100);
}

void connectWiFi() {
  status = WL_IDLE_STATUS;
  digitalWrite(LED_BUILTIN, LOW); // LED off while connecting
  
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  
  digitalWrite(LED_BUILTIN, HIGH); // LED on when connected
  Serial.println("You're connected to the network");
  Serial.println("---------------------------------------");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println("---------------------------------------");
}

void sendLightReading(int lightLevel) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No WiFi - skipping request");
    return;
  }

  Serial.print("Attempting connection to ");
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);
  
  if (client.connect(server, port)) {
    Serial.println("Sending data...");
    
    String payload = "{\"sensor_reading\":" + String(lightLevel) + "}";
    
    client.println("POST " + String(endpoint) + " HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println("Connection: close");
    client.println();
    client.println(payload);
    
    delay(100);
    
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
    
    client.stop();
    Serial.println("Request complete");
  } else {
    Serial.println("Connection to server failed");
  }
}
