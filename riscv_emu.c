#include "riscv_emu.h"
#include "riscv_ui.h"
#include "riscv_hexread.h"
#include "riscv_fetch.h"
#include "riscv_decode.h"
#include "riscv_pc.h"
#include "riscv_regfile.h"
#include "riscv_execute.h"
#include "riscv_memory.h"
#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
  EmulatorState *state = initState();
  //int mem_size = 1024;
  //int mem_size = 0x10000; //fixed 64k for now
  int mem_size = 0x20000FF; // required for memtest.hex?
  state->mem = (uint8_t*) malloc(sizeof(uint8_t)*mem_size);
  if(state->mem == NULL)
    return -1;
  state->mem_size = mem_size;
  if(hexread(state, argv[1]) < 0)
    return -1;

  WINDOW* winInstr;
  WINDOW* winPC;
  WINDOW* winAddr;
  WINDOW* winRegs;
  WINDOW* winMem;

  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  refresh();

  winInstr = createWin(5, 41, 0, 0);
  winPC = createWin(5, 39, 0, 41);
  winAddr = createWin(4, 80, 5, 0);
  winRegs = createWin(35, 41, 9, 0);
  winMem = createWin(35, 39, 9, 41);

  init_InstrWin(winInstr);
  init_PCWin(winPC);
  init_RegWin(winRegs);
  init_PCWin(winPC);
  init_AddrWin(winAddr);
  init_MemWin(winMem);

  wrefresh(winMem);

  mvaddstr(50, 0, "init done");

  int input;

  while(1)
  {
    fetch(state);
    decode(state);
    //don't bother with reads from 'zero'
    if(state->rs1)
      state->rs1_dat = reg_read(state->rs1);
    else
      state->rs1_dat = 0;
    if(state->rs2)
      state->rs2_dat = reg_read(state->rs2);
    else
      state->rs2_dat = 0;
    
    execute(state);
    if(state->memory)
      memory(state);
    if(state->write)
      reg_write(state->rd, state->rd_dat);

    update_PCWin(winPC, state, false);
    update_RegWin(winRegs, state, false);
    update_InstrWin(winInstr, state, false);
    update_AddrWin(winAddr, state, false);
    update_MemWin(winMem, state, false, 0x0000);

    while(1)
    {
      input = getch();
      if(input == KEY_F(9))
      {
        break;
      }
      else if(input == KEY_F(1))
        break;
    }

    if(input == KEY_F(1))
      break;

    pc_inc(state);

    input = 0;
  }

  destroyWin(winInstr);
  destroyWin(winPC);
  destroyWin(winRegs);
  destroyWin(winMem);
  destroyWin(winAddr);
  endwin();

  free(state->mem);
  free(state);
  return 0;
}

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
