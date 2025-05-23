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

const char *startFilePath = "/";
const char *ext = "wav";
AudioSourceSD source(startFilePath, ext, PIN_AUDIO_KIT_SD_CARD_CS);
AudioBoardStream kit(AudioKitEs8388V1);
WAVDecoder decoder;
AudioPlayer player(source, kit, decoder);

unsigned int samplePeriod = 30000;
unsigned const int sampleInterval = 100;
unsigned int maxSamples;
// these next two will be empty arrays, hence they are int*
unsigned int* heartValues;
unsigned int heartAverage;
unsigned int* forceValues;
unsigned int forceAverage;
unsigned int sampleCount = 0;
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

  /* these print statements were for debugging
  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("Heart: ");
  Serial.println(message.heartData);

  Serial.print("Force: ");
  Serial.println(message.forceData);

  Serial.println();
  */
}

void sampleData()
{
  // get new sample?
  if (sampleCount < maxSamples && millis() - startTime >= sampleCount * sampleInterval)
  {
    // sometimes the sensors will just read bad data. so i want to filter it out
    // surely no one's heartrate should be below 50bpm or over 150bpm right?
    if (heart > 50 && heart < 150 && force != 0)
    {
      heartValues[sampleCount] = heart;
      forceValues[sampleCount] = force;
      sampleCount++;
    }

    else
    {
      Serial.print("Bad sample data skipped! ->\t");
      Serial.print("Heart data:\t");
      Serial.print(heart);
      Serial.print(" | Force data:\t");
      Serial.println(force);
      // increase the sampling window to ensure all needed samples are collected
      samplePeriod += sampleInterval;
    }
  }

  // done getting samples
  if (sampleCount > 0 && sampleCount >= maxSamples)
  {
    unsigned long heartSum = 0;
    unsigned long forceSum = 0;

    for (int i = 0; i < sampleCount; i++)
    {
      heartSum += heartValues[i];
      forceSum += forceValues[i];
    }

    heartAverage = heartSum / sampleCount;
    forceAverage = forceSum / sampleCount;

    Serial.println("Sampling complete!");
    Serial.print("Average heart value: ");
    Serial.println(heartAverage);
    Serial.print("Average force value: ");
    Serial.println(forceAverage);

    sampleCount++;
  }
}

// i couldn't find anything in the Audio-Tools library to play a file and then stop once it's done, so i made my own function for it
void playAudio(const char* filePath)
{

  player.setPath(filePath);

  Serial.print("Playing "); // file select
  Serial.println(filePath);

  player.play();
  while (player.isActive())
  {
    player.copy();

    // if the audio track has stopped, deactivate the AudioPlayer, letting the program to continue
    if (!player.copy())
    {
      player.stop();
    }
  }

  Serial.print(filePath);
  Serial.println(" has finished playing.");
}

void calculateAverages()
{
  startTime = millis();
  Serial.print("Sampling start time: ");
  Serial.println(startTime);

  Serial.println("Sampling...");
  while (sampleCount <= maxSamples)
  {
    sampleData();

    if (millis() - startTime >= 30000) // if 30 seconds have passed
    {
      Serial.println("Sampling took too long, skipping stage.");
      break;
    }
  }
}

void playAverageAudio(unsigned int avg)
{
  playAudio("/average.wav");

  if (avg < 50)
  {
    playAudio("/unable-calculate.wav");
    return;
  }

  if (avg >= 100)
  {
    playAudio("/over-100.wav");
    return;
  }
  else if (avg >= 90)
  {
    playAudio("/90.wav");
  }
  else if (avg >= 80)
  {
    playAudio("/80.wav");
  }
  else if (avg >= 70)
  {
    playAudio("/70.wav");
  }
  else if (avg >= 60)
  {
    playAudio("/60.wav");
  }
  else if (avg >= 50)
  {
    playAudio("/50.wav");
  }

  if (avg % 10 == 9)
  {
    playAudio("/9.wav");
  }
  else if (avg % 10 == 8)
  {
    playAudio("/8.wav");
  }
  else if (avg % 10 == 7)
  {
    playAudio("/7.wav");
  }
  else if (avg % 10 == 6)
  {
    playAudio("/6.wav");
  }
  else if (avg % 10 == 5)
  {
    playAudio("/5.wav");
  }
  else if (avg % 10 == 4)
  {
    playAudio("/4.wav");
  }
  else if (avg % 10 == 3)
  {
    playAudio("/3.wav");
  }
  else if (avg % 10 == 2)
  {
    playAudio("/2.wav");
  }
  else if (avg % 10 == 1)
  {
    playAudio("/1.wav");
  }
}

void setup()
{
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);

  auto cfg = kit.defaultConfig(TX_MODE);
  cfg.sd_active = true;
  kit.begin(cfg);

  // TODO find a way to change the volume without having to recompile bc it's so annoying LOL
  player.setVolume(0.2);
  player.begin();

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // message receive callback function = onDataRX()
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRX));

  maxSamples = samplePeriod / sampleInterval;
  heartValues = new unsigned int[maxSamples];
  forceValues = new unsigned int[maxSamples];
}

void loop()
{
  playAudio("/startup.wav");

  calculateAverages();

  playAverageAudio(heartAverage);

  // TODO the actual algorithm for calculating mood. for now, we'll assume "stressed" as the mood
  // everyone's kinda stressed anyway, right?

  playAudio("/stressed.wav");

  playAudio("/goodbye.wav");

  // do nothing once everything is over
  Serial.println("Process complete. Standing by....");
  while (true) {}

  /*
    How I want to structure this:
    2) wait 30 seconds to gather average heart rate and force on the sensor - DONE
    3) then play the mood that it "detects" with the force sensor parameter affecting the audio - DONE kinda
  */
}