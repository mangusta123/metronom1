metronom1
=========

LED drivers for BeagleBone board with metronome functionality

(more about this board on http://beagleboard.org/bone)

In order to compile executables and .ko kernel modules it is necessary to initialize environment

variables for cross-compiler/linker/assembler.

command: source env-init

In order to compile kernel modules it is necessary to have a Linux distribution, properly configured 

and built specifically for Bone model of BeagleBoard. Board model is important since kernel modules 

compiled by means of Linux distro built e.g. for BeagleBoard-xM cannot be deployed by BeagleBoard-Bone 

and vice versa. Path to Linux distro in Makefile is to be modified correspondingly. 

Compilation commands are specified in commands file.

Every executable makes use of kernel module from its folder so prior to using an executable it is needed

to load kernel module into BeagleBone Linux by Linux insmod command and associate device file with it.

Device file resides in /dev directory and its name is hardcoded into executable code (/dev/use_hellodev2).

Creating device file can be automated by writing a rule for udev.

Kernel module sources utilize generic Linux device driver conventions by implementing open() read() write() 

and close() interface functions used by executable to access board devices (LED and interrupt timer) through

device file. 

Executables provide command-line interface through serial connection. User may provide different 

metronome parameters like tempo and time-signature. LEDs are lit in two different ways: polling (3) which is

inefficient since the user must wait for LEDs to finish their flashing given by previous input parameters and

hence is unable to submit the new parameters, and interrupt (4) where LED I/O registers are checked at specific 

intervals on every timer interrupt so the user may update metronome parameters whenever he wants. 

Remote internet connection is accomplished by cross-compiling server side (5) and connecting to it by native

client.

The most challenging part while getting familiar with kernel module implementation was utilizing

memory-mapped I/O and examining board datasheets. Due to the fact that Linux used virtual memory system and 

datasheet used exclusively physical addresses, it was necessary to expose and explicitly allocate certain

physical memory regions (otherwise invisible) by means of Linux system functions.


































