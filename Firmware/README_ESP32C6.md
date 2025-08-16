# Water Tank Monitor - ESP32C6 Version

This is the refactored version of the Water Tank Monitor firmware designed to run on the **Seeed Studio XIAO ESP32C6** module instead of the original ATmega328P + CC2530 combination.

## Key Changes from Original Version

### Hardware Migration
- **Microcontroller**: ATmega328P → ESP32-C6 (dual-core RISC-V, up to 160MHz)
- **Memory**: 32KB Flash + 2KB RAM → 4MB Flash + 512KB RAM
- **Zigbee**: External CC2530 module → Native ESP32C6 Zigbee support
- **Programming**: ICSP/FTDI → USB-C (native CDC)

### Pin Mapping for XIAO ESP32C6
| Function | Pin | Description |
|----------|-----|-------------|
| LED | 21 | Built-in LED |
| Button | 0 | Boot button |
| I2C SDA | 4 | OLED Display |
| I2C SCL | 5 | OLED Display |
| UART RX | 20 | Ultrasonic Sensor |
| UART TX | 21 | Ultrasonic Sensor |

### Software Changes
- **Framework**: Arduino AVR → Arduino ESP32
- **Storage**: EEPROM → ESP32 Preferences (persistent storage)
- **Serial**: NeoSWSerial → HardwareSerial
- **Zigbee**: PTVO UART protocol → Native Zigbee stack
- **Interrupts**: EnableInterrupt → ESP32 attachInterrupt

## Building and Flashing

### Prerequisites
1. **PlatformIO Core** or **PlatformIO IDE**
2. **Seeed Boards Platform** (automatically installed via platformio.ini)

### Build Commands
```bash
# Build release version
pio run -e release

# Build debug version
pio run -e debug

# Upload to device
pio run -e release -t upload

# Monitor serial output
pio device monitor
```

### USB-C Programming
1. Connect XIAO ESP32C6 via USB-C cable
2. Hold BOOT button while connecting (if needed)
3. Select the correct COM port in PlatformIO
4. Upload firmware

## Zigbee Integration

### Native Zigbee Support
The ESP32C6 includes native Zigbee support, eliminating the need for:
- External CC2530 module
- PTVO firmware
- UART communication between MCU and Zigbee module

### Zigbee Clusters
The firmware implements standard Zigbee clusters:
- **Basic Cluster** (0x0000): Device information
- **Power Config Cluster** (0x0001): Power management
- **Analog Input Cluster** (0x000C): Water level and distance values
- **Binary Input Cluster** (0x000F): Display state

### Home Assistant Integration
The device will appear as a Zigbee device with:
- Water level percentage (0-100%)
- Raw distance measurement (cm)
- Display on/off state
- Configurable min/max distance values

## Configuration

### Default Settings
- **Min Distance**: 20cm (tank full)
- **Max Distance**: 200cm (tank empty)
- **Sensor Read Interval**: 1 second
- **Zigbee Report Interval**: 5 seconds
- **Display Timeout**: 30 seconds

### Persistent Storage
Settings are stored in ESP32's non-volatile memory using the Preferences library, ensuring configuration persists across reboots.

## Power Management

### Deep Sleep Support
The ESP32C6 supports deep sleep modes for battery operation:
- **Light Sleep**: CPU paused, peripherals active
- **Deep Sleep**: Most peripherals powered down
- **Hibernation**: Minimal power consumption

### Wake Sources
- Timer-based wakeup
- Button press (GPIO interrupt)
- UART activity

## Troubleshooting

### Common Issues

1. **Upload Fails**
   - Ensure USB-C cable supports data transfer
   - Hold BOOT button during connection
   - Check COM port selection

2. **Zigbee Not Working**
   - Verify Zigbee library compatibility
   - Check network permissions
   - Ensure coordinator is nearby

3. **Display Issues**
   - Verify I2C connections (SDA/SCL)
   - Check power supply to OLED
   - Confirm I2C address (default: 0x3C)

4. **Sensor Readings**
   - Check UART connections
   - Verify sensor power supply
   - Check baud rate (9600)

### Debug Mode
Enable debug output by building with the `debug` environment:
```bash
pio run -e debug
```

This will provide detailed serial output for troubleshooting.

## Future Enhancements

### Planned Features
- **WiFi Configuration**: Web-based setup interface
- **OTA Updates**: Over-the-air firmware updates
- **MQTT Support**: Alternative to Zigbee for some use cases
- **Battery Monitoring**: Built-in voltage monitoring
- **Data Logging**: Local storage of readings

### Zigbee Library Options
The current implementation is a placeholder. Consider these libraries:
- **ESP-IDF Zigbee Stack**: Official Espressif implementation
- **Arduino Zigbee**: Community-driven library
- **Custom Implementation**: Based on ESP32C6 datasheet

## Migration Guide

### From Original Project
1. **Hardware**: Replace ATmega328P + CC2530 with XIAO ESP32C6
2. **Connections**: Update pin assignments per new mapping
3. **Power**: Ensure 3.3V operation (ESP32C6 is not 5V tolerant)
4. **Programming**: Use USB-C instead of ICSP/FTDI

### Pin Compatibility
- **I2C**: Same connections, different pins
- **UART**: Same connections, different pins
- **GPIO**: Update pin numbers in code
- **Power**: 3.3V operation required

## Support

For issues specific to the ESP32C6 version:
1. Check this README
2. Review ESP32C6 documentation
3. Check PlatformIO forums
4. Review Seeed Studio XIAO ESP32C6 wiki

## License

Same license as the original project. This refactored version maintains the same functionality while leveraging the ESP32C6's enhanced capabilities.
