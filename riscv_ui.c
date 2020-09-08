#include "riscv_emu.h"
#include "riscv_ui.h"
#include "riscv_regfile.h"

void mvwprintwBits(WINDOW* win, int y, int x, size_t const size, void const* ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    wmove(win, y, x);

    for(i = size-1; i >= 0; i--)
    {
        for(j = 7; j >= 0; j--)
        {
            byte = (b[i] >> j) & 1;
            waddch(win, byte + '0');    //asking for trouble... works for ASCII only
        }
    }
}

void waddBit(WINDOW* win, uint32_t data, unsigned int pos)
{
    uint32_t tmp = (data >> pos) & 1;
    waddch(win, tmp + '0');   //again, dangerous hack
}

void init_InstrWin(WINDOW* win)
{
  mvwprintw(win, 1, 1, "Instruction:");
  wrefresh(win);
}

void update_InstrWin(WINDOW* win, EmulatorState* state, bool color)
{
  mvwprintw(win, 1, 14, "%-6s", OpcodeS[state->op]);
  mvwprintw(win, 1, 40 -8, "(%s)", ITypeS[state->itype]);
  mvwprintw(win, 2, 40 -10, "0x%+8X", state->instr);
  wmove(win, 3, 40 -32);
  for(int i = 31; i >= 0; i--)
  {
    if(color)
    {
      /*
      //TODO: do later, with seperate functions
      switch (state->itype) {
        case R_Type:

          break;

      }
      */
    }else{
      waddBit(win, state->instr, i);
    }
  }
  wrefresh(win);
}

void init_PCWin(WINDOW* win)
{
  mvwprintw(win, 1, 1, "Program Counter - curr:");
  mvwprintw(win, 2, 1 +16, "- next:");
  mvwprintw(win, 3, 1 +16, "branch:");
  wrefresh(win);
}

void update_PCWin(WINDOW* win, EmulatorState* state, bool color)
{
  mvwprintw(win, 1, 40 -12, "0x%-8X", state->pc);
  mvwprintw(win, 2, 40 -12, "0x%-8X", state->branch_target);
  if(state->branch)
    mvwprintw(win, 3, 40 -12, "TRUE ");
  else
    mvwprintw(win, 3, 40 -12, "FALSE");
  wrefresh(win);
}

void init_RegWin(WINDOW* win)
{
  mvwprintw(win, 1, 1, "Register File");
  for(int l = 0; l < 32; l++)
  {
    mvwprintw(win, 2 + l, 1, "%+2u", l);
    wattron(win, A_REVERSE);
    wprintw(win, "%+5s", ABINameS[l]);
    wattroff(win, A_REVERSE);
  }
}

void update_RegWin(WINDOW* win, EmulatorState* state, bool color)
{
  for(int l = 0; l < 32; l++)
  {
    wmove(win, 2 +l, 8);
    for(int b = 31; b >= 0; b--)
    {
      waddBit(win, reg_read(l), b);
    }
  }
  wrefresh(win);
}

void init_AddrWin(WINDOW* win)
{
  mvwprintw(win, 1, 1, "rs1:");
  mvwprintw(win, 2, 1, "rd:");
  mvwprintw(win, 1, 13, "rs2:");
  mvwprintw(win, 2, 33, "imm:");
  mvwprintw(win, 1, 80 -21, "Mem Addr:");
  mvwprintw(win, 2, 80 -21, "Mem Size:");
}

void update_AddrWin(WINDOW* win, EmulatorState* state, bool color)
{
  // reg addresses
  mvwprintw(win, 1, 6, "%-5s", ABINameS[state->rs1]);
  mvwprintw(win, 1, 18, "%-5s", ABINameS[state->rs2]);
  if(state->write)
    mvwprintw(win, 2, 6, "%-5s", ABINameS[state->rd]);
  else
    mvwprintw(win, 2, 6, "-----");

  // immediate
  mvwprintw(win, 2, 38, "%-11d", state->imm);

  // memory addresses
  if(state->memory)
    mvwprintw(win, 1, 80 -11, "0x%-8X", state->mem_addr);
  else
    mvwprintw(win, 1, 80 -11, "----------");
  mvwprintw(win, 2, 80 -11, "0x%-8X", state->mem_size);

  wrefresh(win);
}

void update_MemWin(WINDOW* win, EmulatorState* state, bool color, uint32_t startAddr)
{
  wrefresh(win);
}

WINDOW* createWin(int height, int width, int starty, int startx)
{
    WINDOW* local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0); // 0, 0 -> default chars for vert and horz

    touchwin(local_win);
    wrefresh(local_win);

    return local_win;
}

void destroyWin(WINDOW* local_win)
{
    wborder(local_win, ' ',' ',' ',' ',' ',' ',' ',' ');
    /*ls, rs, ts, bs - left, right, top, bottom side
     *tl, tr, bl, br - top and bottom left and right
     */
    wrefresh(local_win);
    delwin(local_win);
}

const char* OpcodeS[] = {
    [LUI]   = "LUI",
    [AUIPC] = "AUIPC",
    [JAL]   = "JAL",
    [JALR]  = "JALR",
    [BEQ]   = "BEQ",
    [BNE]   = "BNE",
    [BLT]   = "BLT",
    [BGE]	= "BGE",
    [BLTU]	= "BLTU",
    [BGEU]	= "BGEU",
    [LB]	= "LB",
    [LH]	= "LH",
    [LW]	= "LW",
    [LBU]	= "LBU",
    [LHU]	= "LHU",
    [SB]	= "SB",
    [SH]	= "SH",
    [SW]	= "SW",
    [ADDI]	= "ADDI",
    [SLTI]	= "SLTI",
    [SLTIU]	= "SLTIU",
    [XORI]	= "XORI",
    [ORI]	= "ORI",
    [ANDI]	= "ANDI",
    [SLLI]	= "SLLI",
    [SRLI]	= "SRLI",
    [SRAI]	= "SRAI",
    [ADD]	= "ADD",
    [SUB]	= "SUB",
    [SLL]	= "SLL",
    [SLT]	= "SLT",
    [SLTU]	= "SLTU",
    [XOR]	= "XOR",
    [SRL]	= "SRL",
    [SRA]	= "SRA",
    [OR]	= "OR",
    [AND]	= "AND",
    [FENCE]	= "FENCE",
    [ECALL]	= "ECALL",
    [EBREAK]= "EBREAK"
};

const char* ITypeS[] = {
    [R_Type] = "R-Type",
    [I_Type] = "I-Type",
    [S_Type] = "S-Type",
    [B_Type] = "B-Type",
    [U_Type] = "U-Type",
    [J_Type] = "J-Type"
};

const char* ABINameS[] = {
    [0] = "zero",
    [1] = "ra",
    [2] = "sp",
    [3] = "gp",
    [4] = "tp",
    [5] = "t0",
    [6] = "t1",
    [7] = "t2",
    [8] = "s0/fp",
    [9] = "s1",
    [10]= "a0",
    [11]= "a1",
    [12]= "a2",
    [13]= "a3",
    [14]= "a4",
    [15]= "a5",
    [16]= "a6",
    [17]= "a7",
    [18]= "s2",
    [19]= "s3",
    [20]= "s4",
    [21]= "s5",
    [22]= "s6",
    [23]= "s7",
    [24]= "s8",
    [25]= "s9",
    [26]= "s10",
    [27]= "s11",
    [28]= "t3",
    [29]= "t4",
    [30]= "t5",
    [31]= "t6"
};
