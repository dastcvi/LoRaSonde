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
#define IMET_SERIAL Serial1

class LoRaSonde {
public:
    LoRaSonde();
    ~LoRaSonde() { };

    void Initialize();
    void Run();

    RH_RF95 rf95;

private:
    bool ListenInstrument();
    void ListeniMet();

    void ReadPTUX(unsigned long timeout);
    void ReadGPS(unsigned long timeout);

    bool ReadChars(unsigned long timeout, char * buffer, uint8_t n, char delim, char delim2 = '\0');
    bool WaitSpace(unsigned long timeout);

    void WriteiMetToPacket();
    void SendPacket();

    // iMet data structs
    PTUX_t ptux = {0};
    GPS_t gps = {0};

    // ascii buffer
    char ascii_buf[512] = {0};

    // tx binary buffer
    uint8_t tx_buf[RH_RF95_MAX_MESSAGE_LEN] = {0};
    uint16_t tx_len = 0;

    const int LED_PIN = 13;

};

#endif /* LORASONDE_H */