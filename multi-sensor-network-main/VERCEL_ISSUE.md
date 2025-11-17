# Vercel Deployment Issue - Expected Behavior

## Why You're Seeing the 404 Error

The `404: NOT_FOUND` error from Vercel is **expected and normal** for this project. Here's why:

### This Project is NOT Meant for Vercel

This is an **ESP32 IoT project** designed to run **locally** on your machine. The gateway server needs to:

1. **Be on the same network** as your ESP32 devices
2. **Accept direct connections** from IoT devices
3. **Run continuously** (not as serverless functions)
4. **Have low latency** for real-time sensor data

Vercel is designed for:
- Static websites
- Serverless API functions
- Web applications that don't need persistent connections

### The Architecture

```
ESP32 Devices (Local Network)
    ↓
Gateway Server (localhost:3000) ← You are here
    ↓
Web Dashboard (http://localhost:3000)
```

**NOT:**
```
ESP32 Devices → Vercel Cloud → ❌ (Won't work!)
```

## Solution: Remove Vercel Integration

Since this project doesn't need Vercel, you should remove the integration:

### Option 1: Delete from Vercel Dashboard

1. Go to [vercel.com](https://vercel.com) and log in
2. Find your project (`multi-sensor-network` or similar)
3. Go to **Settings** → **General**
4. Scroll down and click **Delete Project**
5. Confirm deletion

### Option 2: Remove GitHub Integration

1. Go to your GitHub repository: `https://github.com/IsaakBtue/multi-sensor-network`
2. Click **Settings** → **Integrations** → **Deployments**
3. Remove any Vercel integrations

### Option 3: Add `.vercelignore` (if you want to keep Vercel for other projects)

Create a `.vercelignore` file in your project root:
```
*
```

This tells Vercel to ignore this project entirely.

## Running Locally (Correct Way)

1. **Activate conda environment:**
   ```cmd
   conda activate multi-sensor-network
   ```

2. **Navigate to gateway-server:**
   ```cmd
   cd gateway-server
   ```

3. **Start the server:**
   ```cmd
   npm start
   ```

4. **Access the dashboard:**
   - Open browser: `http://localhost:3000`
   - API status: `http://localhost:3000/api/status`
   - Ingest endpoint: `POST http://localhost:3000/ingest`
   - Events stream: `GET http://localhost:3000/events`

## If You Really Need Cloud Deployment

If you absolutely need cloud deployment (not recommended for this IoT project), you would need to:

1. **Refactor for serverless:**
   - Convert Express server to Vercel serverless functions
   - Use WebSockets via external service (not Vercel's built-in)
   - Handle persistent connections differently

2. **Use a different platform:**
   - **Railway** - Better for persistent Node.js apps
   - **Render** - Supports long-running processes
   - **DigitalOcean App Platform** - Full control over deployment

3. **Network considerations:**
   - ESP32 devices need to reach the cloud server
   - Requires public IP/domain
   - Adds latency and complexity

## Summary

✅ **Do:** Run the server locally on `localhost:3000`  
❌ **Don't:** Try to deploy to Vercel (it's not designed for this)  
✅ **Do:** Remove Vercel integration to avoid confusion  
❌ **Don't:** Worry about the 404 error - it's expected!

The local server is now configured to serve your dashboard at `http://localhost:3000/` - just restart the server and it should work!

