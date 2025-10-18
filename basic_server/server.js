import express from 'express';
import { Low } from 'lowdb';
import { JSONFile } from 'lowdb/node';

const app = express();
const adapter = new JSONFile('db.json');
const db = new Low(adapter, { readings: [] }); // Changed from 'items' to 'readings'

app.use(express.json());

// Initialize db
await db.read();

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

const PORT = 3000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});


