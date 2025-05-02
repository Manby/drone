#include "nunchuck.h"
#include "Arduino.h"

#define NUNCHUCK_ADDRESS 0x52
constexpr i2c_port_t i2c_port = (i2c_port_t) 0;

static esp_err_t i2cWrite(const uint8_t *data, size_t len) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (NUNCHUCK_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write(cmd, data, len, true);
  i2c_master_stop(cmd);
  esp_err_t res = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  return res;
}

static esp_err_t i2cRead(uint8_t *data, size_t len)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (NUNCHUCK_ADDRESS << 1) | I2C_MASTER_READ, true);
  
  if (len > 1) {
    i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
  }
  i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  esp_err_t res = i2c_master_cmd_begin(i2c_port, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);
  return res;
}

esp_err_t nunchuckSetup(int pin_sda, int pin_scl) {
  i2c_config_t config {};
  config.mode = I2C_MODE_MASTER;
  config.sda_io_num = pin_sda;
  config.sda_pullup_en = GPIO_PULLUP_ENABLE;
  config.scl_io_num = pin_scl;
  config.scl_pullup_en = GPIO_PULLUP_ENABLE;
  config.master.clk_speed = 100000;
  i2c_param_config(i2c_port, &config);
  esp_err_t res = i2c_driver_install(i2c_port, config.mode, 0, 0, 0);
  if (res != ESP_OK) return res;

  const uint8_t init_data[] = {0xf0, 0x55};
  res = i2cWrite(init_data, 2);
  Serial.println(res == ESP_OK ? "OK" : "NOT OK");
  
  return res;
}

esp_err_t requestNunchuckState(NunchuckState &state) {
  const uint8_t request_init[] = {0x00};
  int res = i2cWrite(request_init, 1);
  if (res != ESP_OK) return res;

  uint8_t data[6];
  res = i2cRead(data, 6);
  if (res != ESP_OK) return res;

  state.x = data[0] - (1<<7);
  state.y = data[1] - (1<<7);
  state.acc_x = ((data[2] << 2) | ((data[5] & 0x0c) >> 2)) - (1<<9);
  state.acc_y = ((data[3] << 2) | ((data[5] & 0x30) >> 4)) - (1<<9);
  state.acc_z = ((data[4] << 2) | ((data[5] & 0xc0) >> 6)) - (1<<9);
  state.c = (data[5] & 0x02) ? 0 : 1;
  state.z = (data[5] & 0x01) ? 0 : 1;

  return ESP_OK;
}


  