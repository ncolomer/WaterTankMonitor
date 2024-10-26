Tags: platformio, ptvo, atmega328p, arduino, pcb, diy

# 0. Intro

This is the reporting of a month-long journey from a given idea to a physical running object, involving PCB design, firmware conception, 3D modeling and printing.
In those lines I try to describes the various steps I gone through, blockers I faced, the errors I made and how I overcame them.
I hope this it help any future tinkerer that would like to build something similar and is looking for inspiration.

# 1. The initial idea

The initial idea was to measure the water level available in a 5m3 water tank and make the corresponding fill percentage visually available somewhere in my garage.
In addition, not only these measures would happen regularly but would go through my Zigbee network and be archived in my Home Assistant instance.

Here are the constraints I defined for myself before starting:

- I wanted the PCB to fit in a Legrand obturateur
  - this meant not much surface budget = compact design
  - with SMT 0603 and QFP parts, those are harder to solder (but still possible) but tiny footprint
  - JST XH connectors, still 2.5mm pitch but smaller courtyard
- As I was expecting a lot of program-test-and-retry, I wanted the ability to re-program easily and quickly
  - This meant have both ICSP (bootloader & fuses) and FTDI (serial debug and and comm) ports available on board
- I wanted proven and widely used microcontroller with I2C, UART, Software Serial, ADC, interrupts
  - ATMega328p, with the benefit it is compatible with Arduino framework
- no analog coupling, I want to use serial protocols only to communicate with sensors and motherboard
  - more precisely, motherboard will be a PTVO-powered PCB communicating via UART protocol

# 2. Board design

The 2 following parts are the most critical. You don't want to produce many versions of the same board, as
- the whole process is strict and many little things (datasheet detail, wiring, etc) have to be overseen (experience makes it easier over time, I guess)
- not like software, you can't really "iterate fast" as producing and delivering PCBs takes time

I chose to use Kicad for the design. I had some experience with Autocad Eagle, but I wanted to use this Open Source and very mature project.

## 2.1. Schematic design

What I want to build is quite basic when we think about it. It's just the uc, its minimal companion electronics, connectors for external communication, and programming pads. The ATMega328p will be the central piece with its many ports and features.

The main components I want the uc to communicate with are:
- a 0.91" OLED display uing SSD3206 I2C-compatible driver
- an A02YYUW ultrasonic range sensor, UART-compatible
- 2 manual calibration and display brightness potentiometers, spoiler: I'd rather choose to implement a push button instead, as these setting values can be set wireless via zigbee
- an onboard status led
- UART connection with upstream motherboard (PTVO firmware)

For the uc and companions parts, I mostly inspired myself from many open source schematics, including:
- Arduino Nano https://www.arduino.cc/en/uploads/Main/Arduino_Nano-Rev3.2-SCH.pdf
- Moteino https://lowpowerlab.com/guide/moteino/design-files/

The rest is reading datasheets and binding components to the right uc ports.

## 2.2. PCB design

This is the part I spent the most of my time in this project!

