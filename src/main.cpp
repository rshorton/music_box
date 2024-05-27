// MIT License
//
// Copyright (c) 2024 Scott Horton
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <Arduino.h>
#include "music_box.hpp"

const int PIN_SW_INPUT_VOL_MINUS = 7; // SW4
const int PIN_SW_INPUT_VOL_PLUS = 6;  // SW3
const int PIN_SW_INPUT_MUSIC = 4;     // SW1
const int PIN_SW_INPUT_SLEEP = 5;     // SW2

const int PIN_LED_OUT0 = 10;          // Drives single LED
const int PIN_LED_OUT1 = 11;          // Drives LED strip

const int PIN_PLAYER_RX = 2;          // From player module
const int PIN_PLAYER_TX = 3;          // To player modules
const int PIN_PLAYER_BUSY_INPUT = 9;  // Logic lo when player is playing

const int PIN_AUDIO_IN = 0;           // Audio input for beat detector
const float AUDIO_THRESH = 100.0f;    // Beat thresh

const int PIN_ANALOG_UNCONN = 1;      // Used for seeding Random


void setup()
{
  Serial.begin(9600);                 // Only used for console debugging
  Serial.println("ready");

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


