#include <core/eos.h>
#define STACK_SIZE 8096

static int8u_t stack0[STACK_SIZE]; // stack for task0
static int8u_t stack1[STACK_SIZE]; // stack for task1

static eos_tcb_t tcb0;
static eos_tcb_t tcb1;

static int8u_t stack0[STACK_SIZE];

// tcb for task1
static void print_numbers(void *arg)
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
static void print_alphabet(void *arg)
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