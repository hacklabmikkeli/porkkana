#include <esp_now.h>
#include <WiFi.h>
#define DEEP_SLEEP_TIME 1
uint8_t broadcastAddress[] = {0x30, 0xAE, 0xA4, 0x9A, 0xEA, 0xC0};

typedef struct struct_message {
  float soil;
  float humidity;
  float temp;
} struct_message;

struct_message data;

esp_now_peer_info_t peerInfo;

// Data sent callback
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void sendData() {
  data.soil = random(1, 500) / 100.0;
  data.humidity = random(1, 500) / 100.0;
  data.temp = random(1, 500) / 100.0;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}

void setup() {
  //Initialise ESP
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  //ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  // Initialise peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  sendData();

  uint64_t u64SleepLength = DEEP_SLEEP_TIME * 60L * 1000000L;
  esp_sleep_enable_timer_wakeup(u64SleepLength);
  Serial.println("Going to sleep");
  Serial.flush();
  esp_deep_sleep_start();
}


void loop() {
}
