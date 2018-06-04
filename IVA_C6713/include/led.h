/*
 * led.h
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */

#ifndef LED_H_
#define LED_H_
#include <csl.h>
#include <csl_gpio.h>

void led_configuration(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
#endif /* LED_H_ */
