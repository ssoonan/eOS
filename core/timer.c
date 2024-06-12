/********************************************************
 * Filename: core/timer.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: User interface for handling interrupts
 ********************************************************/

#include <core/eos.h>

static eos_counter_t system_timer;


int8u_t eos_init_counter(eos_counter_t *counter, int32u_t init_value)
{
    counter->tick = init_value;
    counter->alarm_queue = NULL;

    return 0;
}


void eos_set_alarm(eos_counter_t* counter, eos_alarm_t* alarm, int32u_t timeout, void (*entry)(void *arg), void *arg)
{
    alarm->timeout = timeout;
    alarm->handler = entry;
    alarm->arg = arg;
    _os_add_node_tail(counter->alarm_queue, alarm);
}


eos_counter_t* eos_get_system_timer()
{
    return &system_timer;
}


void eos_trigger_counter(eos_counter_t* counter)
{
    PRINT("tick\n");
}


/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg)
{
    /* Triggers alarms */
    eos_trigger_counter(&system_timer);
}


void _os_init_timer()
{
    eos_init_counter(&system_timer, 0);

    /* Registers timer interrupt handler */
    eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0,
		    timer_interrupt_handler, NULL);
}
