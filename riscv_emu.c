#include "riscv_emu.h"
#include "riscv_ui.h"
#include "riscv_hexread.h"
#include "riscv_fetch.h"
#include "riscv_decode.h"
#include "riscv_regfile.h"
#include "riscv_execute.h"
#include "riscv_memory.h"
#include <string.h>
#include <stdio.h>

EmulatorState* initState()
{
    EmulatorState *state = (EmulatorState*)malloc(sizeof(EmulatorState));

    if(state != NULL)
    {
        state->pc = 0;
        state->branch_target = 0;
        state->branch = false;
        state->instr = 0x13;    //ADDI 0, 0, 0 - NOP
        state->op = ADDI;
        state->itype = I_Type;
        state->imm = 0;
        state->rs1 = 0;
        state->rs2 = 0;
        state->rd = 0;
        state->rs1_dat = 0;
        state->rs2_dat = 0;
        state->rd_dat = 0;
        state->mem_addr = 0;
        state->write = false;
        state->memory = false;
        state->mem = NULL;
        state->mem_size = 0;
    }
    return state;
}

void resetState(EmulatorState* state)
{
  if(state != NULL)
    {
        state->pc = 0;
        state->branch_target = 0;
        state->branch = false;
        state->instr = 0x13;    //ADDI 0, 0, 0 - NOP
        state->op = ADDI;
        state->itype = I_Type;
        state->imm = 0;
        state->rs1 = 0;
        state->rs2 = 0;
        state->rd = 0;
        state->rs1_dat = 0;
        state->rs2_dat = 0;
        state->rd_dat = 0;
        state->mem_addr = 0;
        state->write = false;
        state->memory = false;
    }
}

int reset(EmulatorState* state, const char* hexfile)
{
  reg_reset();
  memory_zero(state);
  state->pc = 0; // in case the Hexfile doesn't modify it
  //not strictly neccessary, but otherwise the UI shows outdated info
  state->rs1 = 0;
  state->rs2 = 0;
  state->rd = 0;
  state->write = false;
  state->branch = false;
  state->memory = false;
  if(hexread(state, hexfile))
  {
    fprintf(stderr, "Something went wrong with the Hexfile\n");
    return -1;
  }

  return 0;
}

void emu_cycle(EmulatorState* state)
{
  fetch(state);
  decode(state);

  state->rs1_dat = reg_read(state->rs1);
  state->rs2_dat = reg_read(state->rs2);

  execute(state);

  if(state->memory)
    if(memory(state))
    {
      fprintf(stderr, "Memory Error @ %x, Op = %s, PC = %x\n", state->mem_addr,
              OpcodeS[state->op], state->pc);
    }
  if(state->write)
    reg_write(state->rd, state->rd_dat);
}

void emu_exit(EmulatorState* state)
{
  if(state->mem)
    free(state->mem);
  if(state)
    free(state);
}
