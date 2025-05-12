#include <esp_now.h>
#include <WiFi.h>
#include <PulseSensorPlayground.h>

/*
//
// Project Alexithymia by Julian Castellanos
// This board will send analog input from the FSR and ECG to the ESP32-A1S
//
*/

// pin definitions
#define forceSensor 35
#define heartSensor 34
#define led 2
#define threshold 550

int heartSignal;
int heartData;
int forceData;

// MAC address of the receiver board
const uint8_t MAC_RX[] = { 0x34, 0x5f, 0x45, 0x25, 0x20, 0x44 };

// message data structure must match between TX'er and RX'er
typedef struct struct_message
{
  int heartData;
  int forceData;
} struct_message;

struct_message message;

esp_now_peer_info peerInfo;

PulseSensorPlayground pulseSensor;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status\t");
  if (status = ESP_NOW_SEND_SUCCESS)
    Serial.println("Delivery Success :)");
  else
    Serial.println("Delivery Failed :(");
}

void setup()
{
  pinMode(forceSensor, INPUT);
  pinMode(heartSensor, INPUT);

  pulseSensor.analogInput(heartSensor);
  pulseSensor.blinkOnPulse(led);
  pulseSensor.setThreshold(threshold);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

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
  if (pulseSensor.begin())
      Serial.println("pulseSensor object created");
}

void loop()
{
  heartSignal = analogRead(heartSensor);
  Serial.print("Signal:");
  Serial.print(heartSignal);

  if (pulseSensor.sawStartOfBeat())
    heartData = pulseSensor.getBeatsPerMinute();
  
  forceData = analogRead(forceSensor);

  Serial.print("\tHeart:");
  Serial.print(heartData);

  Serial.print("\tForce:");
  Serial.println(forceData);

  message.heartData = heartData;
  message.forceData = forceData;

  esp_err_t result = esp_now_send(MAC_RX, (uint8_t *)&message, sizeof(message));
  if (result == ESP_OK)
    Serial.println("Message sent succesfully :D");
  else
    Serial.println("ERROR: message failed to send D:");

  delay(50);  // send every 50ms
}
