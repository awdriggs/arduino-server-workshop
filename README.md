# Arduino and Web Servers

## Prerequisites

### Hardware
- Arduino Nano IOT
- A few 10k Reistors
- a few LEDs
- a phototransistor or other analog sensors
- a few push buttons
- Wifi connections

### Software
- Arduino IDE
- VScode (or another code editor)
- Node.js installed
- Install [ngrok](https://www.npmjs.com/package/ngrok) (`npm install ngrok`)

## Reading Data from a Server

## Pushing Data to a Server

for testing with ngrok...
start your local server with node.
in a another terminal window, run `ngrok http 3000 --scheme=http`


flow
- brainstorm, diagram, write psuedo code
  - what sort of stuff is the arduino good at?
  - what sort of stuff have we seen webservers used for?
  - what sort of things could we do with by connecting the two?
  A: reading sensor data
  A: controlling outputs remotely

  Testing with `curl`
  Getting the led state
  curl  http://localhost:3000/led
   
  Toggle the led state
  curl -X POST http://localhost:3000/led

  Getting the light readings
  curl  http://localhost:3000/data
