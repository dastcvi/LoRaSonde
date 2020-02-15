/*
 * LoRaSonde.h
 * Author:  Alex St. Clair
 * Created: February 2020
 *
 * LoRaSonde operates using the NOAA XDATA protocol to receive balloon instrument (and iMet)
 * data over a TTL UART connection and transmit it to a ground station
 *
 * Hardware: SparkFun SAMD21 Pro RF
 */

#ifndef LORASONDE_H
#define LORASONDE_H

#include "RH_RF95.h"
#include <Arduino.h>

#define INST_SERIAL SerialUSB

// configuration defaults
#define DEFAULT_FREQUENCY       915 // MHz
#define DEFAULT_TXPOWER         23
#define DEFAULT_MODEMCONFIG     RH_RF95::Bw31_25Cr48Sf512

class LoRaSonde {
public:
    LoRaSonde();
    ~LoRaSonde() { };

    void Initialize();
    void Run();

    RH_RF95 rf95;

private:
    void ListenInstrument();

    // ascii buffers
    char xdata_ascii[128] = {0};
    char ptux_buffer[128] = {0};
    char gps_buffer[128] = {0};

    // ascii buffer pointers
    uint8_t xdata_len = 0;
    uint8_t ptux_len = 0;
    uint8_t gps_len = 0;

    // tx binary buffer
    uint8_t bin_buffer[64] = {0};
    uint8_t bin_len = 0;

    const int LED_PIN = 13;

    // configuration variables
    float frequency = DEFAULT_FREQUENCY;
    int8_t tx_power = DEFAULT_TXPOWER;
    RH_RF95::ModemConfigChoice modem_config = DEFAULT_MODEMCONFIG;
};

#endif /* LORASONDE_H */