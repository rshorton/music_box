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


#ifndef _MUSICBOX_HPP_
#define _MUSICBOX_HPP_

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "input_debounce.hpp"
#include "player.hpp"
#include "filter.hpp"
#include "led.hpp"

class MusicBox
{
public:
  MusicBox(int pin_sw_serial_rx, int pin_sw_serial_tx, int pin_busy_in,
           int pin_switch_input_play_list_a, int pin_switch_input_play_list_b,
           int pin_switch_input_vol_minus, int pin_switch_input_vol_plus,
           int pin_led_out_0, int pin_led_out_1,
           int pin_audio_in, float audio_thresh);

  void update();

private:
  const int NO_EGG = -1;
                                       // Mapping of 2-key presses to easter egg ID
                                       //       0       1       2   3       4   5   6       7       8   9   a       b   c       d       e       f
  const int EASTER_EGG_PRESS_TO_IDX[16] = {NO_EGG, NO_EGG, NO_EGG,  0, NO_EGG,  1,  2, NO_EGG, NO_EGG,  3,  4, NO_EGG,  5, NO_EGG, NO_EGG, NO_EGG};

  enum PLAY_LIST_IDX {PLAY_LIST_NONE = -1,
                      PLAY_LIST_A = 0,
                      PLAY_LIST_B,
                      LAST_PLAY_LIST = PLAY_LIST_B,
 };

  enum class PlaylistId { list_a, list_b };

  const int PLAY_FOLDER_A = 1;
  const int PLAY_FOLDER_B = 2;
  const int PLAY_FOLDER_EGGS = 3;

  const int NUM_LIST_B_TRACKS = 5;

  struct PlayList {
    PlayList(int folder, int track):
      folder_(folder),
      track_(track)
    {}

    int folder_;
    int track_;
  };

  void play_track(int folder, int track);
  void play_next(bool reset);
  void play(int play_list, bool reset, bool loop);
  void stop();

  void player_update();
  
  void vol_down();
  void vol_up();
  bool update_vol_button(InputDebounce &btn);

  int get_easter_egg_keypress();
  void hatch_egg(int egg);

  InputDebounce sw_play_list_a_;
  InputDebounce sw_play_list_b_;
  InputDebounce vol_down_;
  InputDebounce vol_up_;

  Player player_;
  Filter filter_;

  LED led_;

  bool playing_ = true;
  bool ck_playing_ = false;
  
  bool play_ = false;
  bool loop_ = true;
  bool egg_active_ = false;

  int egg_on_start = 7;       // Play this clip (if present) on startup

  int cur_play_list_ = PLAY_LIST_A;
  PlayList play_list_sets_[2] = {PlayList(PLAY_FOLDER_A, 0), PlayList(PLAY_FOLDER_B, 0)};

  unsigned long last_update_ = 0;

  // debug/test
  int test_cmd_nibble_cnt_ = 0;
  char player_test_cmd_[6];

  uint8_t led_val_ = 128;
  uint8_t step_ = 20;
};

#endif  // _MUSICBOX_HPP_