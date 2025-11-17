#include <Arduino.h>
#include "SDA41_sensor.h" 
#include "esp32-hal-gpio.h" // Needed for the specific low-level GPIO functions
#include <driver/rtc_io.h>  
#include <esp_sleep.h> 
#include "config.h"

// Header files for esp now communication
#include "espnow_comm.h"

// Define a variable that retains its value across Deep Sleep cycles
RTC_DATA_ATTR int system_state = 0;

// States
const int STATE_INITIAL_BOOT = 0;
const int STATE_START_MEASURE = 1;
const int STATE_READ_VALUE = 2;

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to sec
#define LED_PIN GPIO_NUM_2             // GPIO pin for the LED
#define MEASUREMENT_DURATION 6

uint16_t LONG_SLEEP; 
uint16_t SHORT_SLEEP;
uint8_t cur_time = 0;
uint16_t AWAKE_TIME = 0;

void setMeasuremntIntervals() {
  if(useFan) {
  LONG_SLEEP = MEASUREMENT_INTERVAL - MEASUREMENT_DURATION - FAN_DURATION;
} else {
  LONG_SLEEP = MEASUREMENT_INTERVAL - MEASUREMENT_DURATION;
}
}

void setup() {
  // Turn on indicator LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); 
  setMeasuremntIntervals();

  uint8_t start_time = millis();

  Serial.begin(115200);

  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  // Initialize stuff
  ESPNOWSetup();
  initSensor();
  addBroadcastPeer();

  if (wakeup_reason != ESP_SLEEP_WAKEUP_TIMER) {
        // This is a first boot or reset
        system_state = STATE_START_MEASURE;
    }
  
  switch (system_state) {
        case STATE_START_MEASURE:
            // We woke up from long sleep
            if (useFan) {
              pinMode(FAN_PIN, OUTPUT);
              digitalWrite(FAN_PIN, HIGH);   // Turn on fan
              Serial.println("Fan turned on");

              esp_sleep_enable_timer_wakeup(FAN_DURATION * uS_TO_S_FACTOR);
              Serial.printf("Fan will run for %d seconds (light sleep)\n", FAN_DURATION);

              // Enter light sleep
              esp_light_sleep_start();

              // Wake up here
              digitalWrite(FAN_PIN, LOW);    // Turn off fan
              Serial.println("Fan turned off after light sleep");
            }
            
            // Start single shot measurement
            Serial.println("Starting measurement and short sleep...");
            error = sensor.startPeriodicMeasurement(); // Start the measurement process (non blocking)
            if (error != NO_ERROR) {
              Serial.print("Error trying to execute measureSingleShot(): ");
              errorToString(error, errorMessage, sizeof errorMessage);
              Serial.println(errorMessage);
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
            esp_deep_sleep_start();
            break;

        case STATE_READ_VALUE:
            // we woke up from short sleep
            // Time to read the value and send it
            Serial.println("Woke up from short sleep. Reading value and sending...");
            msg = readSDA41(); // This uses sensor.readMeasurement()

            esp_now_send(broadcastAddr, (uint8_t *)&msg, sizeof(msg));
  
            // Wait for send to complete
            while (!send_done) {
              delay(1);
            }
            // Set the next state to start a new cycle
            system_state = STATE_START_MEASURE; 

            digitalWrite(LED_PIN, LOW);

            cur_time = millis();
            Serial.printf("Awake for %d milliseconds\n", (cur_time - start_time));
            // Go to sleep for 54.5 seconds (the long sleep)
            // Total time should be 60s - 5.5s = 54.5s
            esp_sleep_enable_timer_wakeup(LONG_SLEEP * uS_TO_S_FACTOR); 
            esp_deep_sleep_start();
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
}
