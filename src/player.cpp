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


// Credit:
//  Player control: 	"Richard Makes Escape Rooms"  product review commment on this page:
//    https://www.amazon.com/UMLIFE-Player-Module-DFPlayer-Arduino/product-reviews/B07Y2YKYRS/ref=cm_cr_getr_d_paging_btm_next_3?ie=UTF8&reviewerType=all_reviews&pageNumber=3
//  

#include "player.hpp"
#include "log.hpp"

#include <Arduino.h>


Player::Player(int pin_sw_serial_rx, int pin_sw_serial_tx, int pin_busy_in):
    pin_busy_in_(pin_busy_in)
{
    pinMode(pin_busy_in, INPUT_PULLUP);    
    serial_ptr_ = new SoftwareSerial(pin_sw_serial_rx, pin_sw_serial_tx);
}

void Player::setup()
{
  log(PSTR("Player setup"));

  serial_ptr_->begin(9600);
  delay(500);

  // Stop playing if playing
  send_cmd(0x0E, 0, 0, false);
  delay(200);

  set_vol(DEF_VOL);
  delay(100);
}

Player::~Player()
{
  send_cmd(0x0E, 0, 0, false);
}

int Player::set_vol(int vol)
{
  vol_ = max(MIN_VOL, vol);
  vol_ = min(MAX_VOL, vol_);

  log(PSTR("set volume, vol %d"), vol_);
  send_cmd(6, vol_, 0, false);
  delay(200);
  return vol_;
}

int Player::apply_vol_delta(int delta)
{
  set_vol(vol_ + delta);
  return vol_;
}

bool Player::is_busy() const
{
  return digitalRead(pin_busy_in_) == 0;
}

void Player::stop() const
{
  send_cmd(0x0E, 0, 0, false);
  delay(200);
}

void Player::play_track(int folder, int track) const
{
  // Stop if playing
  stop();

  // Play folder
  send_cmd(0x0F, track, folder, false);
}

void Player::play_folder(int folder) const
{
  // Stop if playing  
  stop();

  // Select folder to play
  send_cmd(0xf, folder, 0, false);
  delay(200);

  // Send next track cmd to begin playback
  send_cmd(0x01, 0, 0, false);
  delay(200);
}

void Player::send_cmd(int cmd, int lb, int hb, bool reply) const
{
  // standard format for module command stream
  uint8_t buf[] = {0x7E, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF};
  int idx = 3;                    // position of first command byte

  buf[idx++] = (uint8_t)cmd;      // command byte
  if (reply) buf[idx++] = 0x01;
  else buf[idx++] = 0x00;         // set if reply is wanted
  
  if (hb >= 0)                    // conditionally add high byte data field
    buf[idx++] = (uint8_t)hb;

  if (lb >= 0)                    // conditionally add low byte data field
    buf[idx++] = (uint8_t)lb;

  buf[2] = idx - 1;               // command length
  buf[idx++] = 0xEF;              // end-of-command byte

  serial_ptr_->write(buf, idx);   // send the command to module
}
