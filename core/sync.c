/********************************************************
 * Filename: core/sync.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Routines for semaphores and condition variables
 ********************************************************/

#include <core/eos.h>

void eos_init_semaphore(eos_semaphore_t *sem, int32u_t initial_count, int8u_t queue_type)
{
    sem->count = initial_count;
    sem->queue_type = queue_type;
}

int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout)
{
    eos_counter_t *system_timer = eos_get_system_timer();
    if (sem->count > 0)
    {
        sem->count -= 1;
        return sem->count;
    }
    else
    {
        // timeout이 음수일 땐 바로 리턴 -> count>0 일 때만 전제
        if (timeout == -1)
            return 0;
        else if (timeout >= 1)
        {
            eos_alarm_t *new_alarm = malloc(sizeof(eos_alarm_t));
            eos_tcb_t *current_task = eos_get_current_task();
            current_task->sleep_at = system_timer->tick;
            eos_set_alarm(system_timer, new_alarm, timeout, _os_wakeup_sleeping_task, current_task);
            // _os_add_node_priority(&(sem->wait_queue), current_task->queueing_node); wait queue를 이렇게 별도로 둬야하는지?
        }
        // timeout == 0
        else {
            eos_sleep(0);
        }
    }
}

void eos_release_semaphore(eos_semaphore_t *sem)
{
    // To be filled by students: Project 4
}

/**
 * Condition variables are not covery in the OS course
 */

void eos_init_condition(eos_condition_t *cond, int32u_t queue_type)
{
    cond->wait_queue = NULL;
    cond->queue_type = queue_type;
}

void eos_wait_condition(eos_condition_t *cond, eos_semaphore_t *mutex)
{
    /* Releases acquired semaphore */
    eos_release_semaphore(mutex);
    /* Waits on condition's wait_queue */
    _os_wait(&cond->wait_queue);
    /* Acquires semaphore before returns */
    eos_acquire_semaphore(mutex, 0);
}

void eos_notify_condition(eos_condition_t *cond)
{
    /* Selects a task in wait_queue and wake it up */
    _os_wakeup_single(&cond->wait_queue, cond->queue_type);
}

int8u_t eos_lock_scheduler()
{
    int32u_t flag = hal_disable_interrupt();
    int8u_t temp = _os_scheduler_lock;
    _os_scheduler_lock = LOCKED;
    hal_restore_interrupt(flag);
    return temp;
}

void eos_restore_scheduler(int8u_t scheduler_state)
{
    int32u_t flag = hal_disable_interrupt();
    _os_scheduler_lock = scheduler_state;
    hal_restore_interrupt(flag);
    eos_schedule();
}

int8u_t eos_get_scheduler_lock()
{
    return _os_scheduler_lock;
}
