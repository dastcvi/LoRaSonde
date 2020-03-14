#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import serial
from datetime import datetime

# open the serial port of the connected device
ser = serial.Serial('COM18', timeout=1)

# get the current date and time in string format
current_datetime = datetime.now()
date = str(current_datetime.date().strftime("%d-%b-%y"))
start_time = str(current_datetime.time().strftime("%H-%M-%S"))

# create and open the log file
filename = "lora_data_" + date + "_" + start_time + ".txt"
log_file = open(filename, 'w', encoding = 'utf-8')

# write a log file header
log_file.write("XDATA Log " + date + "\n")

# print a welcome message
print("\nStartup:   " + date)
print("Data File: " + filename + "\n")

# wait for new xdata lines, print them to the terminal and the file
while (True):
    try:
        # read the next line of data from serial
        line = str(ser.readline(), "ascii")

        # pass on empty strings
        if not line:
            continue

        # remove the \r and \n from the line
        line = line[:-2]

        # get the current time
        time = datetime.now().time().strftime("%H:%M:%S")

        # display the time and line to the terminal
        print(time + ": ", end="")
        print(line)

        # write the time and line to the log file
        log_file.write(time + ": ")
        log_file.write(line + "\n")

    except:
        # graceful exit for KeyboardInterrupt, radio unplugged, etc.
        print("\nExiting and closing file")
        log_file.close()
        quit()