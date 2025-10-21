# Arduino and Web Servers

## Prerequisites

### Hardware
- Arduino Nano IOT
- A few 10k Reistors
- a few LEDs
- a phototransistor or other analog sensors (force sensor, potentiometer, etc)
- a few push buttons
- Wifi connection

### Software
- VScode (or another code editor)
- Node.js installed
- Arduino IDE
- Arduino Libraries: `WiFiNINA`, `ArduinoJson`

### Infastructure
- Render account for hosting your server.

## Some Helpful Utilities 
Maybe you want to test your server routes without a frontend interface. There are a few tools to do this.

### curl
`curl` is a terminal command to do internet things from the terminal. We can use to test our routes without the web browser.

- Getting the led state: `curl  http://localhost:3000/led`
- Toggle the led state: `curl -X POST http://localhost:3000/led`
- Getting the light readings: `curl  http://localhost:3000/data`
- Test adding data to the server: `curl -X POST http://localhost:3000/data -H "Content-Type: application/json" -d '{"sensor_reading": 13}'`

### Insomnia
If you prefer a GUI or need some more features you can use a application called [insomnia](https://insomnia.rest/download).
This allows you to do different types of requests to servers and apis.

Both of these tools are super helpful outside the context of Arduino! I use them a lot for testing/debugging.

## Notes on wifiNINA


### Advanced
When testing, Arduino can't connect to `localhost:3000` but you can install [ngrok](https://www.npmjs.com/package/ngrok) (`npm install ngrok`), this allows you to pass through internet traffic to your local machine easily. 
Ngrok is used to easily forward web traffic from a url hosted on their server to your local machine. Now any arduino with the ngrok address can access your local server while ngrok is running. 
This way you can test out your server with arduino before pushing your server to Render.

- Start your local server with node.
- In a another terminal window, run `ngrok http 3000 --scheme=http`
- Copy the ngrok url.
- In Arduino, uncomment the http only client and replace the url with the ngrok url.
- Now your arduino can connect to to the server running on `localhost`
- In the browser you can see changes at `localhost:3000`
 

