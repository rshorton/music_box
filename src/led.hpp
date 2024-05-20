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