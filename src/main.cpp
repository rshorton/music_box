#include <Arduino.h>
#include "music_box.hpp"

const int PIN_SW_INPUT_VOL_MINUS = 7;
const int PIN_SW_INPUT_VOL_PLUS = 6;
const int PIN_SW_INPUT_MUSIC = 4;
const int PIN_SW_INPUT_SLEEP = 5;

const int PIN_LED_OUT0 = 10;
const int PIN_LED_OUT1 = 11;

const int PIN_PLAYER_RX = 2;
const int PIN_PLAYER_TX = 3;
const int PIN_PLAYER_BUSY_INPUT = 9;

const int PIN_AUDIO_IN = 0;
const float AUDIO_THRESH = 100.0f;

const int PIN_ANALOG_UNCONN = 1;


void setup()
{
  Serial.begin(9600); //for console debugging
  Serial.println("ready");

  // Analog
  randomSeed(analogRead(PIN_ANALOG_UNCONN));

  MusicBox music_box(PIN_PLAYER_RX, PIN_PLAYER_TX, PIN_PLAYER_BUSY_INPUT,
                     PIN_SW_INPUT_MUSIC, PIN_SW_INPUT_SLEEP,
                     PIN_SW_INPUT_VOL_MINUS, PIN_SW_INPUT_VOL_PLUS,
                     PIN_LED_OUT0, PIN_LED_OUT1,
                     PIN_AUDIO_IN, AUDIO_THRESH);

  while (1) {
    music_box.update();
  }
}

void loop()
{
}


