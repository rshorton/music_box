//  Adapted from Arduino Beat Detector By Damian Peckett 2015
//    License: Public  Domain.
//    https://projecthub.arduino.cc/mamifero/arduino-beat-detector-881c72

#ifndef _FILTER_HPP_
#define _FILTER_HPP_

#include <Arduino.h>
#include <SoftwareSerial.h>

class Filter
{
public:
  Filter(int pin_audio_in, float thresh);

  bool update(bool &above_thresh);

private:
  class BassFilter
  {
    public:
      float update(float sample);  

    private:
      float xv[3] = {0, 0, 0};
      float yv[3] = {0, 0, 0};
  };

  class EnvelopeFilter
  {
    public:
      float update(float sample);

    private:
      float xv[2] = {0, 0};
      float yv[2] = {0, 0};
  };

  class BeatFilter
  {
    public:
      float update(float sample);

    private:
      float xv[3] = {0, 0, 0};
      float yv[3] = {0, 0, 0};
  };

  int pin_audio_in_;
  float thresh_ = 0.5f;   

  unsigned long last_sample_time_us_ = 0;
  int sample_cnt_ = 0;

  BassFilter bass_filter_;
  EnvelopeFilter envelope_filter_;
  BeatFilter beat_filter_;
};

#endif  // _FILTER_HPP_