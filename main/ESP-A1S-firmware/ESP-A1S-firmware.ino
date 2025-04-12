#include <esp_now.h>
#include <WiFi.h>

/*
//
// Project Alexithymia by Julian Castellanos
// This board receives analog input data from the ESP32-WROOM-32 module to then use for the music
// 
*/

// message data structure must match between TX'er and RX'er
typedef struct struct_message
{
  int heartData;
  int forceData;
} struct_message;

struct_message message;

// callback function that will be executed when data is received
void OnDataRX(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&message, incomingData, sizeof(message));
  
  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("Heart: ");
  Serial.println(message.heartData);

  Serial.print("Force: ");
  Serial.println(message.forceData);

  Serial.println();
}

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
    {
      Serial.println("Error initializing ESP-NOW");
      return;
    }

  // message receive callback function = onDataRX()
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRX));
}

void loop()
{
}
