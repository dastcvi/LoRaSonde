/*
 * LoRaSondeGround.h
 * Author:  Alex St. Clair
 * Created: February 2020
 *
 * LoRaSondeGround is the ground station portion of the LoRaSonde link
 *
 * Hardware: SparkFun SAMD21 Pro RF
 */

#ifndef LORASONDEGROUND_H
#define LORASONDEGROUND_H

#include "LoRaSondeLink.h"
#include <Arduino.h>

#define OUTPUT_SERIAL   SerialUSB

class LoRaSondeGround {
public:
    LoRaSondeGround();
    ~LoRaSondeGround() { };

    void Initialize();
    void Run();

    RH_RF95 rf95;

private:
    void PrintXDATA();
    void PrintPTUX();
    void PrintGPS();

    uint8_t rx_buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t rx_len = RH_RF95_MAX_MESSAGE_LEN;

    const int LED_PIN = 13;

};

#endif /* LORASONDEGROUND_H */