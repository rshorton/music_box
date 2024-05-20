#include "led.hpp"
#include <Arduino.h>

namespace {

}

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
    //log(PSTR("breath, at set %d"), led_cur_);
    if (led_cur_ >= led_max_) {
      led_set_ = led_min_;
      //log(PSTR("breath, go to min"));
    } else {
      led_set_ = led_max_;
      //log(PSTR("breath, go to max"));
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
