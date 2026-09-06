
## Trigger Pins
| Module Pin | Arduino Uno / Nano Pin | Notes |
| --- | --- | --- |
| **CONN STATUS** | Pin `7` | LED if connected to the second LORA Module |
| **BTN** | Pin `5` | Fire Button Pin |
| **TRIG** | Pin `4` | Fire Trigger Pin |

## Wiring Guide - DX-PJ27-V1.2 LoRa Module

### Uno/Nano
| Module Pin | Arduino Uno / Nano Pin | Notes |
| --- | --- | --- |
| **VCC** | `3.3V` or `5V` | Check your exact baseboard supply voltage specification |
| **GND** | `GND` | Ground connection |
| **MOSI** | Pin `11` | SPI Master Out Slave In |
| **MISO** | Pin `12` | SPI Master In Slave Out |
| **SCK** | Pin `13` | SPI Serial Clock |
| **NSS / CS** | Pin `10` | SPI Chip Select |
| **RST** | Pin `9` | Hardware Reset |
| **DIO1** | Pin `2` | Digital I/O 1 (Interrupt pin) |
| **BUSY** | Pin `3` | Busy Status indicator pin |

### Arduino Mega 2560
| Module Pin | Arduino Mega 2560 Pin | Notes |
| :--- | :--- | :--- |
| **VCC** | `3.3V` or `5V` | Check your exact baseboard supply voltage specification |
| **GND** | `GND` | Ground connection |
| **MOSI** | Pin `51` | Hardware SPI MOSI |
| **MISO** | Pin `50` | Hardware SPI MISO |
| **SCK** | Pin `52` | Hardware SPI SCK |
| **NSS / CS**| Pin `10` | SPI Chip Select (Can be any digital pin, 10 is standard) |
| **RST** | Pin `9` | Hardware Reset |
| **DIO1** | Pin `2` | Digital I/O 1 (Hardware Interrupt pin) |
| **BUSY** | Pin `3` | Busy Status indicator pin |


### Teensy 4.1
| Module Pin | Teensy 4.1 Pin | Notes |
| :--- | :--- | :--- |
| **VCC** | `3V3` or `VIN` | Check your baseboard; Teensy 4.1 digital pins are NOT 5V tolerant! |
| **GND** | `GND` | Ground connection |
| **MOSI** | Pin `11` | Hardware SPI0 MOSI |
| **MISO** | Pin `12` | Hardware SPI0 MISO |
| **SCK** | Pin `13` | Hardware SPI0 SCK |
| **NSS / CS**| Pin `10` | SPI Chip Select (Can be any digital pin, 10 is default) |
| **RST** | Pin `9` | Hardware Reset (Can be any digital pin) |
| **DIO1** | Pin `2` | Digital I/O 1 (Hardware Interrupt pin) |
| **BUSY** | Pin `3` | Busy Status indicator pin |


### Uploader and linux udev rules:
[Using The Teensy Loader on Ubuntu Linux](https://www.pjrc.com/teensy/loader_linux.html)