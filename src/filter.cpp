#include "filter.hpp"
#include <Arduino.h>

//  defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr,  bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr,  bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

namespace {
  // Beat detector global sample Ratte: 5000hz
  const unsigned long SAMPLE_PERIOD_US = 200;
}

Filter::Filter(int pin_audio_in, float thresh):
  pin_audio_in_(pin_audio_in),
  thresh_(thresh * 0.02f)
{
  // Set  ADC to 77khz, max for 10bit
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);
}  


//  20 - 200hz Single Pole Bandpass IIR Filter
float Filter::BassFilter::update(float sample)
{
  xv[0] = xv[1];
  xv[1] =  xv[2]; 
  xv[2] = (sample) / 3.f; // change here to values close to 2, to adapt  for stronger or weeker sources of line level audio  
  

  yv[0] =  yv[1];
  yv[1] = yv[2]; 
  yv[2] = (xv[2] - xv[0]) + (-0.7960060012f  * yv[0]) + (1.7903124146f * yv[1]);
  return yv[2];
}

// 10hz Single  Pole Lowpass IIR Filter
float Filter::EnvelopeFilter::update(float sample)
{
  //10hz low pass
  xv[0] = xv[1]; 
  xv[1]  = sample / 50.f;
  yv[0] = yv[1]; 
  yv[1] = (xv[0] + xv[1]) + (0.9875119299f  * yv[0]);
  return yv[1];
}

// 1.7 - 3.0hz Single Pole Bandpass IIR  Filter
float Filter::BeatFilter::update(float sample)
{
  xv[0] = xv[1];
  xv[1] = xv[2]; 
  xv[2] = sample / 2.7f;
  yv[0] = yv[1];
  yv[1] = yv[2]; 
  yv[2] = (xv[2] - xv[0]) + (-0.7169861741f  * yv[0]) + (1.4453653501f * yv[1]);
  return yv[2];
}

bool Filter::update(bool &above_thresh)
{
  unsigned long time = micros();

  if ((long)time - (long)last_sample_time_us_ < SAMPLE_PERIOD_US) {
    return false;
  }

  last_sample_time_us_ = time;
  
  sample_cnt_++;

  // Read ADC and center so +-512
  float sample = (float)analogRead(pin_audio_in_)- 503.0f;

  // Filter only bass component
  float value = bass_filter_.update(sample);

  // Take signal amplitude and filter
  if (value < 0) {
    value=-value;
  }
  float envelope = envelope_filter_.update(value);

  // Every 200 samples (25hz)  filter the envelope 
  if (sample_cnt_ >= 200) {
    // Reset sample counter
    sample_cnt_ = 0;

    // Filter out repeating bass sounds 100 - 180bpm
    float beat = beat_filter_.update(envelope);

    above_thresh = beat > thresh_;
    return true;
  }
  return false;
}