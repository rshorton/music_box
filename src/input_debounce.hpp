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

#ifndef _INPUT_DEBOUNCE_HPP_
#define _INPUT_DEBOUNCE_HPP_

#include <Arduino.h>

/*
  This class is adapted from this Arduino sample code...

  Debounce

  Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
  press), the output pin is toggled from LOW to HIGH or HIGH to LOW. There's a
  minimum delay between toggles to debounce the circuit (i.e. to ignore noise).

  The circuit:
  - LED attached from pin 13 to ground through 220 ohm resistor
  - pushbutton attached from pin 2 to +5V
  - 10 kilohm resistor attached from pin 2 to ground

  - Note: On most Arduino boards, there is already an LED on the board connected
    to pin 13, so you don't need any extra components for this example.

  created 21 Nov 2006
  by David A. Mellis
  modified 30 Aug 2011
  by Limor Fried
  modified 28 Dec 2012
  by Mike Walters
  modified 30 Aug 2016
  by Arturo Guadalupi

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce
*/


class InputDebounce
{
public:
    InputDebounce() = default;    
    InputDebounce(int input_pin, bool use_pullup, unsigned long debounce_delay_ms);
    
    int update();

    int read() {
      return digitalRead(input_pin_);
    }

    int get_state() const {
        return button_state_;
    };

    void set_state(int state) {
        button_state_ = HIGH;
    };

    unsigned long get_state_duration() const {
      return  millis() - last_state_start_time_;
    }

    void drop_next_change(bool drop) {
      drop_next_change_ = drop;
    }

private:
    int input_pin_ = 0;
    unsigned long debounce_delay_ms_ = 50;    // the debounce time in ms

    int button_state_ = HIGH;                   // the current reading from the input pin
    int last_button_state_ = LOW;               // the previous reading from the input pin
    unsigned long last_debounce_time_ = 0;      // the last time the output pin was toggled
    unsigned long last_state_start_time_ = 0;   // the start time of the last button state
    bool drop_next_change_ = false;             // True to ignore next state change
};

#endif // _INPUT_DEBOUNCE_HPP_