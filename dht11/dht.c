#include "dht.h"
 
int* read_sensor() 
{
	wiringPiSetupGpio();
	uint8_t laststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;

	int* dht11_dat = malloc(5 * sizeof(int));
	if (dht11_dat == NULL) 
	{
    printf("Memory allocation failed!\n");
    return NULL; 
	}
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );

	for (i = 0; i < MAXTIMINGS; i++) 
	{
		counter = 0;

		while (digitalRead(DHTPIN) == laststate) 
		{
			counter++;
			delayMicroseconds(1);

			if (counter == 255) { break; }
		}

		laststate = digitalRead(DHTPIN); // digitalRead return either LOW or HIGH

		if (counter == 255) { break; }
		
		// avoid first 3 readings
		if ((i >= 4) && (i % 2 == 0)) 
		{
			// j / 8 to get the location of the i'th byte where bits are CURRENTLY being stored
			dht11_dat[j / 8] <<= 1; // shifting to make room for the new bit

			if (counter > 16) // ignore the first 16 data points
			{
				dht11_dat[j / 8] |= 1; // switching bits
			}

			j++;
		}
	}

	if (
		(j >= 40) &&
		(dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) // ensure that data fits into one byte
	) 
	{
		return dht11_dat;
	}

	else 
	{
    free(dht11_dat);
    return NULL;  // if data is invalid
	}

}