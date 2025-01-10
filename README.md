# DHT11 and SSD1306 Project

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
![SSD1306](https://github.com/janari01/SSD1306-DHT11/tree/main/images/imgs2.jpg?raw=true)
![SSD1306](https://github.com/janari01/SSD1306-DHT11/tree/main/images/imgs1.jpg?raw=true)

## Notes
Data was gather from documentations, as well from other sources.
It is **not** my priority to develop this project to its fullest.
* Specifically made for 128x64 version
* Tested on Raspberry Pi 5