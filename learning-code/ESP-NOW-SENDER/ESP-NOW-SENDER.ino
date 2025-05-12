#include <esp_now.h>
#include <WiFi.h>

// following example from https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/

// MAC address of the receiver board
const uint8_t MAC_RX[] = { 0x34, 0x5f, 0x45, 0x25, 0x20 0x44 };

// message data structure must match between TX'er and RX'er
typedef struct struct_message
{
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

struct_message message;

esp_now_peer_info peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  if (status = ESP_NOW_SEND_SUCCESS)
    println("Delivery Success :)");
  else
    println("Delivery Failed :(");
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA)

    if (esp_now_init() != ESP_OK)
  {
    Serial.println("ERROR: failed to initialize ESP-NOW :(");
    return;
  }

  // set the callback (cb) function as OnDataSent()
  esp_now_register_send_cb(OnDataSent);

  // setting up where to send data to
  memcpy(peerInfo.peer_addr, MAC_RX, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("ERROR: failed to add peer :(");
      return;
    }
}

void loop()
{
  strcpy(message.a, "DATADATADATA");
  message.b = random(1, 20);
  message.c = 1.2;
  message.d = false;

  esp_err_t result = esp_now_send(MAC_RX, &message, sizeof(message));
  if (result == ESP_OK)
    Serial.println("Message sent succesfully :D");
  else
    Serial.println("ERROR: message failed to send D:");

  delay(2000);  // send every 2 seconds
}
