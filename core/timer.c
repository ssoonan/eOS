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

void eos_set_alarm(_os_node_t **wait_queue, eos_alarm_t *alarm, int32u_t timeout, void (*entry)(void *arg), void *arg)
{
    _os_remove_node(wait_queue, &(alarm->alarm_queue_node));
    if (timeout == 0 || timeout == NULL)
        return;

    alarm->timeout = timeout;
    alarm->handler = entry;
    alarm->arg = arg;

    alarm->alarm_queue_node.ptr_data = alarm;
    alarm->alarm_queue_node.priority = timeout;

    _os_add_node_tail(wait_queue, &(alarm->alarm_queue_node));
}

eos_counter_t *eos_get_system_timer()
{
    return &system_timer;
}

void eos_trigger_counter(eos_counter_t *counter, _os_node_t **queue)
{
    PRINT("tick\n");
    counter->tick += 1;
    _os_node_t *head;
    if (queue)
        head = *queue;
    else
        head = counter->alarm_queue;
    _os_node_t *alarm_node = head;
    while (1)
    {
        eos_alarm_t *alarm = alarm_node->ptr_data;
        eos_tcb_t *alarm_task = (eos_tcb_t *)(alarm->arg);
        if (counter->tick >= alarm_task->sleep_at + alarm->timeout)
            alarm->handler(alarm->arg);
        alarm_node = alarm_node->next;
        if (alarm_node == head)
            break;
    }
}

/* Timer interrupt handler */
static void timer_interrupt_handler(int8s_t irqnum, void *arg)
{
    /* Triggers alarms */
    eos_trigger_counter(&system_timer, NULL);
    // eos_trigger_counter(&system_timer, NULL);
}

void _os_init_timer()
{
    eos_init_counter(&system_timer, 0);

    /* Registers timer interrupt handler */
    eos_set_interrupt_handler(IRQ_INTERVAL_TIMER0,
                              timer_interrupt_handler, NULL);
}
