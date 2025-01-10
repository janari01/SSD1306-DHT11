#ifndef DHT_H
#define DHT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wiringPi.h>

#define MAXTIMINGS              85
#define DHTPIN                  4

int* read_sensor();

#endif