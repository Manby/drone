#include "sensor.h"
#include "Arduino.h"

#include <Wire.h>

static const uint8_t MPU_addr = 0x68;
static const uint8_t MPU_reset = 0x6B;
static const uint8_t MPU_accel = 0x3B;
static const uint8_t MPU_gyro = 0x43;

static AccelData accel_error;
static GyroData gyro_error;

void sensorSetup(int pin_sda, int pin_scl) {
  Wire.begin(pin_sda, pin_scl);

  Wire.beginTransmission(MPU_addr);
  Wire.write(MPU_reset);
  Wire.write(0x00);
  Wire.endTransmission(true);

  vTaskDelay(20);

  calibrate();
}

void getAccelDataRaw(AccelData &data) {
  Wire.beginTransmission(MPU_addr);
  Wire.write(MPU_accel);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_addr, 6, true);
  data.x = ((short) (Wire.read() << 8 | Wire.read())) / 16384.0;
  data.y = ((short) (Wire.read() << 8 | Wire.read())) / 16384.0;
  data.z = ((short) (Wire.read() << 8 | Wire.read())) / 16384.0;
}

void getAccelData(AccelData &data) {
  getAccelDataRaw(data);
  data.x -= accel_error.x;
  data.y -= accel_error.y;
  data.z -= accel_error.z;
}

void getGyroDataRaw(GyroData &data) {
  Wire.beginTransmission(MPU_addr);
  Wire.write(MPU_gyro);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_addr, 6, true);
  data.x = ((short) (Wire.read() << 8 | Wire.read())) / 131.0;
  data.y = ((short) (Wire.read() << 8 | Wire.read())) / 131.0;
  data.z = ((short) (Wire.read() << 8 | Wire.read())) / 131.0;
}

void getGyroData(GyroData &data) {
  getGyroDataRaw(data);
  data.x -= gyro_error.x;
  data.y -= gyro_error.y;
  data.z -= gyro_error.z;
}

void calibrate() {
  int n = 100;

  accel_error = {0, 0, 0};
  gyro_error = {0, 0, 0};
  AccelData accel_data;
  GyroData gyro_data;
  for (int i = 0; i < n; i++) {
    getAccelDataRaw(accel_data);
    accel_error.x += accel_data.x;
    accel_error.y += accel_data.y;
    accel_error.z += accel_data.z;

    getGyroDataRaw(gyro_data);
    gyro_error.x += gyro_data.x;
    gyro_error.y += gyro_data.y;
    gyro_error.z += gyro_data.z;
  }

  accel_error.x /= n;
  accel_error.y /= n;
  accel_error.z /= n;
  accel_error.z -= 1; // When level, z reading should be 1g
  Serial.println(accel_error.z);

  gyro_error.x /= n;
  gyro_error.y /= n;
  gyro_error.z /= n;
}