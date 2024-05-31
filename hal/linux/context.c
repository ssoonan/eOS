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
typedef struct _os_context {
    // To be filled by students: Project 2
    // Emulate the stack of the interrupted task right after pushal instruciton
    /* low address */
    /* high address */	
} _os_context_t;


void print_context(addr_t context)
{
    if(context == NULL)
        return;

    _os_context_t *ctx = (_os_context_t *)context;

    // To be filled by students if needed: Project 2
 
    //PRINT("reg1  =0x%x\n", ctx->reg1);
    //PRINT("reg2  =0x%x\n", ctx->reg2);
    //PRINT("reg3  =0x%x\n", ctx->reg3);
    //...
}


addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg)
{
    // To be filled by students: Project 2
}


void _os_restore_context(addr_t sp)
{
    // To be filled by students: Project 2
}


addr_t _os_save_context()
{
    // To be filled by students: Project 2
}
