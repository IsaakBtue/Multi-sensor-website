#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "esp_wifi.h"

#include "typedef.h"
#include "config.h"

bool send_done = false;
// FF:FF:FF:FF:FF:FF is broadcast MAC
uint8_t broadcastAddr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };



// Station class to manage individual Stations
class Station {
public:
  uint8_t mac[6]; // MAC address
  int rssi;
  struct readings { // Store sensor readings
    float temperature;
    uint16_t co2;
    float humidity;
  } readings;

  Station(const uint8_t* mac_addr) : rssi(0) {
    memcpy(mac, mac_addr, 6);
    // Initialize other members if needed
  }

  void updateRSSI(int new_rssi) {
    rssi = new_rssi;
  }

  void handleMessage(const uint8_t* data, int len) {
    if (len == sizeof(sensor_msg)) {
      const sensor_msg* msg = (const sensor_msg*)data;
      readings.temperature = msg->temperature;
      readings.co2 = msg->co2;
      readings.humidity = msg->humidity;
      Serial.print("Message from station: ");
      for (int i = 0; i < 6; ++i) {
        Serial.printf("%02X", mac[i]);
        if (i < 5) Serial.print(":");
      }
      Serial.printf(" | Temp: %.2f, CO2: %.2f, Humidity: %.2f | RSSI: %d\n", readings.temperature, readings.co2, readings.humidity, rssi);
    } else {
      Serial.println("Invalid sensor_msg length");
    }
  }
};

// Generate Station objects 
Station* stations[NUM_STATIONS]; 
int stationCount = 0;

// Helper to find or create a Station object for a given MAC
Station* findStation(const uint8_t* mac) {
  for (int i = 0; i < stationCount; ++i) {
    if (memcmp(stations[i]->mac, mac, 6) == 0) {
      return stations[i];
    }
  }
  return NULL;
}

// Create or get the existing Station
Station* getOrCreateStation(const uint8_t* mac) {
  Station* existing = findStation(mac);
  if (existing) {
    return existing;
  }
  if (stationCount < NUM_STATIONS) {
    stations[stationCount] = new Station(mac);
    return stations[stationCount++];
  }
  return NULL; // Max stations reached
}

// Promiscuous RX callback to update RSSI for matching stations
void promiscuous_rx_cb(void *buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT)
    return;
  const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *)buf;
  const wifi_ieee80211_packet_t *ipkt = (wifi_ieee80211_packet_t *)ppkt->payload;
  const wifi_ieee80211_mac_hdr_t *hdr = &ipkt->hdr;
  Station* station = findStation(hdr->addr2);
    if (station) {
      station->updateRSSI(ppkt->rx_ctrl.rssi);
    }
}

// Callback when data is sent
void OnDataSent(const uint8_t* mac, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  send_done = true;
}

// Callback when data is received
void OnDataRecv(const esp_now_recv_info_t* recvInfo, const uint8_t* data, int len) {
  const uint8_t* mac = recvInfo->src_addr;  // Extract MAC address from recvInfo

  Station* st = getOrCreateStation(mac);
  if (st && len == sizeof(sensor_msg)) {
    st->handleMessage(data, len);
  } else {
    Serial.println("Received invalid data or too many stations");
  }
}

void addBroadcastPeer() {
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddr, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (!esp_now_is_peer_exist(broadcastAddr)) {
    esp_now_add_peer(&peerInfo);
  }
}

void ESPNOWSetup(){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed. Rebooting...");
        delay(2000);
        ESP.restart();
    }

    // Register callbacks but
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);
}