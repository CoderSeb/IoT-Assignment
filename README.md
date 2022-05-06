# Arduino Nano 33 IoT Code

Author: Sebastian Åkerblom
Student id: sa224ny

## The idea

I wanted to measure the temperatures in a snake terrarium from both sides as this terrarium has a heatmat and a heatlamp on one side.
It's a glass terrarium which in the winter time leaks a lot of heat so I wanted these measurements to be able to adjust the heat accordingly.

This was managed using the Arduino nano 33 IoT microcontroller and 2x DS18B20 waterproof temperature sensors.

Overview (device on top with the temperature sensor cables going off on left and right):

![Full image](./img/full_img_terrarium.jpg)

View of the device from top (I finally opted to use the micro usb cable as the device drained a 9V battery overnight):

![Birdseye view of device](./img/birdseye_device.jpg)

## Curcuit

Diagram:

![Circuit diagram](./img/circuit_diagram.png)

## Installation

- Connect your micro controller to arduino iot cloud.
- Add variables temp_sensor_one & temp_sensor_two as Read Only float values and set interval to 10 seconds.
- Configure network settings.
- Go to sketch and edit the according to IoT-Assignment.ino.
- Upload the sketch to your device.

## Components

- 1x Arduino Nano 33 IoT (Microcontroller)
- 2x DS18B20 (Temperature sensors)
- 1x 4.7k resistor
- 1x Breadboard
- ?x Jumper wires