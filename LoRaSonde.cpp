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
#include "Serialize.h" // from SerialComm library

// configure the IMET SERCOM
Uart Serial2(IMET_SERCOM, IMET_RX, IMET_TX, IMET_RX_PAD, IMET_TX_PAD);

void SERCOM2_Handler()
{
  Serial2.IrqHandler();
}

LoRaSonde::LoRaSonde()
    : rf95(12, 6) // defined by Pro RF hardware
{ }

void LoRaSonde::Initialize()
{
    SerialUSB.begin(115200);
    INST_SERIAL.begin(9600);
    INST_SERIAL.setTimeout(100);
    IMET_SERIAL.begin(9600);
    IMET_SERIAL.setTimeout(100);
    pinMode(LED_PIN, OUTPUT);

    pinPeripheral(IMET_TX, PIO_SERCOM);
    pinPeripheral(IMET_RX, PIO_SERCOM);

    //Initialize the Radio.
    if (!rf95.init()) {
        SerialUSB.println("Radio Error!");
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

void LoRaSonde::Run()
{
    if (ListenInstrument()) SendPacket();
    if (iMet) ListeniMet();
}

bool LoRaSonde::ListenInstrument()
{
    char header[7] = {0};
    char hex_read[3] = {0};
    unsigned long timeout = 0;
    uint16_t xdata_len = 0;
    uint8_t xdata_base = 0;

    // wait for an 'x'
    while (INST_SERIAL.available() && 'x' != INST_SERIAL.peek()) INST_SERIAL.read();

    // if we didn't get an 'x', return
    if ('x' != (header[0] = INST_SERIAL.read())) return false;

    // listen for the rest of the header "xdata="
    timeout = millis() + 200;
    for (int i = 1; i < 6; i++) {
        while (!INST_SERIAL.available()) {
            if (millis() > timeout) {
                INST_SERIAL.flush();
                return false;
            }
        }

        header[i] = INST_SERIAL.read();
    }

    // if we didn't get a good header, flush and return
    if (0 != strcmp(header, "xdata=")) {
        INST_SERIAL.flush();
        return false;
    }

    // get the message
    while ('\r' != (char) INST_SERIAL.peek() && '\n' != (char) INST_SERIAL.peek()) {
        if (millis() > timeout) {
            INST_SERIAL.flush();
            return false;
        }

        if (INST_SERIAL.available()) {
            ascii_buf[xdata_len++] = (char) INST_SERIAL.read();
        }
    }

    // make sure we have an even number of bytes in the packet
    if (0 != xdata_len % 2) {
        SerialUSB.println("Bad packet");
        return false;
    }

    // refresh the tx buffer
    tx_len = 0;

    // if an iMet is connected, add the most recent data to the packet
    if (iMet) {
        WriteiMetToPacket();
    }

    // buffer base address for the xdata
    xdata_base = tx_len;

    // ensure that the packet isn't too big
    if (RH_RF95_MAX_MESSAGE_LEN < tx_len + xdata_len/2) return false;

    // update the transmit size with the xdata length
    tx_len += xdata_len/2;

    // convert the "ASCII-encoded hex" to uint8s
    for (uint16_t i = 0; i < xdata_len/2; i++) {
        hex_read[0] = ascii_buf[2*i];
        hex_read[1] = ascii_buf[2*i+1];

        tx_buf[xdata_base + i] = (uint8_t) strtoul(hex_read, 0, 16);
    }

    // signal that a packet has been received and parsed
    return true;
}

void LoRaSonde::ListeniMet()
{
    char header[7] = {0};
    uint8_t header_len = 0;
    unsigned long timeout = 0;

    // wait for a 'P' or 'G'
    while (IMET_SERIAL.available() && 'P' != IMET_SERIAL.peek() && 'G' != IMET_SERIAL.peek()) IMET_SERIAL.read();

    // if we didn't get a 'P' or 'G', return
    header[0] = IMET_SERIAL.read();
    if ('P' != header[0] && 'G' != header[0]) return;

    // listen for the rest of the header "PTUX: " or "GPS: "
    header_len = ('P' == header[0]) ? 6 : 5;
    timeout = millis() + 200;
    for (int i = 1; i < header_len; i++) {
        while (!IMET_SERIAL.available()) {
            if (millis() > timeout) {
                IMET_SERIAL.flush();
                return;
            }
        }

        header[i] = IMET_SERIAL.read();
    }

    // now parse the right message if correct, otherwise flush
    if (0 == strcmp(header, "PTUX: ")) {
        ReadPTUX(timeout);
    } else if (0 == strcmp(header, "GPS: ")) {
        ReadGPS(timeout);
    } else {
        INST_SERIAL.flush();
    }
}

void LoRaSonde::WriteiMetToPacket()
{
    // PTUX
    BufferAddUInt16(ptux.deciseconds, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddFloat(ptux.p, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddFloat(ptux.t, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddFloat(ptux.u, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddFloat(ptux.x, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);

    // GPS
    BufferAddUInt16(gps.deciseconds, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddFloat(gps.lat,  tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddFloat(gps.lon,  tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddFloat(gps.alt,  tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddUInt8(gps.qual, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddUInt8(gps.hour, tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddUInt8(gps.min,  tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
    BufferAddUInt8(gps.sec,  tx_buf, RH_RF95_MAX_MESSAGE_LEN, &tx_len);
}

void LoRaSonde::SendPacket()
{
    // ensure that the transmitter is free before sending
    rf95.waitPacketSent();

    // attempt to send the packet
    if (!rf95.send(tx_buf, tx_len)) {
        SerialUSB.println("Send error");
        return;
    }

    SerialUSB.println("Sent packet");
}

void LoRaSonde::ReadPTUX(unsigned long timeout)
{
    char param[8] = "";

    // get P
    if (!ReadChars(timeout, param, 7, ',')) return;
    ptux.p = strtof(param, NULL);
    if (!WaitSpace(timeout)) return;

    // get T
    if (!ReadChars(timeout, param, 7, ',')) return;
    ptux.t = strtof(param, NULL);
    if (!WaitSpace(timeout)) return;

    // get U
    if (!ReadChars(timeout, param, 7, ',')) return;
    ptux.u = strtof(param, NULL);
    if (!WaitSpace(timeout)) return;

    // get X
    if (!ReadChars(timeout, param, 7, '\r', '\n')) return;
    ptux.x = strtof(param, NULL);

    ptux.deciseconds = (uint16_t) (millis() / 100);

    SerialUSB.println("PTUX");
}

void LoRaSonde::ReadGPS(unsigned long timeout)
{
    char param[12] = "";

    // get lat
    if (!ReadChars(timeout, param, 11, ',')) return;
    gps.lat = strtof(param, NULL);
    if (!WaitSpace(timeout)) return;

    // get lon
    if (!ReadChars(timeout, param, 11, ',')) return;
    gps.lon = strtof(param, NULL);
    if (!WaitSpace(timeout)) return;

    // get alt
    if (!ReadChars(timeout, param, 11, ',')) return;
    gps.alt = strtof(param, NULL);
    if (!WaitSpace(timeout)) return;

    // get qual
    if (!ReadChars(timeout, param, 3, ',')) return;
    gps.qual = strtoul(param, NULL, 10);
    if (!WaitSpace(timeout)) return;

    // get hours
    if (!ReadChars(timeout, param, 3, ':')) return;
    gps.hour = strtoul(param, NULL, 10);

    // get mins
    if (!ReadChars(timeout, param, 3, ':')) return;
    gps.min = strtoul(param, NULL, 10);

    // get secs
    if (!ReadChars(timeout, param, 3, '\r', '\n')) return;
    gps.sec = strtoul(param, NULL, 10);

    gps.deciseconds = (uint16_t) (millis() / 100);

    SerialUSB.println("GPS");
}

inline bool LoRaSonde::ReadChars(unsigned long timeout, char * buffer, uint8_t n, char delim, char delim2)
{
    // read up to n bytes
    for (uint8_t i = 0; i < n; i++) {
        // wait for a byte to be available unless the timeout is reached
        while (!IMET_SERIAL.available()) {
            if (millis() > timeout) {
                IMET_SERIAL.flush();
                return false;
            }
        }

        // check if the next character is one of our desired delimiters
        if (delim == IMET_SERIAL.peek() || delim2 == IMET_SERIAL.peek()) {
            IMET_SERIAL.read(); // clear the char
            return true;
        }

        // add the character to the buffer
        buffer[i] = IMET_SERIAL.read();
    }

    // if we made it here, the delimiter was never reached
    return false;
}

inline bool LoRaSonde::WaitSpace(unsigned long timeout)
{
    while (!IMET_SERIAL.available()) {
        if (millis() > timeout) {
            IMET_SERIAL.flush();
            return false;
        }
    }

    return (' ' == IMET_SERIAL.read());
}