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
#include "SERCOM.h"
#include "UART.h"
#include "wiring_private.h"
#include <Arduino.h>

// choose the serial ports for each stream
#define INST_SERIAL SerialUSB
#define IMET_SERIAL Serial2

// we need to configure the SERCOM module for the iMet serial
#define IMET_TX           2  /* PA14, 2.2 */
#define IMET_RX           5  /* PA15, 2.3 */
#define IMET_TX_PAD       (UART_TX_PAD_2)
#define IMET_RX_PAD       (SERCOM_RX_PAD_3)
#define IMET_SERCOM       (&sercom2)

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