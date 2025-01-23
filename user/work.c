#include <core/eos.h>
#define STACK_SIZE 8096

int32u_t stack0[STACK_SIZE];
int32u_t stack1[STACK_SIZE];
int32u_t stack2[STACK_SIZE];
eos_tcb_t tcb0;
eos_tcb_t tcb1;
eos_tcb_t tcb2;
static int32u_t queue0[10];
static int32u_t queue1[10];
eos_mqueue_t mq0;
eos_mqueue_t mq1;

static void sender_task(void *arg)
{
    int32u_t *data = "xy";
    while (1)
    {
        PRINT("send message to mq0\n");
        eos_send_message(&mq0, data, 0);
        PRINT("send message to mq1\n");
        eos_send_message(&mq1, data, 0);
        eos_sleep(0);
    }
}
static void receiver_task1(void *arg)
{
    int32u_t data[2];
    while (1)
    {
        PRINT("receive message from mq1\n");
        eos_receive_message(&mq1, data, 0);
        PRINT("received message: %s\n", data);
        eos_sleep(0);
    }
}
static void receiver_task0(void *arg)
{
    int32u_t data[2];
    while (1)
    {
        PRINT("receive message from mq0\n");
        eos_receive_message(&mq0, data, 0);
        PRINT("received message: %s\n", data);
        eos_sleep(0);
    }
}

void eos_user_main()
{
    eos_create_task(&tcb0, (addr_t)stack0, 8096, sender_task, NULL, 50);
    eos_create_task(&tcb1, (addr_t)stack1, 8096, receiver_task0, NULL, 10);
    eos_create_task(&tcb2, (addr_t)stack2, 8096, receiver_task1, NULL, 20);
    eos_set_period(&tcb0, 2);
    eos_set_period(&tcb1, 4);
    eos_set_period(&tcb2, 6);
    eos_init_mqueue(&mq0, queue0, 5, 2, 1);
    eos_init_mqueue(&mq1, queue1, 5, 2, 1);
}