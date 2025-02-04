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

eos_semaphore_t test_sem;

void sender_task(void *arg)
{
    while (1)
    {
        // 세마포어 획득 시도 (타임아웃 -1 => 바로 실패, 0 => 바로 획득 불가 시 무한 대기, 그 외 => 지정 시간 대기)
        // 여기서는 무한정 대기 예시로 0을 사용해봄
        if (eos_acquire_semaphore(&test_sem, 0))
        {
            // 세마포어를 획득했으므로 임계 구역에 들어갈 수 있음
            // 여기에 공유 자원에 대한 작업을 수행
            // 예시로 메시지를 출력
            PRINT("Sender Task: Semaphore acquired! Doing work...\n");

            // 잠깐 딜레이(의미 있는 작업 대신 예시로 sleep)
            eos_sleep(2);

            // 작업 후 세마포어 해제
            eos_release_semaphore(&test_sem);
            PRINT("Sender Task: Semaphore released!\n");
        }
        else
        {
            // 세마포어를 획득하지 못한 경우
            PRINT("Sender Task: Failed to acquire semaphore.\n");
        }

        // 다음 작업 전 잠깐 대기 (타임 슬라이스 양보)
        eos_sleep(4);
    }
}

void receiver_task(void *arg)
{
    while (1)
    {
        // 무한 대기(블로킹 모드)로 세마포어 획득 시도
        // (timeout=-1은 acquire에서 바로 실패 조건이므로, 여기서는 0을 예시로 사용)
        if (eos_acquire_semaphore(&test_sem, 0))
        {
            PRINT("Receiver Task: Semaphore acquired! Doing work...\n");

            // 잠깐 딜레이
            eos_sleep(2);

            eos_release_semaphore(&test_sem);
            PRINT("Receiver Task: Semaphore released!\n");
        }
        else
        {
            PRINT("Receiver Task: Failed to acquire semaphore.\n");
        }

        // 다음 작업 전 잠깐 대기
        eos_sleep(4);
    }
}

void observer_task(void *arg)
{
    while (1)
    {
        // 이 태스크는 주기적으로 세마포어를 얻어 어떤 상태를 체크하는 시나리오
        if (eos_acquire_semaphore(&test_sem, 0))
        {
            PRINT("Observer Task: Checking shared resource...\n");

            // 잠깐 딜레이
            eos_sleep(1);

            eos_release_semaphore(&test_sem);
            PRINT("Observer Task: Done checking. Semaphore released.\n");
        }
        else
        {
            PRINT("Observer Task: Could not acquire semaphore.\n");
        }

        // 5틱(또는 5초)마다 상태 확인
        eos_sleep(5);
    }
}

static void sender_task1(void *arg)
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
    // eos_init_semaphore(&test_sem, 1, 0);
    // eos_create_task(&tcb0, (addr_t)stack0, 8096, sender_task, NULL, 50);
    // eos_create_task(&tcb1, (addr_t)stack1, 8096, receiver_task, NULL, 50);
    // eos_create_task(&tcb2, (addr_t)stack2, 8096, observer_task, NULL, 50);

    eos_create_task(&tcb0, (addr_t)stack0, 8096, sender_task1, NULL, 50);
    eos_create_task(&tcb1, (addr_t)stack1, 8096, receiver_task1, NULL, 10);
    eos_create_task(&tcb2, (addr_t)stack2, 8096, receiver_task0, NULL, 10);

    eos_set_period(&tcb0, 2);
    eos_set_period(&tcb1, 4);
    eos_set_period(&tcb2, 5);

    eos_init_mqueue(&mq0, queue0, 5, 2, FIFO);
    eos_init_mqueue(&mq1, queue1, 5, 2, FIFO);
}