#include "rf.h"
#include "Arduino.h"

// ====== SENDER ======

static TimerHandle_t led_timer = NULL;

static void ledOff(TimerHandle_t xTimer) {
  digitalWrite(2, LOW);
}

void rfSenderSetup(int rx_pin, int tx_pin) {
  Serial2.begin(9600, SERIAL_8N1, rx_pin, tx_pin);
  pinMode(2, OUTPUT);

  led_timer = xTimerCreate(
    "RF Send LED",
    50 / portTICK_PERIOD_MS,
    pdFALSE,
    (void *)0,
    ledOff);
}

static void rfSendCommand(RfCommand &command) {
  Serial2.write((uint8_t *) &command, sizeof(RfCommand));
  digitalWrite(2, HIGH);
  xTimerStart(led_timer, portMAX_DELAY);
}

void rfSendMove(int x, int y, bool up, bool down) {
  RfCommand command {
    MOVE,
    x,
    y,
    up,
    down
  };
  rfSendCommand(command);
}

void rfSendStop() {
  RfCommand command {
    STOP,
    0,
    0,
    0,
    0
  };
  rfSendCommand(command);
}

void rfSendStart() {
  RfCommand command {
    START,
    0,
    0,
    0,
    0
  };
  rfSendCommand(command);
}

// ====== RECEIVER ======

void rfReceiverSetup(int rx_pin, int tx_pin) {
  Serial2.begin(9600, SERIAL_8N1, rx_pin, tx_pin);
}

bool rfCommandAvailable() {
  return Serial2.available() >= sizeof(RfCommand);
}

RfCommand rfReceiveCommand() {
  RfCommand command;
  Serial2.readBytes((uint8_t *) &command, sizeof(RfCommand));
  return command;
}