# Wordclock

A word clock is a clock where you actually read the hour.
There are plenty of wordclocks out there, some of them have had successful
campaigns in sites like Kickstarter or Indiegogo. This project is not meant to
be innovative or revolutionary, it's more like, again, an I-can-do-that-too project,
a pretty big subset of the DIY movement.

The goal of the project is to have a wordclock that would tell the hour in
catalan and spanish. Whilst spanish hour system is pretty simple, catalan one is
not and, AFAIK, there are no wordclocks in catalan out there.

## Hardware

### The clock

The hardware has been designed using [OpenSCad][1] and it's inspired by the [PiBow][2]
cases for Raspberry Pi. A series of layers forming a sandwich. From top to bottom
the layers are (this is true for beta version):

* frontend, a smoke colored acrylic layer cut with a laser cutter
* stencil, a thin black cardboard with the characters cut out with a laser
* diffusor, a thin white paper sheet
* lattice, a 3D printed black PLA grid to isolate each pixel
* LED matrix, a 16x16 flexible RGB LED Matrix w/ WS2812B pixels
* support, a clear acrylic layer
* controller, custom PCB attached to the backend
* backend, a clear acrylic layer with holes for wall mounting

### The controller

The controller is a custom PCB designed to serve different purposes. On one hand
there is an RTC and a uC (ATMega328P, same as an Arduino UNO). The user interface
are 4 big 12x12mm buttons and, optionally there is an SD card reader (more about
this in a future project).

It can be powered with 5V via a 2.1x5.5mm barrel plug or a screw terminal, there is
an FTDI-compatible header to program it and a 3-wire screw terminal to connect
Neopixel-like strips.

You can check the schematics and board layout for this controller in the repository
of another of my projects, the [Fibonacci Clock][3].

## Firmware

The project is ready to be build using [PlatformIO][4].
Please refer to their web page for instructions on how to install the builder.
Once installed:

```bash
> cd code
> platformio init -b uno
> platformio run
> platformio run --target upload
```

Library dependencies are automatically managed via PlatformIO Library Manager.

[1]: http://www.openscad.org
[2]: https://www.raspberrypi.org/blog/pibow/
[3]: https://bitbucket.org/xoseperez/fibonacci_clock
[4]: http://www.platformio.org
