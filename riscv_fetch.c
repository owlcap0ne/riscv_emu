#include "riscv_emu.h"

void fetch(EmulatorState* state)
{
    if(state->pc + sizeof(int32_t) > state->mem_size)
    {
        printf("ERROR: invalid instruction fetch @ %x\n", state->pc);
        return;
    }
    state->instr = *((int32_t*) (state->mem + state->pc));
    return;
}
