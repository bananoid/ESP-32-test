#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

class Encoder
{
private:
public:
  long encoder_pulses = 0;
  long encoder_last_pulses = 0;
  bool encoder_changed = false;
  bool encoder_a = true;
  bool encoder_b = false;

  Encoder();
  void begin();
  bool update();
};

extern Encoder *encoder;

#endif