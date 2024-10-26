# Water Tank Monitor

This repo contains all the necessary assets to build a module monitoring and displaying the level of water in a water tank. the PCB layout was designed to fit in a [Legrand Mosaic obturateur](https://www.legrand.fr/pro/catalogue/obturateur-mosaic-2-modules-blanc), hence compatible with any Mosaic Support.

The project is hosted on [Hackaday.io](https://hackaday.io/project/199023-water-tank-monitor). See this [blogpost](https://hackaday.io/project/199023-water-tank-monitor/details) for more details about the designing and building process.

<img src="https://github.com/user-attachments/assets/8d433b8a-24cd-47a9-8d16-87e86abac079" width=250/>

## Features
- get readings from an [A02 ultrasonic sensor](https://www.dfrobot.com/product-1935.html) (UART)
- display level percentage on a [0.91" SSD1306 OLED screen](https://fr.aliexpress.com/item/1005006160204449.html) (I2C)
- onboard push button to turn on display (incl. off timer)
- report readings to a Zigbee [PTVO firmware](https://ptvo.info) (UART) for integration with any home automation software

# PCB

The PCB was designed using [Kicad](https://www.kicad.org/).
Project files can be found in the [`/Schematics`](https://github.com/ncolomer/WaterTankMonitor/tree/main/Schematics) directory.

It includes [JLCPCB](https://jlcpcb.com/)'s [production files](https://raw.githubusercontent.com/ncolomer/WaterTankMonitor/refs/heads/main/Schematics/jlcpcb/production_files/GERBER-Water%20Tank%20Monitor.zip) if you want to make your own.

# Firmware

The firmware was developed using [PlatformIO](https://platformio.org/).
Project files can be found in [`/Firmware`](https://github.com/ncolomer/WaterTankMonitor/tree/main/Firmware) directory.
