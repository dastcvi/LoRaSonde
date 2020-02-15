/*
 * LoRaSonde.cpp
 * Author:  Alex St. Clair
 * Created: February 2020
 *
 * LoRaSonde operates using the NOAA XDATA protocol to receive balloon instrument (and iMet)
 * data over a TTL UART connection and transmit it to a ground station
 *
 * Hardware: SparkFun SAMD21 Pro RF
 */

#include "LoRaSonde.h"

LoRaSonde::LoRaSonde()
    : rf95(12, 6) // defined by Pro RF hardware
{ }

void LoRaSonde::Initialize()
{
    SerialUSB.begin(115200);
    INST_SERIAL.begin(9600);
    INST_SERIAL.setTimeout(100);
    pinMode(LED_PIN, OUTPUT);

    //Initialize the Radio.
    if (!rf95.init()) {
        SerialUSB.println("Radio Error!");
        while(1) {
            digitalWrite(LED_PIN, HIGH); delay(250);
            digitalWrite(LED_PIN, LOW); delay(250);
        }
    }

    // Set frequency
    rf95.setFrequency(frequency);
    rf95.setTxPower(tx_power, false);
    rf95.setModemConfig(modem_config);

    digitalWrite(LED_PIN, HIGH);
}

void LoRaSonde::Run()
{
    ListenInstrument();
}

void LoRaSonde::ListenInstrument()
{
    static char header[7] = {0};
    static char hex_read[3] = {0};
    static unsigned long timeout = 0;

    // reset the buffers
    xdata_len = 0;
    bin_len = 0;

    // wait for an 'x'
    while (INST_SERIAL.available() && 'x' != INST_SERIAL.peek()) INST_SERIAL.read();

    // if we didn't get an 'x', return
    if ('x' != (header[0] = INST_SERIAL.read())) return;

    // listen for the rest of the header "xdata="
    timeout = millis() + 200;
    for (int i = 1; i < 6; i++) {
        while (!INST_SERIAL.available()) {
            if (millis() > timeout) {
                INST_SERIAL.flush();
                return;
            }
        }

        header[i] = INST_SERIAL.read();
    }

    // if we didn't get a good header, flush and return
    if (0 != strcmp(header, "xdata=")) {
        INST_SERIAL.flush();
        return;
    }

    // get the message
    while ('\r' != (char) INST_SERIAL.peek() && '\n' != (char) INST_SERIAL.peek()) {
        if (millis() > timeout) {
            INST_SERIAL.flush();
            return;
        }

        if (INST_SERIAL.available()) {
            xdata_ascii[xdata_len++] = (char) INST_SERIAL.read();
        }
    }

    // make sure we have an even number of bytes in the packet
    if (0 != xdata_len % 2) {
        SerialUSB.println("Bad packet");
        return;
    }

    // convert the "ASCII-encoded hex" to uint8s
    bin_len = xdata_len/2;
    for (int i = 0; i < bin_len; i++) {
        hex_read[0] = xdata_ascii[2*i];
        hex_read[1] = xdata_ascii[2*i+1];

        bin_buffer[i] = (uint8_t) strtoul(hex_read, 0, 16);
    }

    // TODO: Send packet here
}
