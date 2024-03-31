#include <Wire.h>
#include <MPU6050_tockn.h> // Include MPU6050 library

MPU6050 mpu(Wire);

// Complementary filter constants
const float alpha = 0.98;
const float beta = 1 - alpha;

// Calibration variables
float initialPitch = 0.0;
float initialRoll = 0.0;
float initialYaw = 0.0;

bool calibrated = false;

// Moving average filter constants
const int numReadings = 10;
float pitchReadings[numReadings];
float rollReadings[numReadings];
float yawReadings[numReadings];
int pitchIndex = 0;
int rollIndex = 0;
int yawIndex = 0;
float pitchTotal = 0.0;
float rollTotal = 0.0;
float yawTotal = 0.0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);
  
  // Initialize MPU6050
  Wire.begin();
  mpu.begin();
  
  // Calibrate MPU6050
  MPU6050Calibration();
}

void loop() {
  if (!calibrated) {
    MPU6050Calibration();
    calibrated = true;
  }
  
  // Read and calculate MPU6050 sensor data
  MPU6050Calculation();
  
  // Delay for stability (adjust as needed)
  delay(10);
}

void MPU6050Calibration() {
  Serial.println("Calibrating MPU6050...");
  
  // Read initial sensor data
  mpu.update();
  initialPitch = atan2(mpu.getAccY(), sqrt(pow(mpu.getAccX(), 2) + pow(mpu.getAccZ(), 2))) * 180 / PI;
  initialRoll = atan2(-mpu.getAccX(), mpu.getAccZ()) * 180 / PI;
  initialYaw = atan2(mpu.getAccX(), mpu.getAccY()) * 180 / PI;

  Serial.println("MPU6050 Calibration Complete.");
}

void MPU6050Calculation() {
  // Read MPU6050 sensor data
  mpu.update();
  
  // Get accelerometer angles
  float accPitch = atan2(mpu.getAccY(), sqrt(pow(mpu.getAccX(), 2) + pow(mpu.getAccZ(), 2))) * 180 / PI;
  float accRoll = atan2(-mpu.getAccX(), mpu.getAccZ()) * 180 / PI;
  float accYaw = atan2(mpu.getAccX(), mpu.getAccY()) * 180 / PI;

  // Apply moving average filter to smooth out accelerometer readings
  pitchTotal -= pitchReadings[pitchIndex];
  rollTotal -= rollReadings[rollIndex];
  yawTotal -= yawReadings[yawIndex];
  pitchReadings[pitchIndex] = accPitch;
  rollReadings[rollIndex] = accRoll;
  yawReadings[yawIndex] = accYaw;
  pitchTotal += accPitch;
  rollTotal += accRoll;
  yawTotal += accYaw;
  
  
  pitchIndex = (pitchIndex + 1) % numReadings;
  rollIndex = (rollIndex + 1) % numReadings;
  yawIndex = (yawIndex + 1) % numReadings;


  float pitch = pitchTotal / numReadings;
  float roll = rollTotal / numReadings;
  float yaw = yawTotal / numReadings;

  // Adjust angles based on initial calibration
  pitch -= initialPitch;
  roll -= initialRoll;
  yaw -= initialYaw;
  
  // Print sensor data
  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print(", Roll: ");
  Serial.print(roll);
  Serial.print(", Yaw: ");
  Serial.println(yaw);
}