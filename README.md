# Two axis thrust vectoring tricopter drone

This project focuses on the development of an experimental FPV tricopter drone with a modular architecture and continuous two-axis thrust vectoring.
The platform uses a three-motor configuration (two rear motors and one front motor), where each propulsion unit can redirect its thrust through a dual-ring gimbal mechanism with full 360° rotational capability. This significantly increases maneuverability, attitude control authority, and fault tolerance compared with conventional multirotor designs.

The control system will be based on a Teensy 4.1 microcontroller programmed in C++, using ICM-42688-P inertial sensors and long-range LoRa communication. Power electronics will be integrated into the fixed main frame, while power and signals will be transferred to moving assemblies through slip rings.
In addition to real-time FPV flight, the platform will include onboard video recording to MicroSD storage and an independent action camera.

A core objective of the project is to develop an aircraft capable of:

- Reconfiguring its geometry on the ground by relocating modules thanks to its modular design.

- Automatically detecting structural changes or deformations.

- Adapting its control system to the current geometry.

- Maintaining stability under partial motor or actuator failures.

- Serving as a test platform for advanced flight-control strategies.

As a future extension, in-flight geometry reconfiguration may also be explored.
A custom transmitter/controller specifically designed for the platform will also be developed.

## Project Status

Currently in the conceptual design, component selection, and early embedded control development stage.
