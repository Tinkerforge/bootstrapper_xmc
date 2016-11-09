#!/usr/bin/env python3

CONFIG_BAUDRATE   = 19200 #115200 # 115200 is maximum for standard bootstrap loader
CONFIG_TTY        = '/dev/ttyUSB0'
CONFIG_FIRMWARE   = 'fw.bin'

CONFIG_UID_IQR    = 'uf8'
CONFIG_UID_MASTER = '6qzRzc'

import time
import os
import sys
from subprocess import Popen, PIPE
import fcntl

USBDEVFS_RESET = 21780

MASK_NONE  = 0b0000
MASK_POWER = 0b0001
MASK_DATA  = 0b0010

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_industrial_quad_relay import BrickletIndustrialQuadRelay
from tinkerforge.brick_master import BrickMaster
from xmc_flash import xmc_flash

def reset_usb():
    try:
        lsusb_out = Popen('lsusb | grep -i "Future Technology Devices International, Ltd FT232 USB-Serial"', shell=True, bufsize=64, stdin=PIPE, stdout=PIPE, close_fds=True).stdout.read().strip().split()
        bus = lsusb_out[1]
        device = lsusb_out[3][:-1]
        f = open("/dev/bus/usb/%s/%s"%(bus, device), 'w', os.O_WRONLY)
        fcntl.ioctl(f, USBDEVFS_RESET, 0)
    except Exception as msg:
        print("failed to reset device:" + msg)

def relay_flash(baudrate, tty, firmware, uid_iqr, uid_master):
    ipcon = IPConnection()
    iqr = BrickletIndustrialQuadRelay(uid_iqr, ipcon)
    master = BrickMaster(uid_master, ipcon)

    ipcon.connect('localhost', 4223)

#    reset_usb()
#    time.sleep(1)
    master.get_chibi_error_log()
    iqr.set_value(MASK_NONE)
    time.sleep(0.25)
    iqr.set_value(MASK_POWER | MASK_DATA)

    for _ in range(10):
        try:
            xmc_flash(baudrate, tty, firmware)
            break
        except Exception as e:
            print(str(e))
        
    iqr.set_value(MASK_POWER)

    master.reset()

    ipcon.disconnect()

if __name__ == '__main__':
    relay_flash(CONFIG_BAUDRATE, CONFIG_TTY, CONFIG_FIRMWARE, CONFIG_UID_IQR, CONFIG_UID_MASTER)
