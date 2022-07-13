#!/usr/bin/env python

from time import sleep
import minimalmodbus

instrument = minimalmodbus.Instrument('COM7', 1)  # port name, slave address (in decimal)
instrument.serial.baudrate = 9600

while(True):

    ## Read temperature (PV = ProcessValue) ##
    print( instrument.read_register(0x0, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x1, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x2, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x3, 0, functioncode=4))  # Registernumber, number of decimals
    print( instrument.read_register(0x4, 0, functioncode=4))  # Registernumber, number of decimals

    sleep(2)