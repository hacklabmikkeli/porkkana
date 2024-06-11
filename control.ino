#include <Arduino.h>
#include "WiFi.h"
#include <esp_now.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>
// Google Project ID
#define PROJECT_ID ""

// Service Account's client email
#define CLIENT_EMAIL ""

const char* ssid = "SSID";
const char* password = "VERYsecurePaSsWoRd123!";
const char spreadsheetId[] = "";
const char PRIVATE_KEY[] PROGMEM = "";

// Token Callback function
void tokenStatusCallback(TokenInfo info);

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";

// Variable to save current epoch time
unsigned long epochTime; 

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

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
  bool ready = GSheet.ready();
  if(ready) {
    FirebaseJson response;
    FirebaseJson valueRange;
    valueRange.add("majorDimension", "COLUMNS");
    // Get timestamp
    epochTime = getTime();
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
    Serial.println(ESP.getFreeHeap());
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
  //Configure time
  configTime(0, 0, ntpServer);

  // Set the callback for Google API access token generation status (for debug only)
  GSheet.setTokenCallback(tokenStatusCallback);

  // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
  GSheet.setPrerefreshSeconds(10 * 60);

  // Begin the access token generation for Google API authentication
  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
  
  esp_now_register_recv_cb(OnDataRecv);
}
void tokenStatusCallback(TokenInfo info){
    if (info.status == token_status_error){
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
        GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
    }
    else{
        GSheet.printf("Token info: type = %s, status = %s\n", GSheet.getTokenType(info).c_str(), GSheet.getTokenStatus(info).c_str());
    }
}

void loop() {
  // put your main code here, to run repeatedly:

}
