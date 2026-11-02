#include <prashantyadav967595-project-1_inferencing.h>
#include <Wire.h>
#include "BluetoothSerial.h"

// ---------------- Bluetooth setup ----------------
BluetoothSerial SerialBT;

// ---------------- MPU6050 setup ----------------
const int MPU_ADDR = 0x68;
const float ACCEL_LSB_PER_G = 2048.0;   // for +/-16g range
const float GYRO_LSB_PER_DPS = 16.4;    // for +/-2000 deg/s range

const unsigned long SAMPLE_INTERVAL_MS = 20;  // 20ms = 50Hz
unsigned long nextSampleTime = 0;

// ---------------- Edge Impulse feature buffer ----------------
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
int featureIndex = 0;

// Crash alert cooldown (10 seconds)
unsigned long lastAlertTime = 0;
const unsigned long ALERT_COOLDOWN_MS = 10000;

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void readSensor(float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  int16_t ax_raw = Wire.read() << 8 | Wire.read();
  int16_t ay_raw = Wire.read() << 8 | Wire.read();
  int16_t az_raw = Wire.read() << 8 | Wire.read();
  Wire.read(); Wire.read(); // skip temperature
  int16_t gx_raw = Wire.read() << 8 | Wire.read();
  int16_t gy_raw = Wire.read() << 8 | Wire.read();
  int16_t gz_raw = Wire.read() << 8 | Wire.read();

  // Convert to m/s^2 and deg/s
  ax = (ax_raw / ACCEL_LSB_PER_G) * 9.81;
  ay = (ay_raw / ACCEL_LSB_PER_G) * 9.81;
  az = (az_raw / ACCEL_LSB_PER_G) * 9.81;
  gx = gx_raw / GYRO_LSB_PER_DPS;
  gy = gy_raw / GYRO_LSB_PER_DPS;
  gz = gz_raw / GYRO_LSB_PER_DPS;
}

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  writeRegister(0x6B, 0x00);  // wake sensor
  delay(100);
  writeRegister(0x1C, 0x18);  // accel +/-16g
  writeRegister(0x1B, 0x18);  // gyro +/-2000 deg/s
  delay(100);

  SerialBT.begin("SmartHelmet");
  Serial.println("Bluetooth ON. Device name: SmartHelmet");

  Serial.println("Smart Helmet crash detection active...");
  nextSampleTime = millis();
}

void loop() {
  unsigned long now = millis();

  if (now >= nextSampleTime) {
    nextSampleTime += SAMPLE_INTERVAL_MS;

    float ax, ay, az, gx, gy, gz;
    readSensor(ax, ay, az, gx, gy, gz);

    if (featureIndex < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
      features[featureIndex++] = ax;
      features[featureIndex++] = ay;
      features[featureIndex++] = az;
      features[featureIndex++] = gx;
      features[featureIndex++] = gy;
      features[featureIndex++] = gz;
    }

    if (featureIndex >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
      ei_impulse_result_t result = { 0 };

      signal_t features_signal;
      features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
      features_signal.get_data = &raw_feature_get_data;

      EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);

      if (res == EI_IMPULSE_OK) {
        float crashScore = result.classification[0].value;   // index 0 = "CRASH"
        float normalScore = result.classification[1].value;  // index 1 = "NORMAL"

        Serial.print("CRASH: ");
        Serial.print(crashScore, 3);
        Serial.print("    NORMAL: ");
        Serial.println(normalScore, 3);

        // Calculate total G-force magnitude
        float totalG = sqrt(ax*ax + ay*ay + az*az) / 9.81;

        // BALANCED HYBRID CONDITION: AI score > 0.78 AND physical impact > 3.0G
        if (crashScore > 0.78 && totalG > 3.0) {
          Serial.println(">>> CRASH DETECTED! <<<");

          unsigned long nowAlert = millis();
          if (nowAlert - lastAlertTime > ALERT_COOLDOWN_MS) {
            SerialBT.println("CRASH_ALERT");
            Serial.println("Bluetooth pe CRASH_ALERT bhej diya.");
            lastAlertTime = nowAlert;
          }
        }
      } else {
        Serial.print("Classifier error: ");
        Serial.println(res);
      }

      featureIndex = 0;
    }
  }
}
