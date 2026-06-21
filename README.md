# Smart Bottle Sorting and Reward Dispenser System

This Arduino-based project automates the process of detecting, sorting, and rewarding recycling efforts using sensors, servos, and an LCD display. The system identifies bottles made of metal or plastic and deposits them in the appropriate bin. After sorting three bottles, the system rewards the user by dispensing a coin.

## Features

- **Automatic Bottle Detection:** Uses an IR sensor to detect the presence of an object.
- **Material Classification:**
  - Inductive sensor for detecting metal objects.
  - Photoelectric sensor for identifying plastic materials.
- **Servo-Controlled Sorting:** Bottles are sorted into bins using a servo motor.
- **Coin Dispenser:** Dispenses a coin after every three sorted bottles.
- **Real-time Count Display:** An LCD screen displays current count of metal and plastic bottles.
- **Auditory Feedback:** Buzzer notifies user when detection and sorting are triggered.

## Hardware Requirements

- Arduino UNO (or compatible board)
- IR Sensor
- Inductive Proximity Sensor
- Photoelectric Sensor
- Buzzer
- 3x Servo Motors
  - Gate Servo
  - Sorter Servo
  - Coin Dispenser Servo
- I2C LCD Display (16x2, Address: `0x27`)
- Jumper wires and breadboard
- Power supply

## Wiring Overview

| Component              | Arduino Pin |
|------------------------|-------------|
| IR Sensor              | D2          |
| Inductive Sensor       | D3          |
| Photoelectric Sensor   | D4          |
| Buzzer                 | D8          |
| Gate Servo             | D6          |
| Sorting Servo          | D9          |
| Coin Dispenser Servo   | D10         |
| I2C LCD (SDA/SCL)      | A4/A5       |

> Make sure to power the servos using an external 5V power source if needed, especially if current draw becomes an issue.

## How It Works

1. The IR sensor detects an incoming object.
2. The gate closes to hold the object for sensing.
3. The inductive and photoelectric sensors determine the object's material.
4. Based on detection:
   - Metal → Sorted to metal bin.
   - Plastic → Sorted to plastic bin.
   - Unknown → Sent to general waste bin.
5. The LCD updates bottle counts in real time.
6. After every 3 valid bottles (metal/plastic), the coin dispenser releases a coin and the count resets.

## Installation

1. Clone this repository or copy the code to your Arduino IDE.
2. Install the necessary libraries:
   - `LiquidCrystal_I2C`  
   - `Servo`
3. Upload the code to your Arduino board.
4. Wire the hardware according to the wiring overview.
5. Power on the system.

## Customization

- Change the `if ((metalCount + plasticCount) >= 3)` condition to adjust the number of bottles needed to trigger a reward.
- Modify servo angles in `sorterServo.write()` and `coinDispenser.write()` if your hardware setup differs.

---

Created by Marc R. Melgar  
📧 marcmelgar419@gmail.com  
🌐 [marc-portfoliooo.netlify.app](https://marc-portfoliooo.netlify.app/)
