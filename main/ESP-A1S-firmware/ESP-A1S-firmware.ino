#include <esp_now.h>
#include <WiFi.h>
#include "AudioTools.h"
#include "AudioTools/AudioLibs/AudioBoardStream.h"
#include "AudioTools/Disk/AudioSourceSD.h"
#include "AudioTools/AudioCodecs/CodecWAV.h"

/*
//
// Project Alexithymia by Julian Castellanos
// This board receives analog input data from the ESP32-WROOM-32 module to then use for the music
// For the ARISE program with Undergraduate Research @ Dallas College (UR@DC)
// 
*/

int heart = 0;
int force = 0;
float heartAvg = 0.0f;
float forceAvg = 0.0f;

const char *startFilePath = "/";
const char *ext = "mp3";
AudioSourceSD source(startFilePath, ext, PIN_AUDIO_KIT_SD_CARD_CS);
AudioBoardStream kit(AudioKitEs8388V1);
WAVDecoder decoder;
AudioPlayer player(source, kit, decoder);

const unsigned long samplePeriod = 30000;
const int sampleInterval = 100;
const int maxSamples = samplePeriod / sampleInterval;
int heartValues[maxSamples];
int forceValues[maxSamples];
int sampleCount = 0;
unsigned long startTime;

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
  heart = message.heartData;
  force = message.forceData;
  /*
  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("Heart: ");
  Serial.println(message.heartData);

  Serial.print("Force: ");
  Serial.println(message.forceData);

  Serial.println();
  */
}

void calculateAvg()
{
  if (millis() - startTime < samplePeriod)
    {
      // get new sample?
      if (sampleCount < maxSamples && millis() - startTime >= sampleCount * sampleInterval)
        {
          heartValues[sampleCount] = heart;
          forceValues[sampleCount] = force;

          Serial.print("Heart sample ");
          Serial.print(sampleCount + 1);
          Serial.print(": ");
          Serial.println(heartValues[sampleCount]);

          Serial.print("Force sample ");
          Serial.print(sampleCount + 1);
          Serial.print(": ");
          Serial.println(forceValues[sampleCount]);

          sampleCount++;
        }
    }

  // done getting samples
  else if (sampleCount > 0 && sampleCount >= maxSamples)
    {
      long heartSum = 0;
      long forceSum = 0;

      for (int i = 0; i < sampleCount; i++)
        {
          heartSum += heartValues[i];
          forceSum += forceValues[i];
        }
      
      int heartAverage = heartSum / sampleCount;
      int forceAverage = forceSum / sampleCount;

      Serial.println("Sampling complete!");
      Serial.print("Average heart value: ");
      Serial.println(heartAverage);
      Serial.print("Average force value: ");
      Serial.println(forceAverage);

      sampleCount++;
    }
}

void setup()
{
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  auto cfg = kit.defaultConfig(TX_MODE);
  cfg.sd_active = true;
  kit.begin(cfg);

  player.setVolume(0.5);
  player.begin();

  // file select
  player.setPath("/activated.wav");


  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
    {
      Serial.println("Error initializing ESP-NOW");
      return;
    }

  // message receive callback function = onDataRX()
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRX));

  startTime = millis();
}

void loop()
{
  while (sampleCount <= maxSamples)
    {
      calculateAvg();
    }
  
  while (player.isActive())
    {
      player.copy();
    }
  
  /*
    How I want to structure this:
    2) wait 30 seconds to gather average heart rate and force on the sensor
    3) then play the mood that it "detects" with the force sensor parameter affecting the audio
  */
}