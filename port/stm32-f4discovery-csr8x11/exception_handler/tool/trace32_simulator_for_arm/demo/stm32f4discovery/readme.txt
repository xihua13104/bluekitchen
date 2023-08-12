
The STM32F407G-DISC1 (formerly STM32F4DISCOVERY) board does not present a standard debug header.
To debug the board using TRACE32 you need to perform the following modifications:


Disconnect ST-LINK from STM32F407:

  - Remove both jumpers on CN3
  - Open solder bridges SB11 and SB12


Connect TRACE32 to STM32F407:


  - ARM-20 connector (Debug Cable):

    +---------------------------------------------------+
    | ARM-20        | Name   | STM32F407G-DISC1 | Name  |
    +---------------------------------------------------+
    | Pin 1         | VREF   | P1[3]            | VDD   |
    | Pin 4         | GND    | P1[1]            | GND   |
    | Pin 7         | SWDIO  | P2[42]           | PA13  |
    | Pin 9         | SWCLK  | P2[39]           | PA14  |
    |(Pin 13)       |(SWO)   |(P2[28])          |(PB3)  |
    | Pin 15        | RESET- | P1[6]            | NRST  |
    +---------------------------------------------------+
    (SWO is not supported with Debug Cable)


   - MIPI-10 connector (CombiProbe, µTrace):

    +---------------------------------------------------+
    | MIPI-10       | Name   | STM32F407G-DISC1 | Name  |
    +---------------------------------------------------+
    | Pin 1         | VREF   | P1[3]            | VDD   |
    | Pin 3         | GND    | P1[1]            | GND   |
    | Pin 2         | SWDIO  | P2[42]           | PA13  |
    | Pin 4         | SWCLK  | P2[39]           | PA14  |
    | Pin 6         | SWO    | P2[28]           | PB3   |
    | Pin 10        | RESET- | P1[6]            | NRST  |
    +---------------------------------------------------+


   - MIPI-20 connector (CombiProbe, µTrace):

    +-------------------------------------------------------+
    | MIPI-20       | Name       | STM32F407G-DISC1 | Name  |
    +-------------------------------------------------------+
    | Pin 1         | VREF-DEBUG | P1[3]            | VDD   |
    | Pin 3         | GND        | P1[1]            | GND   |
    | Pin 2         | SWDIO      | P2[42]           | PA13  |
    | Pin 4         | SWCLK      | P2[39]           | PA14  |
    | Pin 6         | SWO        | P2[28]           | PB3   |
    | Pin 10        | RESET-     | P1[6]            | NRST  |
    | Pin 12        | TRACECLK   | P2[15]           | PE2   |
    | Pin 14        | TRACEDATA0 | P2[16]           | PE3   |
    | Pin 16        | TRACEDATA1 | P2[13]           | PE4   |
    | Pin 18        | TRACEDATA2 | P2[14]           | PE5   |
    | Pin 20        | TRACEDATA3 | P2[11]           | PE6   |
    +-------------------------------------------------------+


  - MIPI-34 connector (CombiProbe, µTrace):

    +-------------------------------------------------------+
    | MIPI-34       | Name       | STM32F407G-DISC1 | Name  |
    +-------------------------------------------------------+
    | Pin 1         | VREF-DEBUG | P1[3]            | VDD   |
    | Pin 3         | GND        | P1[1]            | GND   |
    | Pin 2         | SWDIO      | P2[42]           | PA13  |
    | Pin 4         | SWCLK      | P2[39]           | PA14  |
    | Pin 6         | SWO        | P2[28]           | PB3   |
    | Pin 10        | RESET-     | P1[6]            | NRST  |
    | Pin 22        | TRACECLK   | P2[15]           | PE2   |
    | Pin 24        | TRACEDATA0 | P2[16]           | PE3   |
    | Pin 26        | TRACEDATA1 | P2[13]           | PE4   |
    | Pin 28        | TRACEDATA2 | P2[14]           | PE5   |
    | Pin 30        | TRACEDATA3 | P2[11]           | PE6   |
    | Pin 34        | VREF-TRACE | P1[3]            | VDD   |
    +-------------------------------------------------------+


  - Mictor-38 connector (Preprocessor AutoFocus II):

    +-------------------------------------------------------+
    | Mictor-38     | Name       | STM32F407G-DISC1 | Name  |
    +-------------------------------------------------------+
    | Pin 12        | VREF-TRACE | P1[3]            | VDD   |
    | Pin 5,30,32   | GND        | P1[1]            | GND   |
    | Pin 6         | TRACECLK   | P2[15]           | PE2   |
    | Pin 38        | TRACEDATA0 | P2[16]           | PE3   |
    | Pin 28        | TRACEDATA1 | P2[13]           | PE4   |
    | Pin 26        | TRACEDATA2 | P2[14]           | PE5   |
    | Pin 24        | TRACEDATA3 | P2[11]           | PE6   |
    +-------------------------------------------------------+
