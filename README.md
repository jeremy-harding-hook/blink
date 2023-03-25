# Blink for STM-32

This is a test program to learn the very basics of the STM-32. In practice the
hardware I'm using is a nucleao-F401RE (MB1136).

To deploy, first call make, optionally setting the mode as below:

    make mode=debug

Note that the valid modes are debug, semihosted, and release. The idea is that
semihosted is good for debugging with some logging output and stuff but still
with optimisation as opposed to debug. Release on the other hand doesn't have
semihosting.

The makefile in addition to outputting a regular .elf file also does the
equivalent of running the command below from the output folder:

    arm-none-eabi-objcopy -O binary blink.elf blink.bin

To spare you the trouble of referencing the man page, this effectively copies
and reformats the file from an .elf file to a .bin file, which can be then
loaded onto the board. Note that arm-none-eabi-ld in the build step creates the
output file blink with the format elf32-littlearm; there is technically a linker
option to output a different format, but apparently it's broken and was for that
reason disabled in some prior version. If you try to use it now it just outputs
an error message.

After creating the binary file with objcopy, st-flash can be used to flash the
device as demonstrated below:

    st-flash write build/release/blink.bin 0x08000000

Additionally, there is a "flash" target which automates this command after the
build for maximum convenience.

Note that for successful flashing, you'll have to make sure the device isn't
actively running from flash. To do that in my case just stick a spare jumper
between VDD and BOOT0, i.e. on pins 5 and 7 of CN7 (3rd and 4th from the top,
left-hand side). That will result in the device booting to SRAM (check the
datasheet for details/to verify), allowing the flash to be written.