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

#include "LoRaSondeLink.h"
#include <Arduino.h>

#define INST_SERIAL SerialUSB

class LoRaSonde {
public:
    LoRaSonde();
    ~LoRaSonde() { };

    void Initialize();
    void Run();

    RH_RF95 rf95;

private:
    bool ListenInstrument();
    bool ListeniMet();
    void SendPacket();

    // ascii buffers
    char xdata_ascii[512] = {0};
    char ptux_buffer[512] = {0};
    char gps_buffer[512] = {0};

    // ascii buffer pointers
    uint8_t xdata_len = 0;
    uint8_t ptux_len = 0;
    uint8_t gps_len = 0;

    // tx binary buffer
    uint8_t tx_buf[RH_RF95_MAX_MESSAGE_LEN] = {0};
    uint8_t tx_len = 0;

    const int LED_PIN = 13;

};

#endif /* LORASONDE_H */