/********************************************************
 * Filename: hal/linux/context.c
 *
 * Author: Jiyong Park, RTOSLab. SNU
 * Modified by: Seongsoo Hong on 03/31/24
 *
 * Description: Context management (define, create, switch)
 ********************************************************/

#include <core/eos.h>
#include "emulator_asm.h"

//
typedef struct _os_context
{
    // To be filled by students: Project 2
    // Emulate the stack of the interrupted task right after pushal instruciton
    // cpu register들을 여기에 어떻게 정의한다는 거지? 여튼 이걸 정의하면 이 구조체를 써서 여러 함수에서 쓴다는 건 알겠는데..
    // 거꾸로 추상화라는 게 무엇인가를 생각하자. 그냥 변수명으로 int값만 있으면 다 가져올 수 있는 게 맞음
    // 그러면 그냥 어떤 레지스터가 있는지?만 다 정의하면 되나? eflag라는 custom 변수에 더하여..
    // + 이 register에 들어가는 건 주소가 아니라 값이니까 그냥 sign이어도 될 듯
    int32s_t eip;
    int32s_t _eflags;
    int32s_t eax;
    int32s_t ecx;
    int32s_t edx;
    int32s_t ebx;
    int32s_t esp;
    int32s_t ebp;
    int32s_t esi;
    int32s_t edi;
} _os_context_t;

void print_context(addr_t context)
{
    if (context == NULL)
        return;

    _os_context_t *ctx = (_os_context_t *)context;
    PRINT("eip  =0x%x\n", ctx->eip);
    PRINT("_eflags  =0x%x\n", ctx->_eflags);
    PRINT("eax  =0x%x\n", ctx->eax);
    PRINT("ecx  =0x%x\n", ctx->ecx);
    PRINT("edx  =0x%x\n", ctx->edx);
    PRINT("ebx  =0x%x\n", ctx->ebx);
    PRINT("esp  =0x%x\n", ctx->esp);
    PRINT("ebp  =0x%x\n", ctx->ebp);
    PRINT("esi  =0x%x\n", ctx->esi);
    PRINT("edi  =0x%x\n", ctx->edi);
}

addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg)
{
    int32u_t *stack_base_address = (int32u_t *)stack_base;
    int32u_t *stack_bottom = (int32u_t *)(stack_base_address + stack_size);
    *stack_bottom = arg;
    *(stack_bottom + 4) = NULL;
    *(stack_bottom + 8) = entry;
    *(stack_bottom + 12) = 1;
    *(stack_bottom + 16) = NULL;
    *(stack_bottom + 20) = NULL;
    *(stack_bottom + 24) = NULL;
    *(stack_bottom + 28) = NULL;
    *(stack_bottom + 32) = NULL;
    *(stack_bottom + 36) = NULL;
    *(stack_bottom + 40) = NULL;
}

void _os_restore_context(addr_t sp)
{
    __asm__ __volatile__(
        "movl %0, %%esp \n\t"
        "popa \n\t"
        "popf \n\t"
        "ret \n\t"
        :
        : "m"(sp));
}

// addr_t _os_save_context()
// {
//     addr_t sp_address;
//     __asm__ __volatile__(
//         "push %%ebp \n\t"
//         "movl %%esp, %%ebp \n\t" // prologue
//         // "call 1f \n\t" // Call the label 1, this pushes the return address (current %eip) onto the stack
//         // "1: pop %%eax \n\t"
//         // "push %%eax \n\t" // Push the EIP value onto the stack
//         "pushf \n\t"
//         "movl $0, %%eax \n\t"
//         "pusha \n\t"
//         "movl %%esp, %0 \n\t"
//         "push 4(%%ebp) \n\t"
//         "push %%ebp \n\t"
//         "leave \n\t"
//         "ret \n\t"
//         : "=m"(sp_address));
//     PRINT("asd: \n");
//     return sp_address;
// }

addr_t _os_save_context()
{
    // eax에 esp를 넣는 부분이, return 값을 넣어주는 부분이다.
    __asm__ __volatile__("\
		pushl $resume_eip;\
		pushl _eflags;\
		pushl %%eax;\
		pushl %%ecx;\
		pushl %%edx;\
		pushl %%ebx;\
		pushl %%esi;\
		pushl %%edi;\
		movl %%esp, %%eax;\
		pushl 4(%%ebp);\
		pushl 0(%%ebp);\
		movl %%esp, %%ebp;\
		resume_eip: \
		leave;\
		ret;\
	" ::);
}