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

// Credits:
//  Player control: 	"Richard Makes Escape Rooms"  product review commment on this page:
//    https://www.amazon.com/UMLIFE-Player-Module-DFPlayer-Arduino/product-reviews/B07Y2YKYRS/ref=cm_cr_getr_d_paging_btm_next_3?ie=UTF8&reviewerType=all_reviews&pageNumber=3
//  

#ifndef _PLAYER_HPP_
#define _PLAYER_HPP_

#include <Arduino.h>
#include <SoftwareSerial.h>

class Player
{
public:
    const int MIN_VOL = 0;
    const int MAX_VOL = 30;
    const int DEF_VOL = 22;

    Player() = default;
    Player(int pin_sw_serial_rx, int pin_sw_serial_tx, int pin_busy_in);
    ~Player();

    int set_vol(int vol);
    int get_vol() const {
        return vol_;
    }

    void setup();
    int apply_vol_delta(int delta);

    bool is_busy() const;
    void stop() const;
    void play_track(int folder, int track) const;
    void play_folder(int folder) const;

    void send_cmd(int cmd, int lb, int hb, bool reply = false) const;

private:
    SoftwareSerial *serial_ptr_ = nullptr;
    int pin_busy_in_;
    int vol_ = MIN_VOL;
};

#endif  // _PLAYER_HPP_