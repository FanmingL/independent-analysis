/*
 * timer.c
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */
#include <timer.h>
#define TIMER_CNT  100  		/* Maximum count value */
static TIMER_Handle hTimer0, hTimer1;
static Uint32 TimerEventId0, TimerEventId1;
Uint32 sysTimeMicors = 0;

/********************************************************************************/
static Uint32 TimerControl = TIMER_CTL_RMK( /* Timer control register (CTL)*/
  	TIMER_CTL_INVINP_NO, /* TINP inverter control(INVINP). Only affects operation
					      if CLKSRC =0.
                           TIMER_CTL_INVINP_NO  - Uninverted TINP drives timer
                           TIMER_CTL_INVINP_YES - inverted TINP drives timer */

  	TIMER_CTL_CLKSRC_CPUOVR4,/* Timer input clock source (CLKSRC)
						   TIMER_CTL_CLKSRC_CPUOVR4 - CPU clock /4           */

  	TIMER_CTL_CP_CLOCK, /* Clock/pulse mode(CP)
					       TIMER_CTL_CP_PULSE - Pulse mode.TSTAT is active one
						        CPU clock after the timer reaches the timer
								period.PWID determines when it goes inactive.*/

  	TIMER_CTL_HLD_YES, /* Hold(HLD). Counter may be read or written regardless of
					    HLD value.
						   TIMER_CTL_HLD_YES - Counter is disabled and held in
						        current value.
						   TIMER_CTL_HLD_NO - COunter is allowed to count.   */

  	TIMER_CTL_GO_NO, /* Go bit(GO). Resets and starts the timer counter.
				           TIMER_CTL_GO_NO - No effects on the timer.
						   TIMER_CTL_GO_YES - if HLD =1, the counter register
						        is zeroed and begins counting on next clock. */
  	TIMER_CTL_PWID_TWO, /* Pulse width(PWID). Only used in pulse mode.
					       TIMER_CTL_PWID_ONE - TSTAT goes inactive one timer
						        input clock cycle after the timer counter value
								equals the timer period value.
                           TIMER_CTL_PWID_TWO -  TSTAT goes inactive one timer
						        input clock cycle after the timer counter value
								equals the timer period value.               */

  	TIMER_CTL_DATOUT_0, /* Data output (DATOUT).
					       TIMER_CTL_DATOUT_0 - If FUNC  =0,the DATOUT is
						        driven on TOUT.
                           TIMER_CTL_DATOUT_1 - If FUNC =1,The DATOUT is driven
						        on TOUT after inversion by INVOUT.           */

  	TIMER_CTL_INVOUT_NO, /* TOUT inverter control (INVOUT)
					       TIMER_CTL_INVOUT_NO - Uninverted TSTAT drives TOUT
						   TIMER_CTL_INVOUT_YES - Inverted TSTAT drives TOUT.*/
  	TIMER_CTL_FUNC_TOUT /* Function of TOUT pin(FUNC).
					       TIMER_CTL_FUNC_GPIO - TOU is a general purpose
						        output pin
                           TIMER_CTL_FUNC_TOUT - TOUT is a timer output pin  */

);

void timer0_configuration(void)
{
	hTimer0 = TIMER_open(TIMER_DEV0,TIMER_OPEN_RESET);

		/* Obtain the event ID for the timer device. */
	TimerEventId0 = TIMER_getEventId(hTimer0);
	IRQ_map(TimerEventId0, 14);

	  	/* Reset the timer events. */
	IRQ_reset(TimerEventId0);
	TIMER_configArgs(hTimer0,
	    TimerControl, /* use predefined control value  */
	    0xFFFFFFFF,   /* set period                    */
	    0x00000000    /* start count value at zero     */
	  	);
	IRQ_enable(TimerEventId0);

	  /* Start the timers */
	TIMER_start(hTimer0);
}

void timer1_configuration(void)
{
	hTimer1 = TIMER_open(TIMER_DEV1,TIMER_OPEN_RESET);

		/* Obtain the event ID for the timer device. */
	TimerEventId1 = TIMER_getEventId(hTimer1);
	IRQ_map(TimerEventId1, 15);

	  	/* Reset the timer events. */
	IRQ_reset(TimerEventId1);
	TIMER_configArgs(hTimer1,
	    TimerControl, /* use predefined control value  */
	    50000,   /* set period                    */
	    0x00000000    /* start count value at zero     */
	  	);
	IRQ_enable(TimerEventId1);

	  /* Start the timers */
	TIMER_start(hTimer1);
}

/* timer0 IRQ Handler */
static
void timer0_TRQHandler(void)
{

}

/* timer1 IRQ Handler */
static
void timer1_TRQHandler(void)
{
	sysTimeMicors++;
	sys_task();
}

interrupt
void c_int14(void)
{
	timer0_TRQHandler();
}


interrupt
void c_int15(void)
{
	timer1_TRQHandler();
}


