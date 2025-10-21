#include "secret.hpp"
#include <WiFiNINA.h>

const int PHOTO_PIN = A0; //phototransisotr attached here
const int BUTTON_PIN = 2;
const int NUM_SAMPLES = 10;

/* const char* SENSOR_ID = "sensor_01"; */

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

//for "production" use ssl for secure connection
const char* server = "arduino-workshop-server.onrender.com"; //url no http or https!
const int port = 443;
const char* endpoint = "/data";
WiFiSSLClient client;

// for local tessting using ngrok with http, insecure
/* const char* server = "981b9dd87b99.ngrok-free.app"; */
/* const int port = 80;  // Change to 80 */
/* const char* endpoint = "/data"; */
/* WiFiClient client; //using basic http */

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

// claude helped write this, comments are mine
void sendLightReading(int lightLevel) {
  if (WiFi.status() != WL_CONNECTED) { //if the wifi isn't connected you can't send shit
    Serial.println("No WiFi - skipping request");
    return; //exit without any actions
  }

  Serial.print("Attempting connection to ");
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);

  if (client.connect(server, port)) {
    Serial.println("Sending data...");

    String payload = "{\"sensor_reading\":" + String(lightLevel) + "}";

    /* String payload = "{\"sensor_reading\":" + String(lightLevel) + */
    /*   ",\"sensor_id\":\"" + String(SENSOR_ID) + "\"}"; */

    //this builds the string for the http request
    client.println("POST " + String(endpoint) + " HTTP/1.1"); //type of request we are making
    client.println("Host: " + String(server)); //where is it going?
    client.println("Content-Type: application/json"); //what type of data is it?
    client.print("Content-Length: "); //how long is the message? this is used for error checking
    client.println(payload.length()); //the length
    client.println("Connection: close"); //what to do at the end of message
    client.println();
    client.println(payload); //this is the body of the message, our json data

    // Wait for response with timeout
    // if you don't head back if 5 seconds it failed
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) { 
        Serial.println("Timeout waiting for response");
        client.stop();
        return;
      }
    }

    // Read the response that the server is sending back
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }

    client.stop(); //we're done here!
    Serial.println("Request complete");
  } else {
    Serial.println("Connection to server failed");
  }
}
