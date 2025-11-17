#include <Arduino.h>
#pragma once

// SCL pin for I2C: GPIO 22
// SDA pin for I2C: GPIO 21
#define MEASUREMENT_INTERVAL 60  // Interval between measurements in seconds (minimum is 2s, default is 5s)

#define LED_PIN GPIO_NUM_2      // GPIO pin for onboard indicator LED

#define FAN_PIN 20              // Pin where fan is connected to (only reqyured if fan is used)

#define FAN_DURATION 3          // Duration to run the fan in seconds (only relevant if fan is used). 1-5 seconds should be sufficient to clear stale air

#define NUM_STATIONS 10         // Number of stations objects that can be "connected" to gateway

bool useFan = false;             // Set to true if fan is used, false otherwise

bool useOnboardLED = true;      // Set to true if onboard LED is used, false otherwise (save power if not used)
