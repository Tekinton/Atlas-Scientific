/*
 * This Arduino program is designed to calibrate a pH sensor using the EZO I2C library. 
 * It performs a three-point calibration (midpoint, low point, and high point) by 
 * ensuring the sensor readings are stable over a defined number of consecutive windows.
 * 
 * Key Features:
 * - Displays the sensor's slope at the beginning of the program.
 * - Prompts the user to input the number of consecutive stable windows required for calibration.
 * - Provides commands to start calibration, restart calibration, and check stability of readings.
 * - Uses a moving window of pH readings to calculate average and standard deviation.
 * - Executes calibration commands when the readings are stable, indicated by the standard 
 * deviation being below a defined threshold for a specified number of consecutive windows.
 * - Includes user prompts and detailed feedback on the stability of readings during the calibration process.
 * 
 * Commands:
 * - 'start': Begins the calibration process.
 * - 'restart': Restarts the calibration process.
 * - 'check': Displays a sample of the stability check.
 * - 'set_stable_windows <number>': Sets the number of consecutive stable windows required for calibration.
 * 
 * This code ensures accurate and reliable calibration of the pH sensor by using statistical methods 
 * to verify the stability of readings.
 * 
 * Manufacturer Information:
 * - Manufacturer: Atlas Scientific
 * - Model: EZO-pH™ Embedded pH Circuit
 * 
 * License:
 * MIT License
 * 
 * Credits:
 * This code is based on the basic functionality of EZO I2C library and examples provided by Atlas Scientific.
 *
 * Developer: Mehmet Tekin 
 * Github: Tekinton
 */

#include <Wire.h>             // include Arduino's I2C library
#include <Ezo_i2c.h>          // include the EZO I2C library
#include <Ezo_i2c_util.h>     // include common print statements

Ezo_board pH_sensor(99);  // Replace 99 with the actual I2C address of your pH sensor

const int windowSize = 5;  // Increased window size for averaging readings
float readings[windowSize];  // Array to store the most recent pH readings
int readIndex = 0;  // Index to keep track of the current position in the readings array
float total = 0;  // Sum of the readings in the window for calculating the average
float average = 0;  // Average of the readings in the window
float threshold = 0.005;  // Threshold for the standard deviation to determine stability
int stableWindowsRequired = 3;  // Number of consecutive stable windows required
int stableWindows = 0;  // Counter for stable windows
bool restart = false;  // Flag to indicate if the calibration process should restart
bool manualSet = false;  // Flag to indicate if a manual set command was received

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(1000);  // Wait for 1 second
  displaySlope();
  requestStableWindows();
  Serial.println("Type 'start' to begin the calibration process or 'check' to display stability check sample.");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "start") {
      Serial.println("Starting calibration process...");
      calibrationProcess();
    } else if (command == "restart") {
      restart = true;
      Serial.println("Restarting calibration process...");
      calibrationProcess();
    } else if (command == "check") {
      displayStabilityCheckSample();
    }
  }
}

void calibrationProcess() {
  restart = false;
  manualSet = false;
  Serial.println("Clearing previous calibration...");
  pH_sensor.send_cmd("Cal,clear");
  delay(300);  // Give some time for the clear calibration command to execute
  Serial.println("Previous calibration cleared.");
  midpointCalibration();
  if (restart) return;
  lowPointCalibration();
  if (restart) return;
  highPointCalibration();
  if (restart) return;
  displaySlope();
}

void midpointCalibration() {
  Serial.println("Step 1: Midpoint Calibration");
  Serial.println("Please place the probe in pH 7.00 Calibration Solution and type 'ok'.");
  waitForUserConfirmation();
  if (restart) return;
  calibrate("Cal,mid,7.00");
}

void lowPointCalibration() {
  Serial.println("Step 2: Low Point Calibration");
  Serial.println("Please place the probe in pH 4.00 Calibration Solution and type 'ok'.");
  waitForUserConfirmation();
  if (restart) return;
  calibrate("Cal,low,4.00");
}

void highPointCalibration() {
  Serial.println("Step 3: High Point Calibration");
  Serial.println("Please place the probe in pH 10.00 Calibration Solution and type 'ok'.");
  waitForUserConfirmation();
  if (restart) return;
  calibrate("Cal,high,10.00");
}

