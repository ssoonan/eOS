/********************************************************
 * Filename: core/task.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 04/06/24
 *
 * Description: task creation and scheduling
 ********************************************************/

#include <core/eos.h>
#include "eos.h"

#define READY 1
#define RUNNING 2
#define WAITING 3
#define SUSPENDED 4

/**
 * Runqueue of ready tasks
 */
static _os_node_t *_os_ready_queue[LOWEST_PRIORITY + 1];

/**
 * Pointer to TCB of the running task
 */
static eos_tcb_t *_os_current_task;

int32u_t eos_create_task(eos_tcb_t *task, addr_t sblock_start, size_t sblock_size, void (*entry)(void *arg), void *arg, int32u_t priority)
{
    PRINT("task: 0x%x, priority: %d\n", (int32u_t)task, priority);
    addr_t stack_pointer = _os_create_context(sblock_start, sblock_size, entry, arg);

    task->stack_start = sblock_start;
    task->stack_pointer = stack_pointer;
    task->stack_size = sblock_size;

    task->entry = entry;
    task->arg = arg;
    task->state = READY;
    _os_set_ready(priority);

    _os_node_t *new_node = (_os_node_t *)malloc(sizeof(_os_node_t));
    if (new_node == NULL)
    {
        return 0; // Memory allocation failed
    }
    new_node->ptr_data = task;
    new_node->priority = priority;
    new_node->previous = NULL;
    new_node->next = NULL;
    task->queueing_node = new_node;

    _os_add_node_priority(&_os_ready_queue[priority], new_node);
    return 0;
}

int32u_t eos_destroy_task(eos_tcb_t *task)
{
    // To be filled by students: not covered
}

void eos_schedule()
{
    int32u_t priority = _os_get_highest_priority();
    PRINT("highest priority: %d\n", priority);
    _os_node_t *new_current_node;
    // 1. after boot, first time -> not save but only restore the task
    if (_os_current_task == NULL)
    {
        new_current_node = _os_ready_queue[priority];
        PRINT("after boot, first time\n");
        restore_and_erase_new_node(new_current_node, priority);
    }
    else
    {
        addr_t stopped_esp = _os_save_context();
        // 2. Return from restore, exit
        if (stopped_esp == NULL || stopped_esp == 0)
            return;

        // 3. record the saved task's stack pointer
        _os_current_task->stack_pointer = stopped_esp;
        // Add current task back to the ready queue
        _os_add_node_priority(&(_os_ready_queue[priority]), _os_current_task->queueing_node);

        // Select the next task to run.
        new_current_node = _os_ready_queue[priority];
        restore_and_erase_new_node(new_current_node, priority);
    }
}

void restore_and_erase_new_node(_os_node_t *new_current_node, int32u_t priority)
{
    if (new_current_node == NULL)
        return;
    _os_current_task = new_current_node->ptr_data;
    _os_remove_node(&(_os_ready_queue[priority]), new_current_node);

    if (_os_ready_queue[priority] == NULL)
        _os_unset_ready(priority);
    _os_restore_context(_os_current_task->stack_pointer);
}

eos_tcb_t *eos_get_current_task()
{
    return _os_current_task;
}

void eos_change_priority(eos_tcb_t *task, int32u_t priority)
{
    // To be filled by students: not covered
}

int32u_t eos_get_priority(eos_tcb_t *task)
{
    // To be filled by students: not covered
}

void eos_set_period(eos_tcb_t *task, int32u_t period)
{
    task->period = period;
}

int32u_t eos_get_period(eos_tcb_t *task)
{
    return task->period;
}

int32u_t eos_suspend_task(eos_tcb_t *task)
{
    // To be filled by students: not covered
}

int32u_t eos_resume_task(eos_tcb_t *task)
{
    // To be filled by students: not covered
}

void eos_sleep(int32u_t tick)
{
    if (tick == 0)
    {
        // 1. if period
        // 2. else
    }
    else
    {
        // 3. 콜백으로 tick만큼 대기 by eos_set_alarm
    }
}

void _os_init_task()
{
    PRINT("Initializing task module\n");

    /* Initializes current_task */
    _os_current_task = NULL;

    /* Initializes multi-level ready_queue */
    for (int32u_t i = 0; i < LOWEST_PRIORITY; i++)
    {
        _os_ready_queue[i] = NULL;
    }
}

void _os_wait(_os_node_t **wait_queue)
{
    // To be filled by students: not covered
}

void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type)
{
    // To be filled by students: not covered
}

void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type)
{
    // To be filled by students: not covered
}

void _os_wakeup_sleeping_task(eos_tcb_t *task)
{
    // 일단 둘 다 바꿈
    task->state = READY;
    _os_set_ready(task->queueing_node->priority);
    // TODO: ready queue에 넣는 과정까지 필요
    // _os_add_node_priority(&(_os_ready_queue[task->queueing_node->priority]))
}
