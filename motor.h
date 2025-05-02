#ifndef MOTOR_HEADER
#define MOTOR_HEADER

void motorSetup(int pin_fl, int pin_fr, int pin_bl, int pin_br);

void driveFl(int val);
void driveFr(int val);
void driveBl(int val);
void driveBr(int val);
void driveAll(int val);

#endif // MOTOR_HEADER