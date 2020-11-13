#include <Arduino.h>
#include "Encoder.h"

#define ENCODER_INT_A 22
#define ENCODER_INT_B 23
#define ENCODER_STEP_RESOLUTION 800.0f

static void encoder_a_rise();
static void encoder_b_rise();
static void encoder_a_fall();
static void encoder_b_fall();

Encoder::Encoder()
{
}

void Encoder::begin()
{
  attachInterrupt(ENCODER_INT_A, encoder_a_rise, RISING);
  attachInterrupt(ENCODER_INT_B, encoder_b_rise, RISING);
  Serial.println("Encoder Begin");
}

bool Encoder::update()
{

  if (encoder_changed)
  {
    long delta_pulses = encoder_pulses - encoder_last_pulses;
    encoder_last_pulses = encoder_pulses;
    deltaSpeed = (float)delta_pulses / ENCODER_STEP_RESOLUTION;

    // Serial.print("Encoder ");
    // Serial.println(encoder_pulses);
    // Serial.print("deltaSpeed ");
    // Serial.println(deltaSpeed);

    encoder_changed = false;
    // return true;
  }

  speed += deltaSpeed * 20;
  if (speed > 1)
  {
    speed = 1;
  }

  if (speed < -1)
  {
    speed = -1;
  }

  acc = pow((deltaSpeed * 20), 3) * 50.0;
  accFiltered += (acc - accFiltered) * 0.05;
  accSpeed = speed * 20 + accFiltered;

  // Serial.println(deltaSpeed);
  // Serial.println(speed);
  return true;
}

Encoder *encoder = new Encoder();

static void encoder_a_rise()
{
  // Serial.println("encoder_a_rise");

  detachInterrupt(ENCODER_INT_A);

  if (encoder->encoder_b)
  {
    encoder->encoder_pulses--;
  }
  else
  {
    encoder->encoder_pulses++;
  }

  encoder->encoder_a = true;
  encoder->encoder_changed = true;

  attachInterrupt(ENCODER_INT_A, encoder_a_fall, FALLING);
}

static void encoder_a_fall()
{
  // Serial.println("encoder_a_fall");

  detachInterrupt(ENCODER_INT_A);

  if (encoder->encoder_b)
  {
    encoder->encoder_pulses++;
  }
  else
  {
    encoder->encoder_pulses--;
  }

  encoder->encoder_a = false;
  encoder->encoder_changed = true;

  attachInterrupt(ENCODER_INT_A, encoder_a_rise, RISING);
}

static void encoder_b_rise()
{

  // Serial.println("encoder_b_rise");

  detachInterrupt(ENCODER_INT_B);

  if (encoder->encoder_a)
  {
    encoder->encoder_pulses++;
  }
  else
  {
    encoder->encoder_pulses--;
  }

  encoder->encoder_b = true;
  encoder->encoder_changed = true;

  attachInterrupt(ENCODER_INT_B, encoder_b_fall, FALLING);
}

static void encoder_b_fall()
{
  // Serial.println("encoder_b_fall");

  detachInterrupt(ENCODER_INT_B);

  if (encoder->encoder_a)
  {
    encoder->encoder_pulses--;
  }
  else
  {
    encoder->encoder_pulses++;
  }

  encoder->encoder_b = false;
  encoder->encoder_changed = true;

  attachInterrupt(ENCODER_INT_B, encoder_b_rise, RISING);
}
