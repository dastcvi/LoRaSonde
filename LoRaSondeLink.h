/*
 * LoRaSondeProtocol.h
 * Author:  Alex St. Clair
 * Created: February 2020
 *
 * LoRaSondeProtocol defines the message types on the link
 */

#include "RH_RF95.h"

// configuration defaults
#define DEFAULT_FREQUENCY       915 // MHz
#define DEFAULT_TXPOWER         5
#define DEFAULT_MODEMCONFIG     RH_RF95::Bw125Cr45Sf128

enum LoRaSondeMessage_t : uint8_t {
    LORA_XDATA,
    LORA_PTUX,
    LORA_GPS
};