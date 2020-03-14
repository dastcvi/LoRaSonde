#include "LoRaSonde.h"

LoRaSonde loraSonde;

void setup()
{
  // set these before calling initialize
  SetModemConfig(BW_63, CR_4_5, SF_9);
  SetTXPower(TX_MAX_POWER);
  SetFrequency(921);

  // set if the LoRaSonde should downlink data from a connected iMet
  iMet = false;

  // start the radio with the configured parameters
  loraSonde.Initialize();
}

void loop()
{
  loraSonde.Run();
}
