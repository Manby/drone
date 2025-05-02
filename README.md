# drone
Software for ESP32-powered drone.

## Info
The drone is controlled by a Wii nunchuck wirelessly, using a HC-12 RF communication module. I2C allows the controller to talk to the Wii nunchuck.
An MPU6050 6-axis gyro+accelerometer sensor is used to stabilise the drone. I2C allows the ESP32 to talk to the sensor.