It is a really demanding phase, requiring experience and knowledge. It involves parts placement, traces labelling and optimization, taking into account physical constraints for both you aim to build and fabrication. Hopefully, fabs generally give their limits specs, that can be translated into DRC rules (eg. [PCBway](https://www.pcbway.com/pcb_prototype/PCB_Design_Rule_Check.html)).

One difficulty was to find a proper header connector to connect the 4 pins of the oled board considering it will sit in the back of the board.
Yet I could use standard female headers, but they are so tall and might not fit in the space between board and obturateur. I didn't know the exact height, and I wanted the ability to adjust height as required in the final assembly.
I started looking in [Octopart](https://octopart.com) that aggregates catalog and stocks from famous suppliers like Digikey, RS or Mouser. It is a good way to find still "active" parts (basically with stocks high) and ensure you make the right choice.
After looking in many places () is great place to start!), I found several "Surface Mount,Single-Row, Dual Entry, End to End Stackable, low profile"
Eg. the Tyco Electronics MPN 5-147736-3. But they were not as "low" profile as I wanted.
Finally found what I needed on aliexpress https://www.aliexpress.com/item/1005006410708666.html. You always make a bet when ordering exotic parts from aliexpress, hopefully this one was documented with its dimensions, and I could confirm and design proper footprint on Kicad.

At this stage you seems you've reach a final version for your board, it's always good to gather feedbacks. There are many communities out there but I finally asker [r/PrintedCircuitBoard](https://www.reddit.com/r/PrintedCircuitBoard/) for reviews, and got valuable ones. My takeaways from the exchange:
- Orient parts to minimize trace length
- No isolated ground pour island (you’ll be warned by kicad when running drc)
- Keep power rail as simple / straight as possible
- Differentiate power and signal track width

## 2.3. PCB Fab & assembly

I never used PDBA (A stands for Assembly) and wanted to give it a try.
It offers the advantage you don't have to source all the parts and make stock of them.

JLCPCB seemed cheap with good feedbacks from hobbyists.
You can peak parts from their [LCSC](https://www.lcsc.com/) "partner" large catalog and stock.
The process is fully automated with quick reply from support while ordering.
One thing to note is that economic PCBA (the cheapest option) only offers to populate one side of the board.
But I had 2 SMD components (the potentiometers) on the back, so I left them unpopulated to keep price low (otherwise raising price up to $60).
I ordered them separately and soldered by myself.
I also marked the socket pin connector unpopulated too because I didn't find any equivalent in their catalog (see previous section)

Few things to note:
- expect to pay taxes when shipping in CEE
- there is a great Kicad plugin to generate all fabrication files required https://github.com/Bouni/kicad-jlcpcb-tools

The 5 PCBs arrived 2 weeks after the order placement, 2 of them populated. Most of this time was shippment as the boards were printed a populated only a couple of days after ordering. The result was very clean and ready to start using.

# 3. Programming

Now come the rewarding part, when your thing starts to be alive!

## 3.1. Preliminary checks
I first wanted to check the soldering uc was correctly responding
I used an atmel-ice programmer via isp to be able to upload bootloader.
I connected it using this 1.27mm pitch pogo pins found on aliexpress (a bit tricky, 1.27 is super tiny, but that's ok)

Doubled checked pinout and connections from atmel-ice to pcb trace is ok

Then run avrdude command `avrdude -c atmelice_isp -p m328p -v` but got following error

```
avrdude stk500v2_command() error: command failed
avrdude: bad response to AVR sign-on command: 0xa0
avrdude stk500v2_program_enable() warning: target prepared for ISP, signed off
        now retrying without power-cycling the target
... 4 retries and finally:
avrdude stk500v2_command() error: command failed
avrdude: bad response to AVR sign-on command: 0xa0
avrdude stk500v2_program_enable() error: unable to return from debugWIRE to ISP
avrdude main() error: initialization failed, rc=-1
        - double check the connections and try again
        - use -B to set lower the bit clock frequency, e.g. -B 125kHz
        - use -F to override this check
```

I finally found and confirmed the atmel-ice do not power device being programed

I attached a 3.7V 1800mAh 903052 Lipo battery to VCC and GND pins, but still no success

I figured out programmer still sees 0V as target voltage (read value is present in the avrdude output).

I rechecked programmer cable pinout from datasheet plus the connections and indeed found I made mistakes: the view in data sheet is from back/behind, not front!
I reconnected cables properly + battery (leaving target unconnecetd to avoid damage) and then programmer successfully read `Vtarget : 3.72 V`. Once target in reconnected, device signature is read and I got the expected output:
```
$ avrdude -c atmelice_isp -p m328p -v

avrdude: Version 7.3
         Copyright the AVRDUDE authors;
         see https://github.com/avrdudes/avrdude/blob/main/AUTHORS

         System wide configuration file is /usr/local/etc/avrdude.conf
         User configuration file is ~/.avrduderc
         User configuration file does not exist or is not a regular file, skipping

         Using port            : usb
         Using programmer      : atmelice_isp
         AVR Part              : ATmega328P
         Programming modes     : ISP, HVPP, debugWIRE, SPM
         Programmer Type       : JTAG3_ISP
         Description           : Atmel-ICE (ARM/AVR) in ISP mode
         ICE HW version        : 0
         ICE FW version        : 1.39 (rel. 130)
         Serial number         : J41800061425
         Vtarget               : 0.0 V
         SCK period            : 125.0 us
         Vtarget               : 3.72 V

avrdude: AVR device initialized and ready to accept instructions
avrdude: device signature = 0x1e950f (probably m328p)
```
- I could also read the fuses value, which are the [default one](https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p&LOW=62&HIGH=D9&EXTENDED=FF&LOCKBIT=FF) (factory chip not yet touched):
```
$ avrdude -c atmelice_isp -p atmega328p -b 115200 -qq -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h -U lock:r:-:h -U flash:r:-:i
0x62
0xd9
0xff
0xff
:20000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
:20002000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE0
:20004000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC0
...

```

## 3.2. Uploading bootloader

Now we need to adjust fuses and upload a bootloader to enable FTDI serial programing (as for Arduino).

I'll use [optiboot](https://github.com/MCUdude/optiboot_flash.git).
But need to tune the bootloader because I have a custom status led on PB1 (PB5 is the default):
```
$ git clone https://github.com/MCUdude/optiboot_flash.git
$ cd optiboot_flash
$ make atmega328p AVR_FREQ=16000000L BAUD_RATE=115200 LED=B1 LED_START_FLASHES=2 UART=0
...
Output file name: bootloaders/atmega328p/16000000L/optiboot_flash_atmega328p_UART0_115200_16000000L_B1.hex
```

I use platformio to upload the bootloader and set fuses (cleaned output for better readability):
```
$ pio run --verbose --target bootloader --environment bootloader
...
avrdude -p atmega328p -C ~/.platformio/packages/tool-avrdude/avrdude.conf -e -c atmelice_isp -Ulock:w:0x3F:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m -Uefuse:w:0xFD:m
avrdude: processing -U lock:w:0x3F:m
avrdude: processing -U hfuse:w:0xDE:m
avrdude: processing -U lfuse:w:0xFF:m
avrdude: processing -U efuse:w:0xFD:m
...
avrdude -p atmega328p -C ~/.platformio/packages/tool-avrdude/avrdude.conf -c atmelice_isp -Uflash:w:bootloader/optiboot_flash_atmega328p_UART0_115200_16000000L_B1.hex:i -Ulock:w:0x0F:m
avrdude: erasing chip
...
avrdude: processing -U flash:w:bootloader/optiboot_flash_atmega328p_UART0_115200_16000000L_B1.hex:i
avrdude: writing 484 bytes flash ...
...
avrdude: processing -U lock:w:0x0F:m
```

The board finally comes alive and the led blink twice every second or so: the bootloader starts, wait for serial programming signal for a second, and fallback to the program, but nothing there so it reboots again starting with bootloader sequence.

### 3.3 Repeated programming

Now the bootloader is present, I switched to FTDI programmer (FT232RL) to load my first "blink" program
  - pinout and connection https://components101.com/sites/default/files/component_pin/FT232RL-USB-TO-TTL-Converter-Pinout.jpg (courtesy of [components101.com](https://components101.com/modules/ft232rl-usb-to-ttl-converter-pinout-features-datasheet-working-application-alternative))
  - leave CTS in unconnected (nRESET is connected to DTR)
  - take care to reverse RX/TX lines, it's not a straight (RX on programmer is connected to TX on uc, and vice versa)
  - voltage jumper switched to 3.3V because this is the voltage of my board (VCC reads 3.5V actually but that's within uc range)

- the programmer device reads the following characteristics from macOS:
```
FT232R USB UART :
  Identifiant du produit :	0x6001
  Identifiant du fournisseur :	0x0403  (Future Technology Devices International Limited)
  Version :	6.00
  Numéro de série :	A50285BI
  Vitesse :	Jusqu’à 12 Mb/s
  Fabricant :	FTDI
  Identifiant de l’emplacement :	0x14320000 / 9
  Courant disponible (mA) :	500
  Courant requis (mA) :	90
  Exploitation supplémentaire actuelle (mA) :	0
```

- the first upload using platformio is ok
```
$ pio run --target upload
Auto-detected: /dev/cu.usbserial-A50285BI
avrdude -v -p atmega328p -C /Users/nicolas/.platformio/packages/tool-avrdude/avrdude.conf -c arduino -b 115200 -D -P /dev/cu.usbserial-A50285BI -U flash:w:.pio/build/serial/firmware.hex:i
avrdude: AVR device initialized and ready to accept instructions
avrdude: device signature = 0x1e950f (probably m328p)
avrdude: writing 880 bytes flash ...
```

but when I retried, I got an error: the uc was not responding and the programmer timed out
```
avrdude: stk500_recv(): programmer is not responding
avrdude: stk500_getsync() attempt 1 of 10: not in sync: resp=0x00
...
avrdude: stk500_recv(): programmer is not responding.gitignore
avrdude: stk500_getsync() attempt 10 of 10: not in sync: resp=0x00

Failed uploading: uploading error: exit status 1
```

- I got the lead I need [here](https://forum.arduino.cc/t/cant-upload-more-than-once-on-atmega328-chips/683247): « The "can only upload once" symptom usually means that the auto-reset circuit is missing or faulty. »
- this made sense: with a fresh bootloader, uc is mostly spending time waiting for a new program from serial, but once we loaded a program, the bootloader run once and then we're in program loop. a trigger on reset is needed to reboot us and load a program again. but it seems my reset line was not working...
- it was indeed a wiring problem: I added an extra cap between reset line and ground that was missing reseting the chip properly (couldn't be set to low, or not fast enough for the programmer)
I removed the cap and it was finally working as expected, I could reprogram as many time as desired.

## 3.3. The program

You can find the program sources on Github here:
but I describe the most relevant parts in the following sections.

### 3.3.2 Comm with A02 ultrasonic distance sensor

The device is a DYP-A02YYUW-V2.0 (reference A0221AU), Bistatic waterproof probe, waterproof case, UART Auto Output ([datasheet](https://www.dypcn.com/uploads/A02-Datasheet.pdf) + [Output interface](https://www.dypcn.com/uploads/A02-Output-Interfaces.pdf)). Meaning it will constantly stream readings (vs. controlled outputn triggering a read when RX is pulled low).

Connection is easy using the [SoftwareSerial](https://docs.arduino.cc/learn/built-in-libraries/software-serial/) built-in library.

- sensor outputs lot of garbage frame
  - seen as corrupted or wrong checksum
  - I ended by accepting it and just ignore those faulty readings

- had an issue with unstable readings (volatility)
> (2) Communication instruction
> When the pin(RX)is suspended or the input high level, the module outputs processed value, and the data is more stable. The response time of is 100~500ms. When the pin(RX)is input low level, the module outputs real-time value. Response time of is 100ms.
  - so I forced TX_PIN pin PD4 (connected to sensor RX) to low

I inspired from the https://www.dfrobot.com/product-1935.html sample code, but also [this](https://disqus.com/by/disqus_nVDhbJIGmc/) comment.

I chose to implement a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) to simplify the code when reading stream of data comming from the sensor.
It was an apportunity to test platformio's [unit testing](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html) framework.

## 3.3.3 Comm wih SSD1304 oled display

Not much to say here, I used [adafruit/Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) library and everything worked like a charm!

See corresponding code here.

### 3.3.4 Comm with PTVO firmware




# 4. Board potentiometer repurpose
- I want the water tank level, but don't want the display to be always on
- there is no physical trigger include in my initial board design, so I need to implement one
- I finally resigned to use the two potentiometers to control brightness (it doesn't make [much difference](https://github.com/adafruit/Adafruit_SSD1306/blob/14a4563e100c6083e7d51cfc41a9652c6edc3a70/Adafruit_SSD1306.cpp#L1179-L1180)) and calibration (it will be done via a remote zigbee setting)
- I want to solder a push button with 4-leg footprint
- reused the potentiometers footprint, the 2 uc pins 23 calibration and 24 brightness (respectively Arduino A0/14 and A0/15) + brightness' ground pin that is barely aligned with pin 24
- Those pins are Pin Change interrupt enabled, but need to use ISR1 vector, hence incomptible with SoftwareSerial
- finally relie on https://github.com/SlashDevin/NeoSWSerial + https://github.com/GreyGnome/EnableInterrupt

# 5. Retrospective
- as already mentioned, I figure out later the potentiometers was useless because I could send values using zigbee
- add a push button on board to trigger turning display on with an off time (I managed to adapt one on the potentiometers footprints hopefully)
  - long click = debug screen, displaying connectivity with A02 sensor and motherboard (last message sent/received)
  - double click = force sending all values to motherboard, for debugging purposes
- communicate with ptvo firmware using another (software serial) channel, to leave UART for FTDI programming and debug only. Software Serial constraints (baudrate) should be considered though
- holes to screw the board!



Inspiring resources:
- https://blog.kylemanna.com/hardware/sniffer-air-quality-monitor-aqi-using-esp32-pmsa003-bme680/
- https://lcamtuf.substack.com/p/comparing-hobby-pcb-vendors



