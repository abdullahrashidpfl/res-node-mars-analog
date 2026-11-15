# ARES-NODE: Terrestrial Analog Telemetry Probe
NASA Space Apps Challenge 2026 (Islamabad Chapter)

## Overview
Before sending rovers or establishing habitats on Mars, space agencies require verified Earth analog sites. **ARES-NODE** is an edge-telemetry probe built using low-cost microcontrollers and open NASA datasets to evaluate and score how closely a terrestrial test site mimics Martian environmental baselines.

## Hardware Architecture
* **Microcontroller:** ESP32 Dev Board (3.3V Logic, 12-bit ADC)
* **Atmospheric Sensor:** DHT11 (Temperature & Relative Humidity)
* **Soil Subsystem:** DIY Resistive Soil Moisture Sensor (10k ohm voltage divider connected to ADC GPIO 34)

## NASA Open Data Integration
The firmware embeds empirical benchmark constants sourced directly from:
1. **NASA Goddard Mars Fact Sheet** (Global mean surface temperature and pressure profiles).
2. **NASA MSL Curiosity Rover REMS Data** (Gale Crater diurnal thermal swings and hyper-arid regolith moisture baselines < 2%).

## Firmware Logic
The device computes a weighted **Mars Analog Suitability Index (MASI)**:
* Soil Aridity Match (45%)
* Atmospheric Dryness Match (35%)
* Thermal Envelope Match (20%)
