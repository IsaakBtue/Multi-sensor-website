#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

#include "espnow_comm.h" //esp-now communication


void setup() {
    Serial.begin(115200);
    ESPNOWSetup(); 
    // This handles the esp-now communication with stations
    // Currently, this gateway only receives and prints the data  
    // We want to modify it later to forward the data to the server
    // We will likely create instances for each connected station to handle data accordingly
    // Sensor class to manage sensor instances by MAC address
    }

void loop() {
  
}
