Copyright 2026 Diego Rubio Mozo

This source describes the Gimbal Design 7.1 hardware and is licensed under the CERN Open Hardware Licence Version 2 - Weakly Reciprocal (CERN-OHL-W).

You may redistribute and modify this source under the terms of the CERN-OHL-W v2.0.

# Gimbal design - 7.0
This design was the basis for the 7.1 version. The only difference is that it uses the ESP32-C3 SuperMini instead of the ESP32-S3 SuperMini. The S3 has 2 cores, while the C3 only has one. I needed 2 cores: one for ESP-NOW communication and one for FOC control.