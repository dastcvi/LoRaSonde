/*
 * LoRaSondeGround.cpp
 * Author:  Alex St. Clair
 * Created: February 2020
 *
 * LoRaSondeGround is the ground station portion of the LoRaSonde link
 *
 * Hardware: SparkFun SAMD21 Pro RF
 */

#include "LoRaSondeGround.h"

LoRaSondeGround::LoRaSondeGround()
    : rf95(12, 6) // defined by Pro RF hardware
{ }

void LoRaSondeGround::Initialize()
{
    OUTPUT_SERIAL.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    //Initialize the Radio.
    if (!rf95.init()) {
        OUTPUT_SERIAL.println("Radio Error!");
        while(1) {
            digitalWrite(LED_PIN, HIGH); delay(250);
            digitalWrite(LED_PIN, LOW); delay(250);
        }
    }

    // Set link parameters (variables defined in LoRaSondeLink)
    rf95.setFrequency(frequency);
    rf95.setTxPower(tx_power, false);
    rf95.setModemRegisters(&modem_config);

    digitalWrite(LED_PIN, HIGH);
}

void LoRaSondeGround::Run()
{
    if (!rf95.available()) return;

    rx_len = RH_RF95_MAX_MESSAGE_LEN;
    if (!rf95.recv(rx_buf, &rx_len)) {
        OUTPUT_SERIAL.println("Packet error");
        return;
    }

    switch (rx_buf[0])
    {
    case LORA_XDATA:
        PrintXDATA();
        break;
    case LORA_PTUX:
    case LORA_GPS:
    default:
        OUTPUT_SERIAL.println("Unknown packet type");
        break;
    }
}

void LoRaSondeGround::PrintXDATA()
{
    char hex_str[3] = {0};

    OUTPUT_SERIAL.print("xdata=");

    // print all but the first (protocol) byte as ASCII hex
    for (int i = 1; i < rx_len; i++) {
        sprintf(hex_str, "%02X", rx_buf[i]);
        OUTPUT_SERIAL.print(hex_str);
    }

    OUTPUT_SERIAL.println();
}