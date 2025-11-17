#include <Arduino.h>
#pragma once
// Example message structure
typedef struct mymsg {
  int a;
  float b;
  char c;
} mymsg;

// Message structure for sensor values
typedef struct sensor_msg {
  float temperature;
  uint16_t co2;
  float humidity;
} sensor_msg;

// Structs for RSSI
typedef struct {
  uint8_t frame_ctrl[2];
  uint8_t duration_id[2];
  uint8_t addr1[6];  // Receiver address
  uint8_t addr2[6];  // Transmitter address
  uint8_t addr3[6];  // Filtering address
  uint8_t seq_ctrl[2];
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[];
} wifi_ieee80211_packet_t;