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

  // TODO: move all of that UI state stuff to a struct

  // for now only the reg file base can be switched
  enum UIBase regFileBase = HEX;
  uint32_t memAddr = 0;
  bool hasColor = false;

  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  int tmp = curs_set(0);

  if(has_colors())
  {
    start_color();
    hasColor = true;
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);
    init_pair(3, COLOR_WHITE, COLOR_BLUE);
    init_pair(4, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(5, COLOR_BLACK, COLOR_CYAN);
  }

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
  char inputStr[9];

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

    update_PCWin(winPC, state, hasColor);
    update_RegWin(winRegs, state, hasColor, regFileBase);
    update_InstrWin(winInstr, state, hasColor);
    update_AddrWin(winAddr, state, hasColor);
    update_MemWin(winMem, state, hasColor, memAddr);

    while(1)
    {
      input = getch();
      if(input == KEY_F(9))
      {
        break;
      }
      else if(input == KEY_F(1))
        break;
      else if(input == KEY_F(3))
      {
        regFileBase = (regFileBase +1) %3;
        update_RegWin(winRegs, state, hasColor, regFileBase);
      }
      else if(input == KEY_F(2))
      {
        mvwprintw(winMem, 1, 40 -12, "0x        ");
        wrefresh(winMem);
        wmove(winMem, 1, 40 -10);
        wattron(winMem, A_UNDERLINE);
        echo();
        wgetnstr(winMem, inputStr, 8);
        inputStr[8] = '\0'; // just in case
        noecho();
        wattroff(winMem, A_UNDERLINE);
        char *endptr; // tmp, ignore
        memAddr = strtol(inputStr, &endptr, 16);
        update_MemWin(winMem, state, hasColor, memAddr);
      }
    }

    // scond check for exit, just slightly better than GOTO programming...
    if(input == KEY_F(1))
      break;

    // two options: just draw as fast as possible, or only proceed on F9...
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
