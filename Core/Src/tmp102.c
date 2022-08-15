/*
 * tmp102.c
 *
 *  Created on: Jul 16, 2022
 *      Author: hg26972
 */
#include <stdio.h>
#include <string.h>
#include "tmp102.h"

static const uint8_t TMP102_ADDR = 0x48 << 1;
static const uint8_t REG_TEMP = 0x00;

// Tell TMP102 that we want to read from the temperature register
// Temperature text tempText should defined as uint_t tempText[12]
void tmp102_ReadTemperature(uint8_t* tempText)
{
	uint8_t buf[12];
	buf[0] = REG_TEMP;
	HAL_StatusTypeDef ret;
	uint16_t val;
	float temp_c;

	ret = HAL_I2C_Master_Transmit(&TMP102_I2C_PORT, TMP102_ADDR, buf, 1, HAL_MAX_DELAY);

	if(ret != HAL_OK) {
		strcpy((char*)buf, "T102_Err\r\n");
	} else {

		// Read 2 bytes from the temperature register
		ret = HAL_I2C_Master_Receive(&TMP102_I2C_PORT, TMP102_ADDR, buf, 2, HAL_MAX_DELAY);
		if(ret != HAL_OK) {
			strcpy((char*)buf, "ErrT102\r\n");
		} else {
			// Combine the bytes
			val = ((int16_t)buf[0] << 4) | (buf[1] >> 4);

			// Convert to 2's complement, since temperature can be negative
			if(val > 0x7FF) {
			  val |= 0xF000;
			}

			// Convert to float temperature value (Celsius)
			temp_c = val * 0.0625;

			// Convert temperature to decimal format
			temp_c *= 100;
			sprintf((char*)tempText, "%d.%02u C\r\n", (int16_t)temp_c/100, (int16_t)temp_c%100);
		}
	}

	HAL_UART_Transmit(&TMP102_UART_PORT, buf, strlen((char*)buf), HAL_MAX_DELAY);  // TODO for debug
}
