#include "riscv_emu.h"
#include "riscv_ui.h"
#include "riscv_regfile.c"

void init_InstrWin(WINDOW* win)
{
  mvwprintw(win, 1, 1, "Instruction:")
}

void update_InstrWin(WINDOW* win, EmulatorState* state, bool color)
{
  mvwprintw(win, 1, 14, "%s", OpcodeS[state->op]);
  mvwprintw(win, 1, 40 -8, "(%s)", ABINameS[state->itype]);
  mvwprintw(win, 2, 40 -10, "0x%+8X", state->instr);
  wmove(win, 3, 40 -32);
  for(int i = 31; i >= 0; i--)
  {
    if(color)
    {
      switch (state->itype) {
        case R_Type:

          break;
        
      }
    }else{
      waddBit(win, state->instr, i);
    }
  }
}
