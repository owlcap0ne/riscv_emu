#include "riscv_emu.h"
#include "riscv_hexread.c"
#include "riscv_fetch.c"
#include "riscv_decode.c"
#include "riscv_pc.c"
#include "riscv_regfile.c"
#include "riscv_execute.c"
#include "riscv_memory.c"
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  EmulatorState *state = initState();
  //int mem_size = 1024;
  int mem_size = 0x10000; //fixed 64k for now
  state->mem = (uint8_t*) malloc(sizeof(uint8_t)*mem_size);
  if(state->mem == NULL)
    return -1;
  state->mem_size = mem_size;
  if(hexread(state, argv[1]) < 0)
    return -1;
  for(int i = 0; i < 255; i++)
  {
    fetch(state);
    decode(state);
    //don't bother with reads from 'zero'
    if(state->rs1)
      state->rs1_dat = reg_read(state->rs1);
    if(state->rs2)
      state->rs2_dat = reg_read(state->rs2);
    execute(state);
    if(state->memory)
      memory(state);
    if(state->write)
      reg_write(state->rd, state->rd_dat);
    pc_inc(state);
  }
  free(state->mem);
  free(state);
  return 0;
}
