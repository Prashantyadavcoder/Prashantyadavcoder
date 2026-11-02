# Smart Helmet — ESP32 Crash Detection

This folder contains the Arduino sketch for the SmartHelmet ESP32 project.

## Hardware
- ESP32 board
- MPU6050 accelerometer/gyroscope module
- Bluetooth Classic receiver/phone app

## Arduino libraries required
1. Wire (included with the ESP32 Arduino core)
2. BluetoothSerial (included with the ESP32 Arduino core)
3. The Edge Impulse Arduino library exported for project `prashantyadav967595-project-1`

## Install the Edge Impulse library
The generated Edge Impulse library ZIP is not included in this public repository. It contains generated SDK code with license terms that may restrict use or redistribution. Use the original ZIP from your Edge Impulse account, and check that your plan/license permits your intended use before sharing it publicly.

In Arduino IDE:
1. Select **Sketch → Include Library → Add .ZIP Library**.
2. Choose your original `ei-prashantyadav967595-project-1-arduino-1.0.1-impulse.zip` file.
3. Open `SmartHelmet.ino` from this folder.
4. Select an ESP32 board and the correct port, then compile/upload.

## Important model assumptions
The sketch assumes classifier output index 0 is `CRASH` and index 1 is `NORMAL`. Confirm the label order in your Edge Impulse model before relying on the output. It also uses a 3.0G threshold and 0.78 model-score threshold.

**Safety note:** This is a prototype, not a certified crash-detection or emergency-response system. Test carefully and do not rely on it as the sole means of detecting an accident or requesting help.
