#include "secret.hpp"
#include <WiFiNINA.h>
#include <ArduinoJson.h>

const int LED_PIN = 3;

// get the secrets for the .hpp file
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// hold the wifi status in this variable
int status = WL_IDLE_STATUS;

//for "production" use ssl for secure connection
const char* server = "arduino-workshop-server.onrender.com"; //url no http or https!
const int port = 443;
const char* endpoint = "/led";
WiFiSSLClient client;

// for local tessting using ngrok with http, insecure
/* const char* server = "981b9dd87b99.ngrok-free.app"; */
/* const int port = 80;  // Change to 80 */
/* const char* endpoint = "/led"; */

/* WiFiClient client; //using basic http */


void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LED_PIN, OUTPUT);

  connectWiFi(); //connects to the wifi
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


  getLEDStatus();
  delay(3000); //poll the server every 3 seconds
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

//claude helped write this, comments all me
void getLEDStatus() {
  if (WiFi.status() != WL_CONNECTED) { //see if we are still connected o the wifi
    Serial.println("No WiFi - skipping request");
    return; //this will exit this function
  }

  if (client.connect(server, port)) {
    // Send GET request, this is building the http request to send to the server, we don't see this in the terminal
    client.print("GET ");
    client.print(endpoint);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

    // Wait for response, times out if we don't get a response back from the server in 5 seconds, this is an enternity in computer time
    unsigned long timeout = millis(); //time we started in milliseconds
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Timeout");
        client.stop();
        return; //time out, exit funciton
      }
    }

    // Read entire response into string
    String response = "";
    while (client.available()) {
      response += client.readString();
    }

    //uncomment to see what the full server send, good for debugging
    Serial.println("--- Raw Response ---");
    Serial.println(response);
    Serial.println("--- End Response ---");

    // Find where JSON body starts (after empty line)
    int bodyStart = response.indexOf("\r\n\r\n"); // first look for end lines and returns
    if (bodyStart == -1) { // nothing found?
      bodyStart = response.indexOf("\n\n"); //look for just end lines instead
    }

    if (bodyStart != -1) { //this is true as long as an index value was found above

      String body = response.substring(bodyStart);
      body.trim();

      String jsonBody;


      // this bit is for finding the body in the server response
      // different servers will package the response differently, this should find the jsonbody regardless of server setup
      // Check if using chunked transfer encoding
      if (response.indexOf("Transfer-Encoding: chunked") != -1) {
        // Chunked: skip chunk size line and trailing markers
        int firstNewline = body.indexOf('\n');
        int lastNewline = body.lastIndexOf('\n');
        jsonBody = body.substring(firstNewline + 1, lastNewline);
        jsonBody.trim();
      } else {
        // Regular response: body is already the JSON
        jsonBody = body;
      }

      //check the json
      Serial.print("JSON Body: ");
      Serial.println(jsonBody);

      // Parse JSON from string
      // the server just send a string, lts turn it into json
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, jsonBody);

      if (error) { //error checking
        Serial.print("JSON parsing failed: ");
        Serial.println(error.c_str());
      } else {
        // Extract lightState (matching your server's JSON key)
        bool ledStatus = doc["lightState"]; //this gets the value given the "lightState" key from our json.

        digitalWrite(LED_PIN, ledStatus ? HIGH : LOW); //ternery op, if the ledStatus is true, turn the light on, else turn it off

        Serial.print("LED on D3: ");
        Serial.println(ledStatus ? "ON" : "OFF");
      }
    } else {
      Serial.println("Could not find response body");
    }

    client.stop();
  } else {
    Serial.println("Connection failed");
  }
}
