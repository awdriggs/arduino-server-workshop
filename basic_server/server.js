import express from 'express';
import { Low } from 'lowdb';
import { JSONFile } from 'lowdb/node';

//express business
const app = express();
app.use(express.json());
app.use(express.static('public'))

//database business 
// object to give lowdb to start, aka a schema 
const data = {
  readings: [],
  lightState: false, 
}

const adapter = new JSONFile('db.json');
const db = new Low(adapter, data); // Changed from 'items' to 'readings'

// Initialize db
await db.read();

// Routes  

// example one, server holds led state (on or off).
// web interface for changing the state found in public/index.html
// arduino polls the server every 10 seconds to see if the state has changed and turns on/off an led accordinly.

// GET route - see what the current led status is.
app.get('/led', async (req, res) => {
  await db.read();
  res.json({ lightState: db.data.lightState });
});

// POST route - change the led state 
app.post('/led', async (req, res) => {
  console.log('toggle led state');

  await db.read();
  db.data.lightState = !db.data.lightState;
  await db.write();

  res.json({ lightState: db.data.lightState });
});

// example two, arduino sends data to the server
// web interface for seeing the readings

// GET route - retrieve all readings
app.get('/data', async (req, res) => {
  await db.read();
  res.json(db.data.readings);
});

// POST route - add a reading to the readings array
app.post('/data', async (req, res) => {
  console.log('Received:', req.body);

  await db.read();
  db.data.readings.push(req.body);
  await db.write();
  res.status(201).json(req.body);
});

// start it bro!
const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});


