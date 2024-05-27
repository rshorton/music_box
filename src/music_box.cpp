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


#include "music_box.hpp"
#include "log.hpp"
#include "util.hpp"

#include <Arduino.h>

namespace {
const unsigned long PRESS_DURATION_FOR_STOP = 1000;
const unsigned long PRESS_DURATION_VOL_REPEAT = 500;

const int FIRST_TRACK_INDEX = 1;
}

MusicBox::MusicBox(int pin_sw_serial_rx, int pin_sw_serial_tx, int pin_busy_in,
                   int pin_switch_input_play_list_a, int pin_switch_input_play_list_b,
                   int pin_switch_input_vol_minus, int pin_switch_input_vol_plus,
                   int pin_led_out_0, int pin_led_out_1,
                   int pin_audio_in, float audio_thresh):
    sw_play_list_a_(pin_switch_input_play_list_a, true, 50),
    sw_play_list_b_(pin_switch_input_play_list_b, true, 50),
    vol_down_(pin_switch_input_vol_minus, true, 50),
    vol_up_(pin_switch_input_vol_plus, true, 50),
    player_(pin_sw_serial_rx, pin_sw_serial_tx, pin_busy_in),
    filter_(pin_audio_in, audio_thresh),
    led_(pin_led_out_0, pin_led_out_1)
{
  player_.setup();
}

void MusicBox::play_track(int folder, int track)
{
  log(PSTR("play_track, folder %d, track %d"), folder, track);

  player_.play_track(folder, track);
  delay(200);
  play_ = true;
  cur_play_list_ = PLAY_LIST_NONE;
  last_update_ = micros();
}

void MusicBox::play_next(bool reset)
{
  if (cur_play_list_ < 0 || cur_play_list_ > LAST_PLAY_LIST) {
    return;
  }

  PlayList &list = play_list_sets_[cur_play_list_];
  if (reset) {
    list.track_ = FIRST_TRACK_INDEX;
  } else {
    list.track_++;
  }
  log(PSTR("play_next, play_list %d, track %d"), list.folder_, list.track_);

  player_.play_track(list.folder_, list.track_);
  delay(200);
  ck_playing_ = true;
  last_update_ = micros();
}

void MusicBox::play(int play_list, bool reset, bool loop)
{
  log(PSTR("play, play_list %d, reset %d, loop %d"), play_list, reset, loop);
  loop_ = loop;

  if (play_list < 0 || play_list > LAST_PLAY_LIST) {
    return;
  }

  if (play_ && cur_play_list_ == play_list) {
    return;
  }

  play_ = true;
  cur_play_list_ = play_list;

  play_next(reset);
}

void MusicBox::stop()
{
  play_ = false;
  player_.stop();
  cur_play_list_ = PLAY_LIST_NONE;
}

void MusicBox::player_update()
{
  unsigned long time = micros();
  if ((long)(time - last_update_) > 500000) {
    log(PSTR("player_update"));

    last_update_ = time;

    if (play_) {
      log(PSTR("player_update, playing"));

      if (!player_.is_busy()) {
        log(PSTR("player_update, not busy, ck %d"), ck_playing_);

        // Player is not playing.  If this is the first ck after
        // starting a new track, then this track doesn't exist.
        // Start at the beginning of the list if looping is enabled.
        if (ck_playing_ && !loop_) {
          stop();
        } else {
          bool reset = ck_playing_;
          ck_playing_ = false;
          play_next(reset);
        }          

      } else {
        log(PSTR("player_update, busy, ck %d"), ck_playing_);
        ck_playing_ = false;
      }
    }
  }
}

void MusicBox::vol_down()
{
  player_.apply_vol_delta(-1);
}

void MusicBox::vol_up()
{
  player_.apply_vol_delta(1);
}

bool MusicBox::update_vol_button(InputDebounce &btn)
{
  if (btn.update()) {
    // State changed...
    if (btn.get_state() == HIGH) {
      return true;
    }
  // If pressed and held long enough, then repeat vol.    
  } else if (btn.get_state() == LOW &&
             btn.get_state_duration() > PRESS_DURATION_VOL_REPEAT) {
    btn.drop_next_change(true);
    return true;
  }
  return false;
}

int MusicBox::get_easter_egg_keypress()
{
  auto sw0 = sw_play_list_b_.read();
  auto sw1 = sw_play_list_a_.read();
  auto sw2 = vol_up_.read();
  auto sw3 = vol_down_.read();

  if (sw0 + sw1 + sw2 + sw3 == 2) {
    int val = ((sw0 << 3) | (sw1 << 2) | (sw2 << 1) | sw3) ^ 0xf;
    return EASTER_EGG_PRESS_TO_IDX[val];
  } else {
    return NO_EGG;
  }
}

