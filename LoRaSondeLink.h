/*
 * LoRaSondeProtocol.h
 * Author:  Alex St. Clair
 * Created: February 2020
 *
 * LoRaSondeProtocol defines the message types and the link type
 */

#include "RH_RF95.h"

// message types for sonde -> ground communication, stored as first byte
enum LoRaSondeMessage_t : uint8_t {
    LORA_XDATA,
    LORA_PTUX,
    LORA_GPS
};

// valid transmit powers
enum LoRaTXPower_t : uint8_t {
    TX_5dBm  = 5,
    TX_6dBm,
    TX_7dBm,
    TX_8dBm,
    TX_9dBm,
    TX_10dBm,
    TX_11dBm,
    TX_12dBm,
    TX_13dBm,
    TX_14dBm,
    TX_15dBm,
    TX_16dBm,
    TX_17dBm,
    TX_18dBm,
    TX_19dBm,
    TX_20dBm,
    TX_21dBm,
    TX_22dBm,
    TX_23dBm
};

// valid bandwidth settings (in kHz)
enum LoRaBandwidth_t : uint8_t {
    BW_8,   // 7.8   kHz
    BW_10,  // 10.4  kHz
    BW_16,  // 15.6  kHz
    BW_21,  // 20.8  kHz
    BW_31,  // 31.25 kHz
    BW_42,  // 41.7  kHz
    BW_63,  // 62.5  kHz
    BW_125, // 125   kHz
    BW_250, // 250   kHz
    BW_500  // 500   kHz
};

// valid coding rates
enum LoRaCodingRate_t : uint8_t {
    CR_4_5 = 1,
    CR_4_6,
    CR_4_7,
    CR_4_8
};

// valid spreading factors
enum LoRaSpreadingFactor_t : uint8_t {
    SF_6 = 6,
    SF_7,
    SF_8,
    SF_9,
    SF_10,
    SF_11,
    SF_12
};

// link configuration variables
extern RH_RF95::ModemConfig modem_config;
extern float frequency;
extern LoRaTXPower_t tx_power;

// modem config requires constructing register bytes
void SetModemConfig(LoRaBandwidth_t bw_in, LoRaCodingRate_t cr_in, LoRaSpreadingFactor_t sf_in);

// set frequency in MHz
void SetFrequency(float freq_in);

// set tx power using valid value
void SetTXPower(LoRaTXPower_t power_in);
