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