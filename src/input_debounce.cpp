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
