#include "LoRaSondeGround.h"

LoRaSondeGround loraSondeGround;

void setup()
{
  // set these before calling initialize
  SetModemConfig(BW_500, CR_4_5, SF_12);
  SetTXPower(TX_6dBm);
  SetFrequency(921);

  // set if the LoRaSonde should downlink data from a connected iMet
  iMet = true;

  // start the radio with the configured parameters
  loraSondeGround.Initialize();
}

void loop()
{
  loraSondeGround.Run();
}