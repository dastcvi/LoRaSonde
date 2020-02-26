# LoRaSonde
A LoRa RF link utilizing the SparkFun Pro RF and implementing the NOAA XDATA protocol. The intended use of the library is as a low-cost telemetry system for high altitude ballooning. Both the in-air and ground software are provided, and both portions use the SparkFun Pro RF.

*Disclaimer:* it is your resposibility to abide by the spectrum regulations in your country

## Hardware
SparkFun Pro RF: https://www.sparkfun.com/products/14916

## Arduino Driver Setup
https://learn.sparkfun.com/tutorials/sparkfun-samd21-pro-rf-hookup-guide

## Library Dependencies
RadioHead: https://github.com/PaulStoffregen/RadioHead

SerialComm: https://github.com/dastcvi/SerialComm

## Provided Software
LoRaSonde(.cpp/.h): class for the in-air part of the link

examples/LoRaSonde.ino: main Arduino file for the in-air segment

LoRaSondeGround(.cpp/.h): class for the ground station portion of the link

examples/LoRaSondeGround.ino: main Arduino file for the ground segment
