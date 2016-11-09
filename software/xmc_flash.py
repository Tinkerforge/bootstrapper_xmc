#!/usr/bin/env python3

# Test program for simple bootstrapper/firmware flashing
# Copy firmware as "fw.bin" in software folder.
# Configure variables below as necessary

CONFIG_BAUDRATE = 19200 #115200 # 115200 is maximum for standard bootstrap loader
CONFIG_TTY      = '/dev/ttyACM0'
CONFIG_FIRMWARE = 'fw.bin'


import serial
import time

BSL_START  = [0x00]
BSL_ASC_F  = [0x6C]
BSL_ASC_H  = [0x12]
BSL_ENC_F  = [0x93]
BSL_ENC_H  = [0xED]

BSL_BR_OK  = [0xF0]
BSL_ID     = [0x5D]
BSL_ENC_ID = [0xA2]
BSL_BR_OK  = [0xF0]
BSL_OK     = [0x01]
BSL_NOK    = [0x02]

def xmc_flash(baudrate, tty, firmware):
    with serial.Serial() as s:
        s = serial.Serial()
        s.baudrate = baudrate
        s.port = tty
        s.timeout = 0.25
        s.open()

        print("Opening " + str(s.name))

        s.write(BSL_START)
        s.write(BSL_ASC_F)
        print("start read")
        ret = s.read(1)
        if len(ret) < 1:
            raise Exception("Handshake error: No answer")
        elif ord(ret) != BSL_ID[0]:
            raise Exception("Handshake error, received: " + hex(ord(ret)))

        bs = open('build/bootstrapper.bin', 'rb').read()
        length = len(bs)
        length_write = [length & 0xFF, (length >> 8) & 0xFF, (length >> 16) & 0xFF, (length >> 24) & 0xFF]

        print("Writing bootstrapper length " + str(length_write))
        s.write(length_write)
        ret = s.read(1)

        if len(ret) < 1:
            raise Exception("Write bootstrapper length error: No answer")
        elif ord(ret) != BSL_OK[0]:
            raise Exception("Write bootstrapper length error, received: " + hex(ord(ret)))

        print("Writing bootstrapper")
        s.write(bs)
        ret = s.read(1)

        if len(ret) < 1:
            raise Exception("Write bootstrapper error: No answer")
        elif ord(ret) != BSL_OK[0]:
            raise Exception("Write bootstrapper error, received: " + hex(ord(ret)))

        print("Writing bootstrapper done")
    
        s.flush()

        fw = open(firmware, 'rb').read()

        pages = len(fw)//256
        for page in range(pages):
            print("Write firmware page " + str(page))
            s.write(fw[page*256:(page+1)*256])
            
            if page == pages-1: # Last page does not return CRC
                break

            ret_crc = s.read(1)
            crc = 0
            for i in range(256):
                crc = crc ^ ord(fw[page*256+i])

            print("Read CRC (mcu vs calc): {0} vs {1}".format(ord(ret_crc), crc))
            if crc != ord(ret_crc):
                raise Exception("CRC Error")


if __name__ == '__main__':
    xmc_flash(CONFIG_BAUDRATE, CONFIG_TTY, CONFIG_FIRMWARE)
