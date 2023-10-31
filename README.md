# Blink for STM-32

This is a test program to learn the very basics of the STM-32. In practice the
hardware I'm using is a nucleao-F401RE (MB1136).

To deploy, first call make, optionally setting the mode as below:

    make mode=debug

Note that the valid modes are debug and release. These are the same except that
debug has less optimisation. The default mode is release.

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

To allow debugging using the SWD, make sure you don't try to use pins PA13 or
PA14 for anything else. By default it's set up in hardware to allow debugging so
you should just be able to fire up openocd and gdb (arm version of gdb, that is)
and hopefully it'll just work. Additionally, this is needed in order to flash
while the program is running from flash memory.

If you fail to do so, in order to flash without an issue you'll have to make
sure the device isn't actively running from flash memory. To do that in my case
just stick a spare jumper between VDD and BOOT0, i.e. on pins 5 and 7 of CN7
(3rd and 4th from the top, left-hand side). That will result in the device
booting to SRAM (check the datasheet for details/to verify), allowing the flash
to be written.

In order to make use of the USART, use stty to set the speed to 115200 and the
mode to raw. Then in one terminal you can cat it out, and in another cat in
taking from the stdin. Alternatively, if you have screen you can just use that.
