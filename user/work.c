#include <core/eos.h>
#define STACK_SIZE 8096

static int8u_t stack0[STACK_SIZE]; // stack for task0
static int8u_t stack1[STACK_SIZE]; // stack for task1

static eos_tcb_t tcb0;
static eos_tcb_t tcb1;


static void print_numbers(void *arg)
{
    while (1) { }
}

static void print_alphabet(void *arg)
{
    while (1) { }
}

void eos_user_main()
{
    eos_create_task(&tcb0, (addr_t)stack0, STACK_SIZE, print_numbers, NULL, 0);
    eos_create_task(&tcb1, (addr_t)stack1, STACK_SIZE, print_alphabet, NULL, 0);
}
