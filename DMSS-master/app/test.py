#!/usr/bin/env python

from socket import timeout
from time import sleep
import minimalmodbus

instrument = minimalmodbus.Instrument('COM4', 1)  # port name, slave address (in decimal)
instrument.serial.baudrate = 9600
instrument.serial.timeout = 0.5

while(True):

    ## Read temperature (PV = ProcessValue) ##
   ## print( instrument.read_register(0x00, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x01, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x02, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x03, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x04, 0, functioncode=4))  # Registernumber, number of decimals

    sleep(2)
