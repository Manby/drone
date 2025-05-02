#ifndef NUNCHUCK_HEADER
#define NUNCHUCK_HEADER

#include <driver/i2c.h>

struct NunchuckState {
  short int acc_x;
  short int acc_y;
  short int acc_z;
  signed char x;
  signed char y;
  char c;
  char z;
};

esp_err_t nunchuckSetup(int pin_sda, int pin_scl);

esp_err_t requestNunchuckState(NunchuckState &state);

#endif // NUNCHUCK_HEADER