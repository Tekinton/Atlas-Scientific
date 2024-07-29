/*
Automatic Three-Point Calibration Process for Atlas Scientific EZO pH Circuit

Usage Instructions:
1. Upload the code to your Arduino Uno R3.
2. Open the Serial Monitor in the Arduino IDE and set the baud rate to 9600.
3. The program will display instructions on how to start and stop the calibration process.
4. Type 'START' to initiate the calibration process. The program will:
   a. Check for a previous calibration using the "Cal,?" command.
   b. If there is a previous calibration (return value 1), display the current calibration slope using the "Slope,?" command and store it.
   c. If there is no previous calibration (return value 0), clear the previous calibration using the "Cal,clear" command before starting.
5. Follow the prompts to place the probe in the appropriate calibration solutions (pH 7.00, 4.00, and 10.00).
6. The program will automatically calibrate the sensor by waiting for stable readings (4 consecutive readings) and then sending the calibration commands.
7. After completing the calibration, the program will display the initial and final calibration slopes if the device was previously calibrated.
8. Type 'STOP' to exit the calibration process at any time and clear the calibration.

Commands:
- 'START': Begin the calibration process.
- 'STOP': Stop the calibration process and clear the calibration.

Credits:
Developed by Mehmet Tekin
GitHub: Tekinton

Device Manufacturer: Atlas Scientific
Website: https://www.atlas-scientific.com/
*/

#include <Wire.h>             // include Arduino's I2C library
#include <Ezo_i2c.h>          // include the EZO I2C library
#include <Ezo_i2c_util.h>     // include common print statements

Ezo_board PH = Ezo_board(99, "PH"); // create a PH circuit object with address 99 and name "PH"

// Buffer to store the response from the sensor
char sensor_data[32];
bool calibrating = false;         // flag to control calibration process
bool was_previously_calibrated = false;
char initial_slope[32];

void setup() {
  Wire.begin();       // start the I2C
  Serial.begin(9600); // start the serial communication to the computer
  while (!Serial);    // wait for serial port to connect. Needed for native USB

  // Inform the user about the calibration process
  print_instructions();

  // Debug: Confirm I2C connection
  PH.send_cmd("i"); // send info command to check if the sensor responds
  delay(300);       // wait for the sensor to process the command
  if (PH.receive_cmd(sensor_data, sizeof(sensor_data)) == Ezo_board::SUCCESS) {
    Serial.println("Successfully connected to pH sensor.");
  } else {
    Serial.println("Failed to communicate with sensor.");
  }
}

void loop() {
  if (Serial.available() > 0) {
    String user_input = Serial.readStringUntil('\n');

    if (user_input.equalsIgnoreCase("START")) {
      calibrating = true;
      check_previous_calibration();
    } else if (user_input.equalsIgnoreCase("STOP")) {
      calibrating = false;
      clear_calibration();
      Serial.println("Calibration process exited and previous calibration cleared.");
    } else {
      Serial.println("Invalid command. Please type 'START' to begin calibration or 'STOP' to exit.");
    }
  }

  if (calibrating) {
    calibrate_sensor();
  }
}

void print_instructions() {
  Serial.println("Automatic Three-Point Calibration Process for pH Sensor");
  Serial.println("Type 'START' to initiate the calibration process.");
  Serial.println("Type 'STOP' to exit the calibration process at any time.");
}

void check_previous_calibration() {
  PH.send_cmd("Cal,?");
  delay(300);
  if (PH.receive_cmd(sensor_data, sizeof(sensor_data)) == Ezo_board::SUCCESS) {
    int cal_status = atoi(sensor_data);
    if (cal_status == 1) {
      was_previously_calibrated = true;
      PH.send_cmd("Slope,?");
      delay(300);
      if (PH.receive_cmd(sensor_data, sizeof(sensor_data)) == Ezo_board::SUCCESS) {
        Serial.print("Current calibration slope: ");
        Serial.println(sensor_data);
        strncpy(initial_slope, sensor_data, sizeof(initial_slope));
      }
    } else {
      clear_calibration();
    }
    start_calibration();
  } else {
    Serial.println("Failed to read calibration status.");
    calibrating = false;
  }
}

void clear_calibration() {
  PH.send_cmd("Cal,clear");
  delay(300);
  if (PH.receive_cmd(sensor_data, sizeof(sensor_data)) == Ezo_board::SUCCESS) {
    Serial.println("Previous calibration cleared.");
  } else {
    Serial.println("Failed to clear previous calibration.");
  }
}

void start_calibration() {
  Serial.println("Starting automatic three-point calibration...");
  Serial.println("Place the probe in pH 7.00 calibration solution and wait for stabilization.");
}

void calibrate_sensor() {
  static int step = 0;
  static float last_readings[4] = {0, 0, 0, 0};
  static int stable_count = 0;
  static unsigned long step_start_time = millis();
  
  PH.send_read_cmd();
  delay(1000);
  if (PH.receive_cmd(sensor_data, sizeof(sensor_data)) == Ezo_board::SUCCESS) {
    float current_reading = atof(sensor_data);
    Serial.print("Current pH reading: ");
    Serial.println(current_reading);

    // Shift the readings and add the current one
    for (int i = 3; i > 0; i--) {
      last_readings[i] = last_readings[i - 1];
    }
    last_readings[0] = current_reading;

    // Check if the last four readings are the same
    if (last_readings[0] == last_readings[1] && last_readings[1] == last_readings[2] && last_readings[2] == last_readings[3]) {
      stable_count++;
    } else {
      stable_count = 0;
    }

    if (stable_count >= 4) {
      switch (step) {
        case 0:
          PH.send_cmd("Cal,mid,7.00");
          Serial.println("Cal,mid,7.00 command sent.");
          Serial.println("Place the probe in pH 4.00 calibration solution and wait for stabilization.");
          step++;
          stable_count = 0;
          step_start_time = millis();
          break;
        case 1:
          PH.send_cmd("Cal,low,4.00");
          Serial.println("Cal,low,4.00 command sent.");
          Serial.println("Place the probe in pH 10.00 calibration solution and wait for stabilization.");
          step++;
          stable_count = 0;
          step_start_time = millis();
          break;
        case 2:
          PH.send_cmd("Cal,high,10.00");
          Serial.println("Cal,high,10.00 command sent.");
          display_slope();
          Serial.println("Calibration completed.");
          if (was_previously_calibrated) {
            Serial.print("Initial calibration slope: ");
            Serial.println(initial_slope);
          }
          calibrating = false;
          step = 0;
          break;
      }
    }

    if (millis() - step_start_time > 600000) { // 10 minutes timeout for each step
      Serial.println("Calibration step timeout. Please try again.");
      clear_calibration();
      calibrating = false;
      step = 0;
    }
  } else {
    Serial.println("Failed to read from sensor.");
  }
}

void display_slope() {
  PH.send_cmd("Slope,?");
  delay(300);
  if (PH.receive_cmd(sensor_data, sizeof(sensor_data)) == Ezo_board::SUCCESS) {
    Serial.print("Final calibration slope: ");
    Serial.println(sensor_data);
  } else {
    Serial.println("Failed to read calibration slope.");
  }
}
