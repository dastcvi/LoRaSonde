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
#include "Serialize.h" // from SerialComm library

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

    // set to the buffer size for recv, which will set to actual size
    receive_size = RH_RF95_MAX_MESSAGE_LEN;

    if (!rf95.recv(rx_buf, &receive_size)) {
        OUTPUT_SERIAL.println("Packet error");
        return;
    }

    // reset the index for message printing
    rx_index = 0;

    // if an iMet is connected to the LoRaSonde, print that data
    if (iMet) {
        if (!PrintPTUX() || !PrintGPS()) {
            OUTPUT_SERIAL.println("Message error");
        }
    }

    PrintXDATA();
}

bool LoRaSondeGround::PrintPTUX()
{
    float temp;

    OUTPUT_SERIAL.print("PTUX: ");

    // deserialize and print P
    if (!BufferGetFloat(&temp, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(", ");

    // deserialize and print T
    if (!BufferGetFloat(&temp, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(", ");

    // deserialize and print U
    if (!BufferGetFloat(&temp, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(", ");

    // deserialize and print X
    if (!BufferGetFloat(&temp, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.println("\r");

    return true;
}

bool LoRaSondeGround::PrintGPS()
{
    float temp;
    uint8_t temp_u;

    OUTPUT_SERIAL.print("GPS: ");

    // deserialize and print lat
    if (!BufferGetFloat(&temp, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(", ");

    // deserialize and print lon
    if (!BufferGetFloat(&temp, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(", ");

    // deserialize and print alt
    if (!BufferGetFloat(&temp, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(", ");

    // deserialize and print qual
    if (!BufferGetUInt8(&temp_u, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(", ");

    // deserialize and print hours
    if (!BufferGetUInt8(&temp_u, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(":");

    // deserialize and print minutes
    if (!BufferGetUInt8(&temp_u, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.print(":");

    // deserialize and print seconds
    if (!BufferGetUInt8(&temp_u, rx_buf, receive_size, &rx_index)) return false;
    OUTPUT_SERIAL.print(temp);
    OUTPUT_SERIAL.println("\r");

    return true;
}

void LoRaSondeGround::PrintXDATA()
{
    char hex_str[3] = {0};

    OUTPUT_SERIAL.print("xdata=");

    // print all but the first (protocol) byte as ASCII hex
    while (rx_index < receive_size) {
        sprintf(hex_str, "%02X", rx_buf[rx_index++]);
        OUTPUT_SERIAL.print(hex_str);
    }

    OUTPUT_SERIAL.println("\r");
}