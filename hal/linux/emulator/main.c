/********************************************************
 * Filename: hal/linux/emulator/main.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/27/24
 *
 * Description: Entry point of eOS and dispatch interrupts
 ********************************************************/

#include "private.h"
#include "../emulator.h"
#include <core/eos.h>
#define STACK_SIZE 8096

int32u_t _eflags_saved;

static int8u_t stack0[STACK_SIZE];
int8u_t stack1[STACK_SIZE];
eos_tcb_t tcb0;
eos_tcb_t tcb1;
// tcb for task1
void print_numbers(void *arg)
{
    // Function for task0: Print numbers from 1 to 20 repeatedly
    int i = 1;
    while (1)
    {
        printf("%d", i);
        eos_schedule();
        // Yield CPU to task1
        if (i++ == 20)
        {
            i = 1;
        }
    }
}
void print_alphabet(void *arg)
{
    // Function for task1: Print alphabet from ‘a’ to ‘z’ repeatedly
    int i = 97;
    while (1)
    {
        printf("%c", i);
        eos_schedule();
        // Yield CPU to task0
        if (i++ == 122)
        {
            i = 97;
        }
    }
}
void eos_user_main()
{
    // This is the test code for Project 2
    // Note that you must use a priority of 0 only since the priority-based scheduling hasn’t been implemented yet
    eos_create_task(&tcb0, (addr_t)stack0, STACK_SIZE, print_numbers, NULL, 0);
    eos_create_task(&tcb1, (addr_t)stack1, STACK_SIZE, print_alphabet, NULL, 0);
}

/**
 * Entry point of eOS:
 * 	eOS starts as if it were reset -- jump to the reset handler
 */
int main(int argc, char **argv)
{
    PRINT("Processing reset\n");
    _eflags = 0;

    // eos_user_main();

    __asm__ __volatile__(
        "jmp *%0		\n\t"
        :
        : "r"(_vector[0]));

    /* Never reaches here */
    return 0;
}

/**
 * Delivers an irq to CPU
 * 	Emulation of CPU's reaction to the delivered irq
 */
void _deliver_irq()
{
    // PRINT("_eflags: %d, _irq_pending: 0x%x, _irq_mask: 0x%x\n", _eflags, _irq_pending, _irq_mask);
    //  조사한 결과로는 여러 인터럽트가 있을 때 특정 인터럽트가 masking은 안 되어있지만 pending으로 체크가 되어있을 때 interrupt를 실행한다고 하는데..
    //  여기서는 인터럽트가 timer 밖에 없고 다른 건 없지 않나? -> 이 변수 자체가 extern으로 설정되어 있고, 체크를 하고 실행하니까 조건은 매겨진다고 봐야겠군
    //  그럼에도 vector table에는 handler가 1개 밖에 없는데,,?
    if (_irq_pending & ~_irq_mask)
    {
        if (_eflags == 1)
        {
            // Hardware disables interrtupts and saves previous _eflags
            _eflags_saved = _eflags;
            _eflags = 0;

            // Calls _os_irq_handler to execute the generic irq handler
            __asm__ __volatile__(
                "push $resume_eip	\n\t"
                "jmp *%0		\n\t"
                "resume_eip:			\n\t"
                :
                : "r"(_vector[3]));
        }
        // Else CPU silently ignores the interrupt
    }
    // Else CPU silently ignores the interrupt
}
