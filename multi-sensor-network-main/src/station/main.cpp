#include <Arduino.h>
#include "SDA41_sensor.h" 
#include "esp32-hal-gpio.h" // Needed for the specific low-level GPIO functions
#include <driver/rtc_io.h>  
#include <esp_sleep.h> 
#include "config.h"  // Where information is stored about constants, e.g. fan duration etc.
#include <Wire.h>

// Header files for esp now communication
#include "espnow_comm.h"

// Define a variable that retains its value across Deep Sleep cycles
RTC_DATA_ATTR int system_state = 0;


// Calibration constants that must stay in memory!
RTC_DATA_ATTR int cali_counter = 1; // iteration counter for calibration; counts from 1. 

// States
const int STATE_INITIAL_BOOT = 0;
const int STATE_START_MEASURE = 1;
const int STATE_READ_VALUE = 2;
const int STATE_CALI_ROUTINE = 3;


// Define statements help you save time when writing. These are essentially pointers that make the compiler replace
// what you wrote with what you assign it - it will replace LED_PIN with GPIO_Num_2 each time you run the code.
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to sec
#define LED_PIN GPIO_NUM_2             // GPIO pin for the LED
#define MEASUREMENT_DURATION 5  // Time needed for sensor measurement (seconds)



uint16_t LONG_SLEEP;      
uint8_t SHORT_SLEEP = 5; // seconds to wait until measurement is back
uint8_t cur_time = 0;     // shorter length to save memory
uint16_t AWAKE_TIME = 0;

void setMeasuremntIntervals() {
  if(useFan) {
  LONG_SLEEP = MEASUREMENT_INTERVAL - MEASUREMENT_DURATION - FAN_DURATION;
} else {
  LONG_SLEEP = MEASUREMENT_INTERVAL - MEASUREMENT_DURATION;
}
} 

void checkCalibration() {
  /*
   Function checks if the calibration variable is such that we can trigger calibration later.
    The variable this changes (needCalibration) is defined in config.h
    Logic is written expressly, such that each time this command is run, this is updated.
  */

  if(cali_counter >= CALI_PERIOD){    // condition leq due to error handling.
    Serial.println("Calibration boundary reached. Sensor will calibrate this cycle.");
    needCalibration = true;
    system_state = STATE_CALI_ROUTINE;
  }
  else {    // this else statement should catch errors, if this does not trigger, will be evident from serial
    needCalibration = false;
  }

}

