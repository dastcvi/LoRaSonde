/*
 * LoRaSondeProtocol.cpp
 * Author:  Alex St. Clair
 * Created: February 2020
 *
 * LoRaSondeProtocol defines the message types and the link type
 */

#include "LoRaSondeLink.h"

// link configuration defaults
RH_RF95::ModemConfig modem_config = {0x72, 0x74, 0x00}; // chip default
float frequency = 915;
LoRaTXPower_t tx_power = TX_23dBm;

// is an iMet connected to the LoRaSonde?
bool iMet = false;

void SetModemConfig(LoRaBandwidth_t bw_in, LoRaCodingRate_t cr_in, LoRaSpreadingFactor_t sf_in)
{
    modem_config.reg_1d = (bw_in << 4) | (cr_in << 1);
    modem_config.reg_1e = (sf_in << 4) | 0x4;
    modem_config.reg_26 = 0;
}

void SetFrequency(float freq_in)
{
    frequency = freq_in;
}

void SetTXPower(LoRaTXPower_t power_in)
{
    tx_power = power_in;
}