void MusicBox::hatch_egg(int egg)
{
  egg_active_ = true;
  stop();
  play_track(PLAY_FOLDER_EGGS, egg + 1);
  log(PSTR("Egg: %d"), egg);
}  


void MusicBox::update()
{
  // Process beat filter.  Returns true if envelope threshold output valid
  bool above_thresh = false;
  if (filter_.update(above_thresh)) {
    led_.set_on(above_thresh);
  }

  // Update LED mode based on player busy
  auto busy = player_.is_busy();
  if (busy != playing_) {
    log(PSTR("player playing: %d"), busy);
    playing_ = busy;
    if (busy) {
      led_.set_ramp_delay(100);
      led_.set_breathing_enable(false);
    } else {
      led_.set_ramp_delay(15000);
      led_.set_breathing_enable(true);
    }
  }

  led_.update();
  player_update();

  if (egg_on_start != NO_EGG) {
    hatch_egg(egg_on_start);
    egg_on_start = NO_EGG;
    return;
  }

  if (egg_active_) {
    if (playing_) {
      return;
    }
    egg_active_ = false;
  }

  // Check input switches...

  auto egg = get_easter_egg_keypress();

  if (egg != NO_EGG) {
    hatch_egg(egg);
    return;
  }

  // Playlist A used by button labeled as with music note
  bool is_playing_a = play_ && cur_play_list_ == PLAY_LIST_A;

  if (sw_play_list_a_.update()) {
    // State changed...
    if (sw_play_list_a_.get_state() == HIGH) {
      // Button released
      log(PSTR("Play track A"));

      if (is_playing_a) {
        play_next(false);
      } else {
        play(PLAY_LIST_A, false, true);
      }
    }

  // If pressed and held long enough, then stop.    
  } else if (sw_play_list_a_.get_state() == LOW &&
             sw_play_list_a_.get_state_duration() > PRESS_DURATION_FOR_STOP) {
    sw_play_list_a_.drop_next_change(true);
    stop();
  }

  // Playlist B used by button labeled with crescent moon
  bool is_playing_b = play_ && cur_play_list_ == PLAY_LIST_B;

  if (sw_play_list_b_.update()) {
    if (sw_play_list_b_.get_state() == HIGH) {
      log(PSTR("Play track B"));

      if (is_playing_b) {
        stop();
      } else {
        long track = random(NUM_LIST_B_TRACKS);
        play_track(PLAY_FOLDER_B, track + 1);
        //play(PLAY_LIST_B, true, false);
      }
    }

  // If pressed and held long enough, then stop.    
  } else if (sw_play_list_b_.get_state() == LOW &&
             sw_play_list_b_.get_state_duration() > PRESS_DURATION_FOR_STOP) {
    sw_play_list_b_.drop_next_change(true);
    stop();
  }

  if (update_vol_button(vol_down_)) {
    vol_down();
  }

  if (update_vol_button(vol_up_)) {
    vol_up();
  }

  // Debug/testing support
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      log(PSTR("cmd clear"));
      test_cmd_nibble_cnt_ = 0;

    } else if (ch == '[') {
      led_.set_breathing_enable(false);
      if (led_val_ < 254 - step_) {
        led_val_ += step_;
        led_.set_level(led_val_);
        log(PSTR("led out %d"), led_val_);
      }

    } else if (ch == ']') {
      led_.set_breathing_enable(false);
      if (led_val_ > step_) {
        led_val_ -= step_;
        led_.set_level(led_val_);
        log(PSTR("led out %d"), led_val_);
      }

    } else if (ch == ',') {
      led_.set_on(false);

    } else if (ch == '.') {
      led_.set_on(true);

    } else if (ch == 'q') {
      led_.set_ramp_delay(max(0, led_.get_ramp_delay() - 50));
      log(PSTR("led_ramp_delay %d"), led_.get_ramp_delay());

    } else if (ch == 'w') {
      led_.set_ramp_delay(led_.get_ramp_delay() + 10);
      log(PSTR("led_ramp_delay %d"), led_.get_ramp_delay());

    } else {
      player_test_cmd_[test_cmd_nibble_cnt_++] = ch;
      if (test_cmd_nibble_cnt_ == 6) {
        int b0 = hexToInt(player_test_cmd_[0], player_test_cmd_[1]);
        int b1 = hexToInt(player_test_cmd_[2], player_test_cmd_[3]);
        int b2 = hexToInt(player_test_cmd_[4], player_test_cmd_[5]);

        log(PSTR("cmd %02x %02x %02x"), b0, b1, b2);
        test_cmd_nibble_cnt_ = 0;
        player_.send_cmd(b0, b1, b2);
      }
    }
  }  
}