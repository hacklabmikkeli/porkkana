#include "WiFi.h"
#include <esp_now.h>
#include <ESP_Google_Sheet_Client.h>

#define CLIENT_EMAIL "porkana@"

const char* ssid = "SSID";
const char* password = "verySecurePassword123!";

const char spreadsheetId[] = "sheet id yes";
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\n-----END PRIVATE KEY-----\n";

typedef struct struct_message {
  float soil;
  float humidity;
  float temp;
} struct_message;

struct_message data;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&data, incomingData, sizeof(data));
  int soilHum = map(data.soil, 239, 595, 100, 0);
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Time: ");
  Serial.println(millis());
  Serial.print("Soil: ");
  Serial.println(soilHum);
  Serial.print("Humidity: ");
  Serial.println(data.humidity);
  Serial.print("Temperature: ");
  Serial.println(data.temp);
  Serial.println();
  epochTime = getTime();
  bool ready = GSheet.ready();
  if(ready) {
    FirebaseJson response;
    FirebaseJson valueRange;
    valueRange.add("majorDimension", "COLUMNS");

    valueRange.set("values/[0]/[0]", epochTime+7200); //adds 2hours to time, GMT+2
    valueRange.set("values/[1]/[0]", data.soil);
    valueRange.set("values/[2]/[0]", data.humidity);
    valueRange.set("values/[3]/[0]", data.temp);

    bool success = GSheet.values.append(&response /* returned response */, spreadsheetId /* spreadsheet Id to append */, "Sheet1!A1" /* range to append */, &valueRange /* data range to append */);
    if (success){
      response.toString(Serial, true);
      valueRange.clear();
    }
    else{
      Serial.println(GSheet.errorReason());
    }
  }
}

void setup() {
  //Initialise ESP
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());

  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_AP_STA);
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());

  //ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // put your main code here, to run repeatedly:

}
