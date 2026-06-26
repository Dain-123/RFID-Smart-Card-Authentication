# Firmware-Driven RFID Access Control System

An advanced, enterprise-grade embedded security simulation featuring dynamic authentication and real-time state management. Built and verified using the Wokwi development platform.

## 🚀 Advanced Features (Firmware Architecture)

- **Finite State Machine (FSM):** Implemented a dual-state control loop (`NORMAL_MODE` vs `PROGRAMMING_MODE`) allowing deterministic runtime execution paths based on administrative events.
- **Non-Volatile Register Allocation:** Integrated the standard `EEPROM.h` library to read/write 4-byte validation matrices dynamically, ensuring critical credential persistence across power-loss states.
- **Synchronized Audio Signaling:** Utilized software-timed Pulse Width Modulation (PWM) on a piezo buzzer to handle multi-tone feedback for error states and administrative access shifts.

---

## 🗺️ System Pin-Mapping Architecture

| Peripheral Component | Component Pin | Arduino Uno Pin | Protocol / Signal Type |
| :--- | :--- | :--- | :--- |
| **MFRC522 RFID Reader** | VCC | 3.3V | Power (3.3V Only) |
| | RST | D9 | Reset Control Line |
| | GND | GND | Ground Reference |
| | MISO | D12 | SPI Master In Slave Out |
| | MOSI | D11 | SPI Master Out Slave In |
| | SCK | D13 | SPI Serial Clock |
| | SDA (SS) | D10 | SPI Slave Select Line |
| **I2C LCD Display (16x2)**| GND | GND | Ground Reference |
| | VCC | 5V | Power |
| | SDA | A4 | I2C Serial Data Line |
| | SCL | A5 | I2C Serial Clock Line |
| **Piezo Buzzer Alarm** | Positive (+) | D5 | Digital PWM Output |
| | Negative (-) | GND | Ground Reference |

---

## 🛠️ Hardware Interface Protocols

- **SPI Bus Protocol Execution:** The MFRC522 RFID transceiver is interfaced via standard hardware SPI lines (`D11`, `D12`, `D13`). A dedicated Slave Select (`SS`) line is established on `D10` to allow clean data packet clocking without bus contention.
- **I2C Shared Bus Utilization:** The 16x2 alphanumeric LCD uses a standard 2-wire serial protocol over pins `A4 (SDA)` and `A5 (SCL)` at device hex address `0x27`, drastically reducing overall GPIO pin allocation from 6 digital lines down to 2.
