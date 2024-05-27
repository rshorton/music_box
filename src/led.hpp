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

#ifndef _LED_HPP_
#define _LED_HPP_

#include <Arduino.h>

class LED
{
public:
  LED(int pin_led_0, int pin_led_1);

  void update();

  void set_ramp_delay(long delay) {
    ramp_update_delay_ = delay;
  }

  long get_ramp_delay() const {
    return ramp_update_delay_;
  }

  void set_breathing_enable(bool enable) {
    breathing_enabled_ = enable;
  }

  bool get_breathing_enable() const {
    return breathing_enabled_;
  }

  void set_on(bool on);
  void set_level(uint8_t level);

private:
  void update_breathing();
  
  int pin_led_0_;
  int pin_led_1_;

  uint8_t led_set_ = 0;
  uint8_t led_cur_ = 0;
  uint8_t led_step_ = 1;

  uint8_t led_min_ = 48;
  uint8_t led_max_ = 200;

  unsigned long last_update_time_ = 0;
  long ramp_update_delay_ = 0;

  bool breathing_enabled_ = false;
};

#endif  // _FILTER_HPP_