#ifndef RF_HEADER
#define RF_HEADER

enum CommandType {
  MOVE,
  STOP,
  START
};

struct RfCommand {
  CommandType type;
  int x;
  int y;
  bool up;
  bool down;
};

void rfSenderSetup(int rx_pin, int tx_pin);

void rfSendMove(int x, int y, bool up, bool down);

void rfSendStop();

void rfSendStart();

void rfReceiverSetup(int rx_pin, int tx_pin);

bool rfCommandAvailable();

RfCommand rfReceiveCommand();

#endif // RF_HEADER