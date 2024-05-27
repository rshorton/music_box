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

#include "led.hpp"
#include <Arduino.h>

LED::LED(int pin_led_0, int pin_led_1):
  pin_led_0_(pin_led_0),
  pin_led_1_(pin_led_1)
{
  pinMode(pin_led_0_, OUTPUT);
  pinMode(pin_led_1_, OUTPUT);
}

void LED::update_breathing()
{
  if (led_cur_ == led_set_) {
    if (led_cur_ >= led_max_) {
      led_set_ = led_min_;
    } else {
      led_set_ = led_max_;
    }
  }
}

void LED::update()
{
  unsigned long time = micros();
  if ((long)(time - last_update_time_) > 50 + ramp_update_delay_) {
    last_update_time_ = time;
    if (led_cur_ < led_set_) {
      led_cur_ = min(led_set_, led_cur_ + led_step_);
    } else if (led_cur_ > led_set_) {
      led_cur_ = max(led_set_, led_cur_ - led_step_);
    } else {
      if (breathing_enabled_) {
        update_breathing();
      }      
      return;
    }
    analogWrite(pin_led_0_, led_cur_);
    analogWrite(pin_led_1_, led_cur_);
  }
}

void LED::set_on(bool on)
{
  if (breathing_enabled_) {
    return;
  }
  if (on) {
    led_set_ = led_max_;
  } else {
    led_set_ = led_min_;
  }
}

void LED::set_level(uint8_t level)
{
 led_cur_ = min(led_max_, level);
 led_cur_ = max(led_min_, led_cur_);
 
  analogWrite(pin_led_0_, led_cur_);
  analogWrite(pin_led_1_, led_cur_);
}
