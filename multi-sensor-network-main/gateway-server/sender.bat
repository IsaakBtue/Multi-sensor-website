@echo off
for /l %%i in (1,1,25) do (
  curl -s -X POST http://localhost:3000/ingest -H "Content-Type: application/json" ^
    -d "{\"sensor_id\":\"roomA_01\",\"timestamp\":\"2025-09-29T15:%%i:00Z\",\"temperature\":22+%%i*0.15,\"humidity\":40,\"co2\":500+%%i*18,\"battery\":25}"
  timeout /t 2 >nul
)