void setup() {
  // Turn on indicator LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); 
  setMeasuremntIntervals();

  uint8_t start_time = millis();

  Serial.begin(115200);
  delay(1000); // Give serial time to initialize
  
  Serial.println("\n========================================");
  Serial.println("=== ESP32 Station Starting ===");
  Serial.println("========================================\n");
  Serial.printf("Measurement interval: %d seconds\n", MEASUREMENT_INTERVAL);
  Serial.printf("Short sleep (measurement wait): %d seconds\n", SHORT_SLEEP);
  Serial.printf("Long sleep (between measurements): %d seconds\n", LONG_SLEEP);
  Serial.println();

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    Serial.print("Boot Reason: ");
    if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("Timer Wakeup (OK)");
    } else if (wakeup_reason == ESP_SLEEP_WAKEUP_UNDEFINED) {
        Serial.println("Reset/Crash (OR post-calibration restart!)");
    } else {
        Serial.printf("Other Wakeup Reason: %d\n", wakeup_reason);
    }

  // Initialize stuff
  ESPNOWSetup();
  initSensor();
  addBroadcastPeer();
  checkCalibration();   // check current iteration if we need to do calibration

  switch (system_state) {  // switch case sytax note; you check the () variable against if it equals what is after case _____;
        case STATE_START_MEASURE:

            // We woke up from long sleep OR first boot
            if (useFan) {
              pinMode(FAN_PIN, OUTPUT);
              digitalWrite(FAN_PIN, HIGH);   // Turn on fan
              Serial.println("Fan turned on");

              esp_sleep_enable_timer_wakeup(FAN_DURATION * uS_TO_S_FACTOR);   // set sleep duration
              Serial.printf("Fan will run for %d seconds (light sleep)\n", FAN_DURATION);

              // Enter light sleep
              esp_light_sleep_start();    

              // Wake up here
              digitalWrite(FAN_PIN, LOW);    // Turn off fan
              Serial.println("Fan turned off after light sleep");
            }

          //   // check iteration - enforce sensor is already on (testing)
          //   if (cali_counter == 1) {  // only run on initialisation & when cali_counter is reset.            
          //     Serial.println("Sensor will be forced awake");
          //     sensor.wakeUp();
          //     delay(500);
          // }

            // Start periodic measurement
            Serial.println("Starting sensor measurement...");
            error = sensor.startPeriodicMeasurement(); // Start the measurement process (non blocking)
            if (error != NO_ERROR) {
              Serial.print("Error trying to execute startPeriodicMeasurement(): ");
              errorToString(error, errorMessage, sizeof errorMessage);
              Serial.println(errorMessage);
            } else {
              Serial.println("Sensor measurement started successfully");
            }

            // Save state in RTC memory
            system_state = STATE_READ_VALUE;
            
            // Turn off indicator LED
            digitalWrite(LED_PIN, LOW);
            
            // Note down the time
            cur_time = millis();
            Serial.printf("Awake for %d milliseconds\n", (cur_time - start_time));

            // Go to sleep (the short sleep)
            esp_sleep_enable_timer_wakeup(SHORT_SLEEP * uS_TO_S_FACTOR);
            Serial.printf("timer set going to bed(short)"); 
            esp_deep_sleep_start();
            break;

        case STATE_READ_VALUE:
            // we woke up from short sleep
            // Time to read the value and send it
            Serial.println("\n=== Reading sensor data ===");
            msg = readSDA41(); // This uses sensor.readMeasurement()
            
            Serial.printf("Sensor readings - Temp: %.2f°C, CO2: %d ppm, Humidity: %.2f%%\n", 
                         msg.temperature, msg.co2, msg.humidity);

            // Reset send_done flag
            send_done = false;
            
            Serial.println("Sending data via ESP-NOW...");
            esp_err_t result = esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
            
            if (result == ESP_OK) {
              Serial.println("ESP-NOW send initiated successfully");
            } else {
              Serial.printf("ESP-NOW send failed with error: %d\n", result);
            }
  
            // Wait for send to complete (with timeout)
            unsigned long sendTimeout = millis() + 2000; // 2 second timeout
            while (!send_done && millis() < sendTimeout) {
              delay(10);
            }
            
            if (send_done) {
              Serial.println("ESP-NOW data sent successfully!");
            } else {
              Serial.println("WARNING: ESP-NOW send timeout");
            }
            
            // Set the next state to start a new cycle
            system_state = STATE_START_MEASURE; 

            digitalWrite(LED_PIN, LOW);

            cur_time = millis();
            Serial.printf("Total awake time: %d milliseconds\n", (cur_time - start_time));
            Serial.printf("Sleeping for %d seconds until next measurement...\n", LONG_SLEEP);

            // Calibration counter tracking
            cali_counter++; // adds one to previous value of counter
            Serial.printf("Calibration counter: %d / %d\n", cali_counter, CALI_PERIOD);

            // enable sleep for the remaining time to complete 10 second interval
            esp_sleep_enable_timer_wakeup(LONG_SLEEP * uS_TO_S_FACTOR); 
            Serial.println("Entering deep sleep...\n"); 
            esp_deep_sleep_start();
            break;

        case STATE_CALI_ROUTINE:
            
            error = sensor.setAutomaticSelfCalibrationInitialPeriod(0);  // See Inspirion docs, 0 forces IMMEDIATE recalibration!
            if (error == NO_ERROR) {
              Serial.println("Sensor forced automatic Cal. was a success");
              }
            else {
              Serial.println("Error encountered during calibration:  ");
              errorToString(error, errorMessage, sizeof errorMessage);
              Serial.println(errorMessage);;
              }
            
            error = sensor.setAutomaticSelfCalibrationInitialPeriod(44);  // See Inspirion docs, reset to default value.
            if (error == NO_ERROR) {
              Serial.println("Sensor automatic Cal reset was a success");
              }
            else {
              Serial.println("Error encountered during calibration:  ");
              errorToString(error, errorMessage, sizeof errorMessage);
              Serial.println(errorMessage);;
              }

            if (error == NO_ERROR) {   // is the local error state that everything is fine? Then, restart.
              system_state = STATE_START_MEASURE; //reboot
              cali_counter = 1;
              Serial.println("Device has reset state to measurement state, continuing to reboot");
              esp_restart();
            }
              else {
                Serial.println("Post-Calibration restart failed, due to previous errors.");
              }
          break;
            
        default:
            // Fallback
            Serial.println("Unknown state. Resetting cycle...");
            system_state = STATE_START_MEASURE;
            esp_sleep_enable_timer_wakeup(1000000ULL);
            esp_deep_sleep_start();
            break;
    }

}



void loop() {
  // Don't put shit here bc loop is never reached :)
  // Deep sleep reboots the entire device, thereby avoiding loop by shutting off in setup
}
