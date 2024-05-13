#include "WiFi.h"
#include <esp_now.h>

typedef struct struct_message {
  float soil;
  float humidity;
  float temp;
} struct_message;

struct_message data;

void setup() {
  //Initialise ESP
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  //ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Time: ");
  Serial.println(millis());
  Serial.print("Soil: ");
  Serial.println(data.soil);
  Serial.print("Humidity: ");
  Serial.println(data.humidity);
  Serial.print("Temperature: ");
  Serial.println(data.temp);
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:

}
