//#define CONTROLLER

#ifndef CONTROLLER

#include "motor.h"
#include "rf.h"
#include "sensor.h"

#define PIN_MOTOR_FL 25
#define PIN_MOTOR_FR 12
#define PIN_MOTOR_BL 26
#define PIN_MOTOR_BR 13

#define PIN_SENSOR_SDA 27
#define PIN_SENSOR_SCL 32

#define PIN_RF_RX 16
#define PIN_RF_TX 17

static unsigned long prevTime;
float accelRoll;
float accelPitch;
float gyroRoll;
float gyroPitch;
float roll;
float pitch;
float yaw;

void drive(void *parameter) {
  AccelData accel_data;
  GyroData gyro_data;
  prevTime = millis();

  while (1) {
    getAccelData(accel_data);
    getGyroData(gyro_data);

    accelRoll = (atan(accel_data.x / sqrt(pow(accel_data.x, 2) + pow(accel_data.z, 2))) * 180 / PI) - 0.58;
    accelPitch = (atan(-1 * accel_data.x / sqrt(pow(accel_data.y, 2) + pow(accel_data.z, 2))) * 180 / PI) + 1.58;

    unsigned long time = millis();
    unsigned long deltaTime = (time - prevTime) / 1000;

    gyroRoll += gyro_data.x * deltaTime;
    roll = 0.96 * gyroRoll + 0.04 * accelRoll;

    gyroPitch += gyro_data.y * deltaTime;
    pitch = 0.96 * gyroPitch + 0.04 * accelPitch;

    yaw += gyro_data.z * deltaTime;

    if (0) {
      Serial.print(accel_data.x);
      Serial.print("/");
      Serial.print(accel_data.y);
      Serial.print("/");
      Serial.print(accel_data.z);
      Serial.print(":");
      Serial.print(gyro_data.x);
      Serial.print("/");
      Serial.print(gyro_data.y);
      Serial.print("/");
      Serial.println(gyro_data.z);
    }

    /*
    Serial.print(roll);
    Serial.print(",");
    Serial.print(pitch);
    Serial.print(",");
    Serial.println(yaw);
    */

    //Serial.println();

    vTaskDelay(50);
  }
}

void rfReceiver(void *parameter) {
  pinMode(2, OUTPUT);
  int i = 0;

  while (1) {
    int j = 255;
    if (i < j) {j = i;}
    digitalWrite(2, i/128 == 1 ? 0 : 1);
    driveAll(j);
    Serial.print("Speed ");
    Serial.println(i);
    i++;
    if (i == 300) i = 0;
    
    vTaskDelay(250);
        
    /*
    if (rfCommandAvailable()) {
      RfCommand command = rfReceiveCommand();
      
      switch (command.type) {
        case STOP:
          Serial.println("C: STOP");
          digitalWrite(2, LOW);
          driveAll(0);
          break;

        case START:
          Serial.println("C: START");
          digitalWrite(2, HIGH);
          driveAll(0);
          break;

        case MOVE:
          Serial.print("C: MOVE ");
          Serial.print(command.x);
          Serial.print(",");
          Serial.print(command.y);
          Serial.print(";");
          Serial.print(command.up);
          Serial.print(",");
          Serial.println(command.down);
          Serial.println((int) abs(command.x)*2);
          driveAll(abs(command.x)*2);
          break;
      }
    }

    vTaskDelay(5);
    */
  }
}

void setup() {
  Serial.begin(19200);
  //sensorSetup(PIN_SENSOR_SDA, PIN_SENSOR_SCL);
  rfSenderSetup(PIN_RF_RX, PIN_RF_TX);
  motorSetup(PIN_MOTOR_FL, PIN_MOTOR_FR, PIN_MOTOR_BL, PIN_MOTOR_BR);

  vTaskDelay(1000);

/*
  xTaskCreate(
    drive,
    "Drive Motors",
    2048,
    NULL,
    2,
    NULL);
  */

  xTaskCreate(
    rfReceiver,
    "RF Receiver",
    4096,
    NULL,
    1,
    NULL);

  vTaskDelete(NULL);  // Delete setup and loop task
}

void loop() {
}


// ========================================================================


#else

#include "rf.h"
#include "nunchuck.h"

#define PIN_NUNCHUCK_SDA 21
#define PIN_NUNCHUCK_SCL 22
#define PIN_RF_RX 16
#define PIN_RF_TX 17

static NunchuckState prevState = {};
static bool isFlying = false;
static bool startStopLock = false;

static bool differentState(NunchuckState &a, NunchuckState &b) {
  if (a.c && a.z && b.c && b.z) return false;
  if (a.x != b.x) return true;
  if (a.y != b.y) return true;
  if (a.c != b.c) return true;
  if (a.z != b.z) return true;
  return false;
}

void controller(void *parameter) {
  NunchuckState state;

  while (1) {
    esp_err_t res = requestNunchuckState(state);
    if (!startStopLock) {
      if (state.c && state.z) {
        startStopLock = true;
        if (isFlying) {
          Serial.println("STOP");
          rfSendStop();
        } else {
          Serial.println("START");
          rfSendStart();
        }
        isFlying = !isFlying;
      } else {
        if (isFlying && differentState(prevState, state)) {
          Serial.printf("MOVE %4d, %4d : %s\n", state.x, state.y, state.c?"/\\" : (state.z?"\\/":"--"));
          rfSendMove(state.x, state.y, state.c, state.z);
        }
      }
    } else {
      if (!state.c && !state.z) {
        startStopLock = false;
      }
    }
    prevState = state;
    vTaskDelay(10);
  }
}

void setup() {
  Serial.begin(19200);
  nunchuckSetup(PIN_NUNCHUCK_SDA, PIN_NUNCHUCK_SCL);
  rfSenderSetup(PIN_RF_RX, PIN_RF_TX);

  xTaskCreate(
    controller,
    "Controller",
    4096,
    NULL,
    1,
    NULL);

  vTaskDelete(NULL);  // Delete setup and loop task
}

void loop() {
}

#endif