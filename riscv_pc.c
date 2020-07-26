#include "riscv_emu.h"

void pc_inc(EmulatorState* state)
{
    if(state->branch)
        state->pc = state->branch_target;
    else
        state->pc += 4;
    return;
}
