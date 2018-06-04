/*
 * sysTask.c
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */
#include <sysTask.h>
#include <iva.h>
#include <sys.h>

static void task_1ms(void)
{

}

static void task_2ms(void)
{

}

static void task_5ms(void)
{

}

static void task_10ms(void)
{

}

static void task_20ms(void)
{

}


static void task_50ms(void)
{

	led_toggle();

}


void sys_task(void)
{
	task_1ms();
	if (SYS_TIME%2==0)task_2ms();
	if (SYS_TIME%5==0)task_5ms();
	if (SYS_TIME%10==0)task_10ms();
	if (SYS_TIME%20==0)task_20ms();
	if (SYS_TIME%50==0)task_50ms();
}





