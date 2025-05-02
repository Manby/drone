#include "motor.h"

#include "Arduino.h"

static int pin_fl;
static int pin_fr;
static int pin_bl;
static int pin_br;

void motorSetup(int pin_fl_, int pin_fr_, int pin_bl_, int pin_br_) {
  pin_fl = pin_fl_;
  pin_fr = pin_fr_;
  pin_bl = pin_bl_;
  pin_br = pin_br_;
  pinMode(pin_fl, OUTPUT);
  pinMode(pin_fr, OUTPUT);
  pinMode(pin_bl, OUTPUT);
  pinMode(pin_br, OUTPUT);
}

void driveFl(int val) {
  analogWrite(pin_fl, val);
}

void driveFr(int val) {
  analogWrite(pin_fr, val);
}

void driveBl(int val) {
  analogWrite(pin_bl, val);
}

void driveBr(int val) {
  analogWrite(pin_br, val);
}

void driveAll(int val) {
  analogWrite(pin_fl, val);
  analogWrite(pin_fr, val);
  analogWrite(pin_bl, val);
  analogWrite(pin_br, val);
}