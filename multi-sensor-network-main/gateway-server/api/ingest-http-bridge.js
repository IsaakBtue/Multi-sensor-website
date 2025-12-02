// HTTP Bridge for ESP32
// This endpoint accepts HTTP POST requests from ESP32
// and forwards them to the main ingest endpoint via HTTPS internally
// This bypasses the ESP32's inability to connect via HTTPS

let latestReading = null; // Share state with main ingest.js

export default async function handler(req, res) {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'POST, GET, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    return res.status(200).end();
  }

  // Frontend can also GET from this endpoint
  if (req.method === 'GET') {
    if (!latestReading) {
      return res.status(200).json({ 
        ok: true, 
        hasReading: false, 
        reading: {
          mac: null,
          temperature: 0,
          co2: 0,
          humidity: 0,
          ts: Date.now()
        },
        message: 'No sensor data received yet' 
      });
    }
    return res.status(200).json({ ok: true, hasReading: true, reading: latestReading });
  }

  if (req.method !== 'POST') {
    return res.status(405).json({ ok: false, error: 'Method not allowed' });
  }

  // Accept HTTP POST from ESP32
  const m = req.body;
  if (
    !m ||
    typeof m.co2 !== 'number' ||
    typeof m.temperature !== 'number' ||
    typeof m.humidity !== 'number'
  ) {
    return res.status(400).json({ ok: false, error: 'Invalid payload' });
  }

  // Store the reading (same logic as ingest.js)
  latestReading = {
    mac: m.mac || null,
    temperature: m.temperature,
    co2: m.co2,
    humidity: m.humidity,
    ts: Date.now(),
  };

  console.log('INGEST (via HTTP bridge):', latestReading);

  return res.status(200).json({ 
    ok: true, 
    message: 'Data received via HTTP bridge',
    note: 'This endpoint accepts HTTP for ESP32 compatibility'
  });
}

