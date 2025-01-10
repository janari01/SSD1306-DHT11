# SSD1306 and DHT11 Project

## Introduction
The purpose of this project is to display temperature and humidity on the SSD1306 display.
Furthermore, parabola animation between iterations.

## Connecting to Raspberry Pi
### SSD1306
* VCC -> 3.3V
* Grnd -> ground pin
* SCL -> GPIO 3 (SCL)
* SDA -> GPIO 2 (SDA)

### DHT11
* Signal -> GPIO pin
* VCC -> 5V
* Grnd -> ground pin

## Example
Code toggles between the DHT11 module and parabola animation.
<div style="display: flex; flex-wrap: wrap; gap: 20px;">
  <img src="/images/imgs2.jpg" alt="SSD1306" style="object-fit: cover; width: auto; height: 275px;">
  <img src="/images/imgs1.jpg" alt="SSD1306" style="object-fit: cover; width: auto; height: 275px;">
</div>

## Notes
Data was gathered from documentations and other sources.
It is **not** my priority to develop this project to its fullest.
* Specifically made for 128x64 version
* Tested on Raspberry Pi 5
