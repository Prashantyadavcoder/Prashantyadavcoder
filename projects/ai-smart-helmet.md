# AI Smart Helmet — Crash Detection & Emergency SOS

**BCA (AI Specialization) final-year project** by Prashant Yadav, Future University, Bareilly.

A prototype smart-helmet system designed to detect motorcycle crashes using on-device TinyML and notify a paired Android phone, which can send an emergency SMS with GPS coordinates to a saved contact.

## System architecture

`MPU6050` → `ESP32 + TinyML` → Bluetooth Classic (`ALERT`) → `MIT App Inventor Android app` → GPS location + emergency SMS

## Hardware and tools

- ESP32 Dev Module — sensor processing, TinyML inference and Bluetooth
- MPU6050 — 3-axis accelerometer and 3-axis gyroscope
- TP4056 charging module, MT3608 boost converter and 3.7V Li-ion battery
- Android smartphone running the companion app
- Edge Impulse for TinyML model training
- Arduino C++ firmware and MIT App Inventor Blocks

## Detection workflow

1. Collect labelled MPU6050 motion samples for **CRASH** and **NORMAL** classes.
2. Train a lightweight classifier in Edge Impulse and export an Arduino inference library.
3. Sample motion at 50 Hz and run inference on a rolling window (about 400 ms).
4. Treat two consecutive confident crash predictions (confidence above 90% in the described firmware) as a crash to reduce false alarms.
5. Send the text `ALERT` over Bluetooth Classic (SPP) to the paired Android app.
6. The app reads the phone's GPS location and sends an SMS containing a Google Maps link to the saved emergency contact.

## Prototype highlights

- ESP32 and MPU6050 mounted inside the helmet shell.
- Rechargeable battery power chain for standalone operation.
- Bluetooth crash-alert delivery tested with live hardware, as described in the project presentation.

## Future improvements

- Add a dedicated GSM/SIM module so alerts do not depend on the rider's phone.
- Expand training data and distinguish a real fall from simply removing the helmet.
- Explore integration with emergency services and a custom PCB.

> This repository page documents the project based on the presentation. The system is a prototype; it should not be treated as a certified safety device or relied on as the sole emergency-alert method.
