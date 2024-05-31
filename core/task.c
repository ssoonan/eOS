/********************************************************
 * Filename: core/task.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 04/06/24
 *
 * Description: task creation and scheduling
 ********************************************************/

#include <core/eos.h>

#define READY		1
#define RUNNING		2
#define WAITING		3
#define SUSPENDED       4

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
    // To be filled by students: Projects 2 and 3
}


int32u_t eos_destroy_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_schedule() {
    // To be filled by students: Projects 2 and 3
}


eos_tcb_t *eos_get_current_task() {
	return _os_current_task;
}


void eos_change_priority(eos_tcb_t *task, int32u_t priority) {
    // To be filled by students: not covered
}


int32u_t eos_get_priority(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_set_period(eos_tcb_t *task, int32u_t period){
    // To be filled by students: Project 3
}


int32u_t eos_get_period(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_suspend_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


int32u_t eos_resume_task(eos_tcb_t *task) {
    // To be filled by students: not covered
}


void eos_sleep(int32u_t tick) {
    // To be filled by students: Project 3
}


void _os_init_task() {
    PRINT("Initializing task module\n");

    /* Initializes current_task */
    _os_current_task = NULL;

    /* Initializes multi-level ready_queue */
    for (int32u_t i = 0; i < LOWEST_PRIORITY; i++) {
        _os_ready_queue[i] = NULL;
    }
}


void _os_wait(_os_node_t **wait_queue) {
    // To be filled by students: not covered
}


void _os_wakeup_single(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_all(_os_node_t **wait_queue, int32u_t queue_type) {
    // To be filled by students: not covered
}


void _os_wakeup_sleeping_task(void *arg) {
    // To be filled by students: Project 3
    eos_tcb_t *task = (eos_tcb_t *) arg;
}
