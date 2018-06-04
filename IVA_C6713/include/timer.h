/*
 * timer.h
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */

#ifndef TIMER_H_
#define TIMER_H_
#include <csl_timer.h>
#include <csl_irq.h>
void timer0_configuration(void);
void timer1_configuration(void);
extern Uint32 sysTimeMicors;
#define SYS_TIME sysTimeMicors
#endif /* TIMER_H_ */