void waitForUserConfirmation() {
  String userInput = "";
  while (true) {
    if (Serial.available() > 0) {
      userInput = Serial.readStringUntil('\n');
      userInput.trim();  // Remove any leading/trailing whitespace
      if (userInput == "ok") {
        break;
      } else if (userInput == "restart") {
        restart = true;
        return;
      }
    }
  }
}

void calibrate(const char* command) {
  for (int i = 0; i < windowSize; i++) {
    readings[i] = 0;
  }
  readIndex = 0;
  total = 0;
  average = 0;

  stableWindows = 0;  // Reset stable windows counter

  while (true) {
    if (restart) return;  // Check for restart before sending command

    pH_sensor.send_read_cmd();
    delay(1000);  // Increased delay between readings
    pH_sensor.receive_read_cmd();
    float currentReading = pH_sensor.get_last_received_reading();

    Serial.print("Current pH reading: ");
    Serial.println(currentReading, 3);

    // Update the readings array and compute average
    total = total - readings[readIndex];
    readings[readIndex] = currentReading;
    total = total + readings[readIndex];
    readIndex = (readIndex + 1) % windowSize;

    if (readIndex == 0) {  // Only check once we have a full window of readings
      average = total / windowSize;
      float stdDev = calculateStandardDeviation(readings, windowSize);
      Serial.print("Current average: ");
      Serial.println(average, 3);
      Serial.print("Current standard deviation: ");
      Serial.println(stdDev, 3);
      if (stdDev < threshold) {
        stableWindows++;
        Serial.print("Stable windows: ");
        Serial.println(stableWindows);
        if (stableWindows >= stableWindowsRequired) {  // Check for stability over required consecutive windows
          Serial.println("Readings are stable. Executing calibration command.");
          break;
        }
      } else {
        Serial.println("Readings are not stable yet.");
        stableWindows = 0;
      }
    }

    if (Serial.available() > 0) {
      String userCommand = Serial.readStringUntil('\n');
      userCommand.trim();
      if (userCommand == "restart") {
        restart = true;
        return;
      } else if (userCommand == "set") {
        manualSet = true;
        break;
      }
    }
  }

  if (restart) return;

  if (manualSet) {
    Serial.println("Manual set command received. Executing calibration command directly.");
    manualSet = false;  // Reset manualSet flag after sending command
  }

  pH_sensor.send_cmd(command);
  delay(300);  // Give some time for the calibration command to execute
  Serial.println("Calibration command executed: ");
  Serial.println(command);
}

float calculateStandardDeviation(float* data, int length) {
  float mean = 0;
  for (int i = 0; i < length; i++) {
    mean += data[i];
  }
  mean = mean / length;

  float sum = 0;
  for (int i = 0; i < length; i++) {
    sum += pow(data[i] - mean, 2);
  }
  return sqrt(sum / length);
}

void displaySlope() {
  Serial.println("Displaying Slope...");
  pH_sensor.send_cmd("Slope,?");
  delay(300);

  char response[32];
  Ezo_board::errors error = pH_sensor.receive_cmd(response, 32);

  if (error == Ezo_board::SUCCESS) {
    Serial.print("Slope: ");
    Serial.println(response);
  } else {
    Serial.print("Error retrieving slope: ");
    Serial.println(error);
  }
}

void requestStableWindows() {
  Serial.println("Enter the number of consecutive stable windows required for calibration:");
  while (true) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      int value = input.toInt();
      if (value > 0) {
        stableWindowsRequired = value;
        Serial.print("Stable windows required set to: ");
        Serial.println(stableWindowsRequired);
        break;
      } else {
        Serial.println("Invalid number. Please enter a positive integer.");
      }
    }
  }
}

void displayStabilityCheckSample() {
  Serial.println("Displaying a sample of stability check...");
  
  for (int i = 0; i < windowSize; i++) {
    pH_sensor.send_read_cmd();
    delay(2000);  // Increased delay between readings
    pH_sensor.receive_read_cmd();
    float currentReading = pH_sensor.get_last_received_reading();
    
    readings[i] = currentReading;
    Serial.print("Reading ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(currentReading, 3);
  }

  float average = 0;
  for (int i = 0; i < windowSize; i++) {
    average += readings[i];
  }
  average /= windowSize;

  float stdDev = calculateStandardDeviation(readings, windowSize);

  Serial.print("Average of last ");
  Serial.print(windowSize);
  Serial.print(" readings: ");
  Serial.println(average, 3);

  Serial.print("Standard deviation of last ");
  Serial.print(windowSize);
  Serial.print(" readings: ");
  Serial.println(stdDev, 3);
}
