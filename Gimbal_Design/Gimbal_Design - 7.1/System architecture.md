Copyright 2026 Diego Rubio Mozo

This source describes the Gimbal Design 7.1 hardware and is licensed under the CERN Open Hardware Licence Version 2 - Weakly Reciprocal (CERN-OHL-W).

You may redistribute and modify this source under the terms of the CERN-OHL-W v2.0.

# Gimbal design - 7.1

System architecture

    BODY (this is not part of the Gimbal design 7.1)
        6S-5V Matek converter - 1x
        Teensy 4.1 - 1x
        ESP32-S3 SuperMini ESP-NOW command transmitter - 1x
        ESP32-S3 SuperMini ESP-NOW error report receiver - 1x
        1800MAh 100C 6S battery - 1x
        200A PDB - 1x
    BODY - AXIS 1
        Fixed side (BODY)
            6S-5V Matek converter - 1x
            ESP32-S3 SuperMini (BODY - AXIS 1) local controller - 1x
            AS5048A SPI module - 1x
            DRV8313 FOC motor driver - 1x
            MCP33131-10-E/MS ADCs - 2x
            Power Distribution Network (PDN) filters - x
            REF5025AIDR voltage reference - 1x
            AP7375-18SA-7 LDO - 1x
        Shared interface components (BODY - AXIS 1)
            GM3506 gimbal motor with belt drive reduction system - 1x
            20A 2CH slip ring - 2x
            6703RS bearing - 2x
        Extra rotating side (1:1 gear system)
            Diametric magnet - 1x
    AXIS 1 - AXIS 2
        Fixed side (AXIS 1)
            6S-5V Matek converter - 1x
            ESP32-S3 SuperMini (BODY - AXIS 1) local controller - 1x
            AS5048A SPI module - 1x
            DRV8313 FOC motor driver - 1x
            MCP33131-10-E/MS ADCs - 2x
            Power Distribution Network (PDN) filters - x
            REF5025AIDR voltage reference - 1x
            AP7375-18SA-7 LDO - 1x
        Shared interface components (AXIS 1 - AXIS 2)
            GM3506 gimbal motor with belt drive reduction system - 1x
            20A 2CH slip ring - 2x
            6703RS bearing - 2x
        Rotating side (AXIS 2)
            6S-5V Matek converter - 1x
            Power Distribution Network (PDN) filters - x
            ESP32-S3 SuperMini ESCs local controller - 1x
            2207 1800KV 6S motor - 2x
            HQProp MCK 5130 - 2x
            ESC AM32 40A 6S - 2x
            Motor holder structure - 1x
        Extra rotating side (1:1 gear system)
            Diametric magnet - 1x