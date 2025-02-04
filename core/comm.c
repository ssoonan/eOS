/********************************************************
 * Filename: core/comm.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/30/24
 *
 * Description: Message queue management
 ********************************************************/

#include <core/eos.h>

void eos_init_mqueue(eos_mqueue_t *mq, void *queue_start, int16u_t queue_size, int8u_t msg_size, int8u_t queue_type)
{
    eos_semaphore_t *putsem = (eos_semaphore_t *)malloc(sizeof(eos_semaphore_t));
    eos_semaphore_t *getsem = (eos_semaphore_t *)malloc(sizeof(eos_semaphore_t));
    eos_init_semaphore(putsem, queue_size, queue_type);
    eos_init_semaphore(getsem, 0, queue_type);

    mq->queue_start = queue_start;
    mq->queue_size = queue_size;
    mq->msg_size = msg_size;
    mq->queue_type = queue_type;
    mq->putsem = putsem;
    mq->getsem = getsem;
    mq->front = queue_start;
    mq->rear = queue_start;
}

int8u_t eos_send_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    int8u_t putsem_aquired = eos_acquire_semaphore(mq->putsem, timeout);
    if (!putsem_aquired)
        return;

    for (int32u_t i = 0; i < (mq->msg_size); i++)
    {
        *(int32u_t *)mq->rear = ((int32u_t *)message)[i];
        if (mq->rear == (int32u_t *)mq->queue_start + mq->queue_size * mq->msg_size) // 여기는 순전히 주소 계산으로
            mq->rear = mq->queue_start;
        else
            mq->rear++;
    }
    eos_release_semaphore(mq->getsem);
}

int8u_t eos_receive_message(eos_mqueue_t *mq, void *message, int32s_t timeout)
{
    int8u_t getsem_aquired = eos_acquire_semaphore(mq->getsem, timeout);
    if (!getsem_aquired)
        return;

    int32u_t *msge = (int32u_t *)message;

    for (int i = 0; i < mq->msg_size; i++)
    {
        // get the message from the front part of queue
        msge[i] = *((int32u_t *)(mq->front));

        // if 'front' points the end of the queue, make 'front' point the start of the queue
        // if not, make 'rear' point the next entry of the queue
        if (mq->front == (int32u_t *)mq->queue_start + mq->queue_size * mq->msg_size)
            mq->front = mq->queue_start;
        else
            mq->front++;
    }
    // release semaphore
    eos_release_semaphore(&mq->putsem);
}
