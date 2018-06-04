/*
 * main.c
 * IVA algrithm
 */

#include <sys.h>
#include <led.h>
#include <timer.h>
#include <sysTask.h>
int time_len = 0;
void main()
 {
	/* CSL IRQ IVA init */
	sys_init();
	/* led configuration */
	led_configuration();
	/* timer configuration */
	timer0_configuration();

	timer1_configuration();
	float in_buffer[2]={0};
	float **out_buffer;
	int i = 0;

	Uint32 time_recorder = 0;
	while(1)
	{
		time_recorder = SYS_TIME;
		for (i = 0; i < SHIFT_SIZE; i++)
		{
			if ((out_buffer=iva_step(&iva_instance, in_buffer))!=NULL);
		}
		time_len = SYS_TIME - time_recorder;
	}
}
