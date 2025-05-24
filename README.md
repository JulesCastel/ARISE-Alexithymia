
# <ins>Alexithymia</ins>

*For the ARISE program with Undergraduate Research at Dallas College (UR@DC)*

Won the Most Creative Project award!

[Showcase video](https://www.youtube.com/watch?v=obvycJi4Fpg)

[Full music track](https://youtu.be/uM08ENIJbZg)

## Abstract

There is an epidemic of loneliness that our generation has been struggling with since at least the 2010s and especially during the COVID-19 pandemic lockdown. There are many factors attributed to why this is (social media, individualism, etc.), but one key factor may be that many people lack the emotional skills to even identify that they struggle with loneliness in the first place, thus are unable to begin to address the issue.

This lack of emotional skill can stem from something called <ins>alexithymia</ins>, a psychological phenomenon characterized by challenges in being able to identify one's internal emotional state.

## Introduction

I based my project on the issue of the loneliness epidemic and alexithymia in order to combat it in the way I know best—music.

My ESP32-powered device reads the user's heartrate and the force they apply to a force-sensitive resistor to determine how the user might be feeling. Then it plays a track based off the emotion that the device "calculates" using that data, with the purpose of trying to connect the user's feelings to something less ephemeral, like music.

## Materials & Methods

- ESP32-Audio-kit
- ESP32-WROOM-32
- 2x 3" speakers, 4Ω 3W
- FSR406_CM Force-sensitive resistor
- PulseSensor Amped

After a careful selection of the many ESP32 dev boards on the market, the Ai-Thinker ESP32-Audio-kit was chosen for its integrated SD card reader and audio amplifiers for use with external speakers. An additional ESP32-WROOM-32 was used to read the sensors and transmit them to the Audio-kit wirelessly using the ESP-NOW library.

The firmware for both ESP32s were written in C++ and compiled using the Arduino IDE using several 3rd party libraries, but especially [Audio-Tools by Phil Schatzmann.](https://github.com/pschatzmann/arduino-audio-tools)

Design for the wiring schematic and PCB were done in the freeware CAD program, KiCAD.

![Wiring schematic of the sensors and the ESP32-WROOM-32](/documentation/schematic.png)
*Wiring schematic of the sensors and the ESP32-WROOM-32.*
![PCB design created from the schematic](/documentation/pcb.png)
*PCB design created from the schematic.*

The 3D modeling design for printing was done in AutoDesk Fusion.

![A view of the device's 3D model in AutoDesk Fusion](/documentation/model.png)
*A view of the device's 3D model in AutoDesk Fusion.*

## Interaction

The user straps the heartrate sensor to either the left or right ring finger and places a finger on the force sensor on the top of the device. The device will start collecting samples to create an average of the data read from both sensors, and then an audio track based on the mood that is "calculated" from the data will play.

## Program flow

![The ESP32-WROOM-32 initializes ESP-NOW wireless transmission and reads sensor data. That data continuously transmits to the ESP32-Audio-kit. The ESP32-Audio-kit syncs with the ESP32-WROOM-32 and samples the data for ~10 seconds. The data from both sensors are averaged and a mood is "calcuated" from it. Then the music track based off of that mood plays](/documentation/diagram.jpg)

## Acknowledgements

Thank you to the following, without whom this project would not have been possible:

Title V AbriendoCaminos DOE grant P031S210233  
Jessica Gonzalez  
[Phil Schatzmann](https://www.pschatzmann.ch/home/)
