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
    sem->wait_queue = NULL;
}

int32u_t eos_acquire_semaphore(eos_semaphore_t *sem, int32s_t timeout)
{
    // 0) semaphore가 있을 때 -> 성공
    if (sem->count > 0)
    {
        int32u_t flag = hal_disable_interrupt();
        sem->count -= 1;
        hal_restore_interrupt(flag);
        return 1;
    }
    // 1) 2) 3) semaphore가 없을 때 -> timeout 따른 추가 로직
    // 1) timeout이 음수일 땐 바로 리턴 -> count>0 일 때만 전제
    if (timeout == -1)
        return 0;
    // 2) timeout 양수 -> 해당 시간만큼 대기, 이후 다시 체크
    else if (timeout >= 1)
    {
        eos_sleep(timeout);
        return 0;
    }
    // 3) 무한히 wait, 깨어난 이후 다시 검사
    else
    {
        while (1)
        {
            _current_task_to_wait_queue(&(sem->wait_queue));
            // 3-1) 깨어난 뒤 count > 0 -> 성공
            while (sem->count > 0)
            {
                int32u_t flag = hal_disable_interrupt();
                sem->count -= 1;
                hal_restore_interrupt(flag);
                return 1;
            }
        }
    }
}

void eos_release_semaphore(eos_semaphore_t *sem)
{
    int32u_t flag = hal_disable_interrupt();
    sem->count += 1;
    hal_restore_interrupt(flag);

    _os_node_t *head = sem->wait_queue;
    if (head)
    {
        _os_wakeup_sleeping_task(head->ptr_data);
        _os_remove_node(&(sem->wait_queue), head);
    }
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
