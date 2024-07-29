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
   - Upload the related sketch to your Arduino Uno R3.

2. **Open Serial Monitor**:
   - Open the Serial Monitor in the Arduino IDE.
   - Set the baud rate to `9600`.

3. **Follow Instruction**:
   - Each file may have specific instructions. Please follow them.

## Credits

- Developed by: Mehmet Tekin
- GitHub: Tekinton

## Device Manufacturer

- Manufacturer: Atlas Scientific
- Website: [https://www.atlas-scientific.com/](https://www.atlas-scientific.com/)

## License

This project is licensed under the MIT License. See the LICENSE file for details.
