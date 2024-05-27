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

#include "input_debounce.hpp"

InputDebounce::InputDebounce(int input_pin, bool use_pullup, unsigned long debounce_delay_ms):
    input_pin_(input_pin),
    debounce_delay_ms_(debounce_delay_ms)
{
  pinMode(input_pin_, use_pullup? INPUT_PULLUP: INPUT);
  button_state_ = read();
}


int InputDebounce::update()
{
  bool changed = false;

  // read the state of the switch into a local variable:
  int reading = read();

  // If the switch changed, due to noise or pressing:
  if (reading != last_button_state_) {
    // reset the debouncing timer
    last_debounce_time_ = millis();
  }

  if ((millis() - last_debounce_time_) > debounce_delay_ms_) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != button_state_) {
      button_state_ = reading;
      auto now = millis();
      last_state_start_time_ = now;

      if (drop_next_change_) {
        drop_next_change_ = false;
      } else {
        changed = true;
      }        
    }
  }

  // Save the reading. Next time through the loop, it'll be the lastButtonState_:
  last_button_state_ = reading;

  return changed;
}
