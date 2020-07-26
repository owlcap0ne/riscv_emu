#include "riscv_emu.h"

void fetch(EmulatorState* state)
{
    state->instr = *((int32_t*) (state->mem + state->pc));
    return;
}
