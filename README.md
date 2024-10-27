# Water Tank Monitor

> <img src="https://github.com/user-attachments/assets/8d433b8a-24cd-47a9-8d16-87e86abac079" width=250/>
> <img src="https://github.com/user-attachments/assets/b13222e3-4fc8-44c3-ad0d-d334df377e2d" width=250/>
> <img src="https://github.com/user-attachments/assets/83b4a405-47c6-4293-8210-22ff54bf38a5" width=250/>

This repo contains all the necessary assets to build a module monitoring and displaying the level of water in a water tank. 
The PCB layout was designed to fit in a [Legrand Mosaic obturateur](https://www.legrand.fr/pro/catalogue/obturateur-mosaic-2-modules-blanc), hence compatible with any Mosaic Support.

The project is hosted on [Hackaday.io](https://hackaday.io/project/199023-water-tank-monitor). See this [blogpost](https://hackaday.io/project/199023-water-tank-monitor/details) for more details about the designing and building process.

## Features
- get distance readings from an A02 ultrasonic sensor (UART)
- display water level percentage on a 0.91" SSD1306 OLED screen (I2C)
- onboard push button to turn on display (incl. off timer)
- report readings to a Zigbee [PTVO firmware](https://ptvo.info) (UART) for integration with home automation software

# PCB

The PCB was designed using [Kicad](https://www.kicad.org/).
Project files can be found in the [`/Schematics`](https://github.com/ncolomer/WaterTankMonitor/tree/main/Schematics) directory.

It includes [JLCPCB](https://jlcpcb.com/)'s [production files](https://raw.githubusercontent.com/ncolomer/WaterTankMonitor/refs/heads/main/Schematics/jlcpcb/production_files/GERBER-Water%20Tank%20Monitor.zip) if you want to make your own.

## Parts
- onboard
  - refer to the attached Kicad project BOM
  - special part: SMT Dual Entry low profile 1x04 header ([aliexpress](https://fr.aliexpress.com/item/1005006410708666.html))
- external
  - A02 ultrasonic sensor ([dfrobot](https://www.dfrobot.com/product-1935.html) [aliexpress](https://www.aliexpress.com/item/1005005543253128.html))
  - 0.91" SSD1306 OLED screen ([aliexpress](https://fr.aliexpress.com/item/1005006160204449.html))
  - JST XH connectors and cables
- programming
  - 2x03 (3P Double) 1.27" pogo pin clamp ([aliexpress](https://www.aliexpress.com/item/1005005832969596.html))
  - Atmel-ICE for bootloader upload via ICSP
    (an Arduino can be used too, [doc](https://support.arduino.cc/hc/en-us/articles/4841602539164-Burn-the-bootloader-on-UNO-Mega-and-classic-Nano-using-another-Arduino))
  - FT232RL module for serial programming via FTDI ([aliexpress](https://www.aliexpress.com/item/1005006861628994.html))
- (optional) zigbee connectivity
  - Zigbee module with PTVO firmware, eg. CC2530

Documentation (datasheets, footprints) can be found in the [`/Documentation`](https://github.com/ncolomer/WaterTankMonitor/tree/main/Documentation) directory.

## Programming

### ICSP (bootloader upload)
<img src="https://github.com/user-attachments/assets/2cbef0ca-41b4-4456-9cee-b5f38b30f94e" width="750"/>

### FTDI (serial programming)
<img src="https://github.com/user-attachments/assets/7c1197ed-e906-4475-9bdd-04f4403dbfe7" width="750"/>

# Firmware

The firmware was developed using [PlatformIO](https://platformio.org/).
Project files can be found in [`/Firmware`](https://github.com/ncolomer/WaterTankMonitor/tree/main/Firmware) directory.

## Host PTVO firmware characteristics

To report measured values via Zigbee, the board need a host PTVO firmware connected on pins PD3(RX) PD4(TX).
The following infos should help you generate your own custom PTVO firmware:
```
Board type: CC2530
Device type: Router
Model ID: water.monitor
Disable periodic On/Off reports for GPIO outputs: Yes
Status LED: P01, Sending reports
Set default reporting interval (s): 0

Output pins:
P02: Output 2, UART (UART: 115200 8N1, Packet end: 0x0D, Byte: 239)
     => PTVO UART pins are P02(RX) P03(TX)
P30: Output 3, UART sensor (Mode: On/Off, : Read/Write, Units: None), Remember state
     => oled display on/off state
P31: Output 4, UART sensor (Mode: Analog value, : Read, Units: None)
     => water level value in percentage
P32: Output 5, UART sensor (Mode: Analog value, : Read, Units: None)
     => measure distance to water in cm
P33: Output 6, UART sensor (Mode: Analog value, : Read/Write, Units: None), Remember state
     => configurable min measurable distance in cm
P34: Output 7, UART sensor (Mode: Analog value, : Read/Write, Units: None), Remember state
     => configurable max measurable distance in cm

Input pins:
P00: Input 1, External wake-up, Pull-up
     => onboard push button, if any
```
