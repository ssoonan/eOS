/********************************************************
 * Filename: core/comm.c
 *
 * Author: jtlim, RTOSLab. SNU.
 *
 * Description: message queue management.
 ********************************************************/

#include <core/eos.h>

void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type)
{
    mq->queue_size = queue_size;
    mq->msg_size = msg_size;
    mq->queue_start = queue_start;
    mq->front = queue_start;
    mq->rear = queue_start;
    mq->queue_type = queue_type;
    // 애초에 세마포어 초기화가 없기에 포인터로 선언했으면 여기서 새로 선언 필요
    mq->putsem = (eos_semaphore_t *)malloc(sizeof(eos_semaphore_t));
    mq->getsem = (eos_semaphore_t *)malloc(sizeof(eos_semaphore_t));

    eos_init_semaphore(mq->putsem, queue_size, queue_type);
    eos_init_semaphore(mq->getsem, 0, queue_type);
}

int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    // put semaphore 획득 (mq->putsem가 이미 할당되어 있다고 가정)
    if (eos_acquire_semaphore(mq->putsem, timeout) == 0)
        return 0;

    int8u_t *msge = (int8u_t *)message;
    for (int i = 0; i < mq->msg_size; i++)
    {
        // mq->rear를 int8u_t 포인터로 변환하여 큐의 끝에 도달했는지 검사
        if ((int8u_t *)mq->rear == (int8u_t *)mq->queue_start + mq->queue_size * mq->msg_size)
            mq->rear = mq->queue_start; // 큐의 시작으로 순환

        // 현재 위치에 한 바이트 기록
        *((int8u_t *)mq->rear) = msge[i];

        // 포인터를 다음 바이트로 이동 (void 포인터 산술 대신 int8u_t 포인터 연산)
        mq->rear = (void *)((int8u_t *)mq->rear + 1);
    }
    // get semaphore 해제
    eos_release_semaphore(mq->getsem);
    return 1;
}

int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    // get semaphore 획득
    if (eos_acquire_semaphore(mq->getsem, timeout) == 0)
        return 0;

    int8u_t *msge = (int8u_t *)message;
    for (int i = 0; i < mq->msg_size; i++)
    {
        // mq->front가 큐의 끝에 도달했는지 검사
        if ((int8u_t *)mq->front == (int8u_t *)mq->queue_start + mq->queue_size * mq->msg_size)
            mq->front = mq->queue_start; // 큐의 시작으로 순환

        // 현재 위치의 데이터를 한 바이트 읽어옴
        msge[i] = *((int8u_t *)mq->front);

        // 포인터를 다음 바이트로 이동
        mq->front = (void *)((int8u_t *)mq->front + 1);
    }
    // put semaphore 해제
    eos_release_semaphore(mq->putsem);
    return 1;
}