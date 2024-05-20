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
  if (serial_ptr_) {
    delete serial_ptr_;
  }
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
  send_cmd(0x0E, 0, 0, false);
  delay(200);

  // Select folder to play
  send_cmd(0xf, folder, 0, false);
  delay(200);

  // Send next track cmd that begins playback
  send_cmd(0x01, 0, 0, false);
  delay(200);
}

void Player::send_cmd(int cmd, int lb, int hb, bool reply = false) const
{
  // standard format for module command stream
  uint8_t buf[] = {0x7E, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF};
  int16_t chksum = 0;
  int idx = 3; // position of first command byte

  buf[idx++] = (uint8_t)cmd; // inject command byte in buffer
  if (reply) buf[idx++] = 0x01;
  else buf[idx++] = 0x00;// set if reply is needed/wanted
  
  if (hb >= 0) // if there is a high byte data field
    buf[idx++] = (uint8_t)hb; // add it to the buffer

  if (lb >= 0) // if there is a low byte data field
    buf[idx++] = (uint8_t)lb; // add it to the buffer

  buf[2] = idx - 1; // inject command length into buffer
  buf[idx++] = 0xEF; // place end-of-command byte

  serial_ptr_->write(buf, idx); // send the command to module
}
