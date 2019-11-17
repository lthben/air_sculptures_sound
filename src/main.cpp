/*
  Author: Benjamin Low (Lthben@gmail.com)
  Date: Nov 2019
  Description: Air sculptures 1 & 2- Teensy sound only

      Teensy 3.2 with audio shield used for sculptures 1 & 2. Sound only. The Arduino Mega controls the LEDs. 
      Each sculpture has two buttons that activates the sound, shared by the Mega and Teensy.
      There is a sound for idle mode and one for active playback mode.
*/

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>

//-------------------- USER DEFINED SETTINGS --------------------//

//Uncomment one below
#define __CO2__
// #define __PM25__

//-------------------- Audio --------------------//

// GUItool: begin automatically generated code. See https://www.pjrc.com/teensy/gui/index.html
AudioPlaySdWav playSdWav1; //xy=416,186
AudioOutputI2S i2s1;       //xy=821,189
AudioConnection patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection patchCord2(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1; //xy=615,336
// GUItool: end automatically generated code

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

float vol = 0.4; //master volume gain 0.0 - 1.0

#if defined(__CO2__)
const char *idleTrack = "DRONE1.WAV";
const char *activeTrack = "TINKLING.WAV";
const long strip1playDur = 17000, strip2playDur = 40000; //msec duration for the button playback mode for each strip
#elif defined(__PM25__)
const char *idleTrack = "DRONE1.WAV";
const char *activeTrack = "RAYGUN.WAV";
const long strip1Dur = 20000, strip2Dur = 32000; //msec duration for the button playback mode for each strip
#endif

//-------------------- Buttons and distance sensor --------------------//
Bounce button0 = Bounce(0, 15); // 15 = 15 ms debounce time
Bounce button1 = Bounce(1, 15);

bool isButton0Pressed, isButton1Pressed; //track response to button triggered

const int IDLE_MODE = 1, BUTTON_MODE = 2;
unsigned int strip1playMode = IDLE_MODE, strip2playMode = IDLE_MODE;
bool strip1hasPlayModeChanged = false, strip2hasPlayModeChanged = false; //for audio track changes
elapsedMillis strip1msec, strip2msec; //tracks when the button for each strip is triggered

/*--------------------------------------------------------------------------------
  Setup
--------------------------------------------------------------------------------*/
void setup()
{
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  Serial.begin(9600);

  AudioMemory(8);

  sgtl5000_1.enable();
  sgtl5000_1.volume(vol);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

/*--------------------------------------------------------------------------------
  Loop
--------------------------------------------------------------------------------*/
void loop()
{
  read_buttons();

  set_playmode();

  play_audio();
}

/*--------------------------------------------------------------------------------
  Read buttons
--------------------------------------------------------------------------------*/
void read_buttons()
{
  if (strip1playMode == IDLE_MODE)
  {
    button0.update();

    if (button0.fallingEdge())
    {
      isButton0Pressed = true;
      Serial.println("button0 pressed");
    }
  }
  if (strip2playMode == IDLE_MODE)
  {
    button1.update();

    if (button1.fallingEdge())
    {
      isButton1Pressed = true;
      Serial.println("button1 pressed");
    }
  }
}

/*--------------------------------------------------------------------------------
  Set playmode
--------------------------------------------------------------------------------*/
void set_playmode() 
{
  if (strip1msec > strip1playDur)
  {
    strip1playMode = IDLE_MODE;
    strip1hasPlayModeChanged = true;
  }
  if (strip2msec > strip2playDur)
  {
    strip2playMode = IDLE_MODE;
    strip1hasPlayModeChanged = true;
  }
  if (isButton0Pressed == true) //set this to false after playback mode finishes
  {
    strip1playMode = BUTTON_MODE;
    strip1hasPlayModeChanged = true;
    strip1msec = 0;
  }
  if (isButton1Pressed == true) //set this to false after playback mode finishes
  {
    strip2playMode = BUTTON_MODE;
    strip2hasPlayModeChanged = true;
    strip2msec = 0;
  }
}

/*--------------------------------------------------------------------------------
  Play audio
--------------------------------------------------------------------------------*/
void play_audio()
{
  if (strip1playMode == IDLE_MODE && strip2playMode == IDLE_MODE)
  {
    if ((strip1hasPlayModeChanged == true || strip2hasPlayModeChanged) && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
      strip1hasPlayModeChanged = strip2hasPlayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
    }
  }
  else if (strip1playMode == BUTTON_MODE || strip2playMode == BUTTON_MODE)
  {
    if ((strip1hasPlayModeChanged == true || strip2hasPlayModeChanged == true) && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
      strip1hasPlayModeChanged = strip2hasPlayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
    }
  }
}
