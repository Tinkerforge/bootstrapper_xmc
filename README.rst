Bootstrapper for Infineon XMC1x00 devices
=========================================

This bootstrapper is used to initially flash Bricklets with
Infineon XMC1x00 microcontroller.

It uses the default ASC BSL mode of the micro to flash a bootstrapper
which then has a simple interface to flash the bootloader and firmware.

To use this firmware you have to check out the bricklib2 git into the
software/src/ directory.

Note: This step is done internally at Tinkerforge once during production,
the firmware can later on be updated by the use through the Brick Viewer
with help of the (then installed) bootloader.
