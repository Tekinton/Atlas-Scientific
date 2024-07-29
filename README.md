# Atlas Scientific EZO Devices

This repository includes automated and interactive codes for Atlas Scientific EZO devices. It aims to simplify the usage of these products for users by providing easy-to-follow examples and comprehensive instructions.

## Description

This Arduino sketch automates the calibration process for various Atlas Scientific sensors. The process includes:

1. **Automatic Detection of Previous Calibration**: The sketch checks for existing calibration data and either displays the current calibration slope or clears previous calibration data.
2. **Multi-Point Calibration**: The user is prompted to place the sensor in different calibration solutions. The sketch waits for stable readings before executing calibration commands.
3. **User-Friendly Interface**: The calibration process can be easily controlled through the Arduino IDE's Serial Monitor using `START` and `STOP` commands.

## Key Features

- **Automatic Calibration**: Automates steps required to calibrate Atlas Scientific sensors.
- **Stability Detection**: Ensures accurate calibration by waiting for stable readings.
- **Serial Monitor Control**: Allows users to start and stop the calibration process using simple serial commands.
- **Comprehensive Instructions**: Provides clear prompts and instructions via the Serial Monitor.

## How to Use

1. **Upload the Code**:
   - Upload the `auto_ph_calibration_i2c_r3.ino` sketch to your Arduino Uno R3.

2. **Open Serial Monitor**:
   - Open the Serial Monitor in the Arduino IDE.
   - Set the baud rate to `9600`.

3. **Start Calibration**:
   - Type `START` in the Serial Monitor and press `Send`.
   - The program will check for any previous calibration:
     - If there is a previous calibration, it will display the current calibration slope.
     - If there is no previous calibration, it will clear the previous calibration.
   - Follow the prompts to place the probe in the appropriate calibration solutions (pH 7.00, 4.00, and 10.00).

4. **Calibration Process**:
   - The program will automatically calibrate the sensor by waiting for stable readings (4 consecutive readings) and then sending the calibration commands.

5. **Stop Calibration**:
   - Type `STOP` in the Serial Monitor and press `Send` to exit the calibration process at any time and clear the calibration.

## Commands

- `START`: Begin the calibration process.
- `STOP`: Stop the calibration process and clear the calibration.

## Repository Contents

- `Calibration_Sketch.ino`: The main Arduino sketch file for the calibration process.
- `README.md`: Detailed instructions and information about the project.
- `LICENSE`: License information for the project.

## Credits

- Developed by: Mehmet Tekin
- GitHub: Tekinton

## Device Manufacturer

- Manufacturer: Atlas Scientific
- Website: [https://www.atlas-scientific.com/](https://www.atlas-scientific.com/)

## License

This project is licensed under the MIT License. See the LICENSE file for details.
