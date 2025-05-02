#ifndef SENSOR_HEADER
#define SENSOR_HEADER

struct AccelData {
  float x;
  float y;
  float z;
};

struct GyroData {
  float x;
  float y;
  float z;
};

void sensorSetup(int pin_sda, int pin_scl);

void calibrate();

void getAccelData(AccelData &data);

void getGyroData(GyroData &data);

#endif // SENSOR_HEADER