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
  box(win, 0, 0); // 0, 0 -> default chars for vert and horz
  wattron(win, A_UNDERLINE | A_BOLD);
  mvwprintw(win, 1, 1, "Instruction:");
  wattroff(win, A_UNDERLINE | A_BOLD);
}

void update_InstrWin(UIState* ui, EmulatorState* state)
{
  mvwprintw(ui->winInstr, 1, 14, "%-6s", OpcodeS[state->op]);
  mvwprintw(ui->winInstr, 1, 21, "(%s)", ITypeS[state->itype]);
  mvwprintw(ui->winInstr, 2, 40 -10, "0x%-8X", state->instr);
  wmove(ui->winInstr, 3, 40 -32);
  for(int i = 31; i >= 0; i--)
  {
    if(ui->hasColor)
    {
      waddBit(ui->winInstr, state->instr, i); // replace with colored functions...
      /*
      //TODO: do later, with seperate functions
      switch (state->itype) {
        case R_Type:

          break;

      }
      */
    }else{
      waddBit(ui->winInstr, state->instr, i);
    }
  }
  wrefresh(ui->winInstr);
}

void init_PCWin(WINDOW* win)
{
  box(win, 0, 0); // 0, 0 -> default chars for vert and horz
  wattron(win, A_UNDERLINE | A_BOLD);
  mvwprintw(win, 1, 1, "Program Counter");
  wattroff(win, A_UNDERLINE | A_BOLD);
  wprintw(win, " - curr:");
  mvwprintw(win, 2, 1 +16, "- next:");
  mvwprintw(win, 3, 1 +16, "branch:");
  wrefresh(win);
}

void update_PCWin(UIState* ui, EmulatorState* state)
{
  if(ui->hasColor)
  {
    wattron(ui->winPC, A_BOLD | COLOR_PAIR(4));
    mvwprintw(ui->winPC, 1, 40 -12, "0x%-8X", state->pc);
    wattroff(ui->winPC, A_BOLD | COLOR_PAIR(4));
  }
  else
    mvwprintw(ui->winPC, 1, 40 -12, "0x%-8X", state->pc);

  mvwprintw(ui->winPC, 2, 40 -12, "0x%-8X", state->branch_target);
  if(state->branch)
    mvwprintw(ui->winPC, 3, 40 -12, "TRUE ");
  else
    mvwprintw(ui->winPC, 3, 40 -12, "FALSE");
  wrefresh(ui->winPC);
}

void init_RegWin(WINDOW* win)
{
  box(win, 0, 0); // 0, 0 -> default chars for vert and horz
  wattron(win, A_UNDERLINE | A_BOLD);
  mvwprintw(win, 1, 1, "Register File");
  wattroff(win, A_UNDERLINE | A_BOLD);
  for(int l = 0; l < 32; l++)
  {
    mvwprintw(win, 2 + l, 1, "%+2u", l);
    wattron(win, A_REVERSE);
    wprintw(win, "%+5s", ABINameS[l]);
    wattroff(win, A_REVERSE);
  }
}

void update_RegWin(UIState* ui, EmulatorState* state)
{
  for(int l = 0; l < 32; l++)
  {
    int colorPair = 0;
    if(l == state->rs1)
      colorPair = 1;
    else if(l == state->rs2)
      colorPair = 2;
    else if(l == state->rd)
      colorPair = 3;

    if(ui->hasColor && colorPair)
      wattron(ui->winRegs, A_BOLD | COLOR_PAIR(colorPair));

    switch(ui->regFileBase)
    {
      case BIN:
        wmove(ui->winRegs, 2 +l, 8);
        for(int b = 31; b >= 0; b--)
        {
          waddBit(ui->winRegs, reg_read(l), b);
        }
      break;

      case DEC:
        mvwprintw(ui->winRegs, 2 +l, 8, " %-31d", reg_read(l));
      break;

      case HEX:
        mvwprintw(ui->winRegs, 2 +l, 8, " 0x%-29X", reg_read(l));
      break;
    }

    wattroff(ui->winRegs, A_BOLD | COLOR_PAIR(colorPair));

  }
  wrefresh(ui->winRegs);
}

void init_AddrWin(WINDOW* win)
{
  box(win, 0, 0); // 0, 0 -> default chars for vert and horz
  mvwprintw(win, 1, 1, "rs1:");
  mvwprintw(win, 2, 1, "rd:");
  mvwprintw(win, 1, 13, "rs2:");
  //mvwprintw(win, 2, 33, "imm:");
  mvwprintw(win, 2, 25, "imm:");
  mvwprintw(win, 1, 80 -21, "Mem Addr:");
  mvwprintw(win, 2, 80 -21, "Mem Size:");
}

void update_AddrWin(UIState* ui, EmulatorState* state)
{
  // reg addresses
  if(ui->hasColor)
  {
    wattron(ui->winAddr, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(ui->winAddr, 1, 6, "%-5s", ABINameS[state->rs1]);
    wattroff(ui->winAddr, COLOR_PAIR(1) | A_BOLD);
  }
  else
  {
    mvwprintw(ui->winAddr, 1, 6, "%-5s", ABINameS[state->rs1]);
  }

  if(ui->hasColor)
  {
    wattron(ui->winAddr, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(ui->winAddr, 1, 18, "%-5s", ABINameS[state->rs2]);
    wattroff(ui->winAddr, COLOR_PAIR(2) | A_BOLD);
  }
  else
  {
    mvwprintw(ui->winAddr, 1, 18, "%-5s", ABINameS[state->rs2]);
  }
  if(state->write)
  {
    if(ui->hasColor)
    {
      wattron(ui->winAddr, COLOR_PAIR(3) | A_BOLD);
      mvwprintw(ui->winAddr, 2, 6, "%-5s", ABINameS[state->rd]);
      wattroff(ui->winAddr, COLOR_PAIR(3) | A_BOLD);
    }
    else
    {
      mvwprintw(ui->winAddr, 2, 6, "%-5s", ABINameS[state->rd]);
    }
  }
  else
    mvwprintw(ui->winAddr, 2, 6, "-----");

  // immediate
  //mvwprintw(ui->winAddr, 2, 38, "%-11d", state->imm);
  mvwprintw(ui->winAddr, 2, 30, "%-11d", state->imm);
  wprintw(ui->winAddr, " (0x%-8X)", state->imm);

  // memory addresses
  if(state->memory)
    if(ui->hasColor)
    {
      wattron(ui->winAddr, A_BOLD | COLOR_PAIR(5));
      mvwprintw(ui->winAddr, 1, 80 -11, "0x%-8X", state->mem_addr);
      wattroff(ui->winAddr, A_BOLD | COLOR_PAIR(5));
    }
    else
      mvwprintw(ui->winAddr, 1, 80 -11, "0x%-8X", state->mem_addr);
  else
    mvwprintw(ui->winAddr, 1, 80 -11, "----------");
  mvwprintw(ui->winAddr, 2, 80 -11, "0x%-8X", state->mem_size);

  wrefresh(ui->winAddr);
}

void init_MemWin(WINDOW* win)
{
  box(win, 0, 0); // 0, 0 -> default chars for vert and horz
  wattron(win, A_UNDERLINE | A_BOLD);
  mvwprintw(win, 1, 1, "Memory");
  wattroff(win, A_UNDERLINE | A_BOLD);
}

void update_MemWin(UIState* ui, EmulatorState* state)
{
  uint64_t n; // index into memory, requires extra bits to avoid overflow

  wattron(ui->winMem, A_UNDERLINE);
  mvwprintw(ui->winMem, 1, 40 -12, "0x%+8X", ui->memAddr);
  wattroff(ui->winMem, A_UNDERLINE);

  for(int y = 0; y < 32; y++)
  {
    wmove(ui->winMem, y +2, 1);
    for(int x = 0; x < 16; x++)
    {
      int colorPair = 0;
      if((x == 0) || (x == 8))
        wattron(ui->winMem, A_REVERSE);
      else if((x == 4) || (x == 12))
        wattroff(ui->winMem, A_REVERSE);

      n = ui->memAddr + 16*y + x; // index into memory

      if((n >= state->pc) && (n < state->pc +4))
        colorPair = 4;
      else if ((n >= state->mem_addr) && (n < state->mem_addr +4) && state->memory)
        colorPair = 5;

      if(n < (uint64_t) state->mem_size)
        if(ui->hasColor)
        {
          wattron(ui->winMem, A_BOLD | COLOR_PAIR(colorPair));
          wprintw(ui->winMem, "%+2X", state->mem[n]);
          wattroff(ui->winMem, A_BOLD | COLOR_PAIR(colorPair));
        }
        else
        {
          wprintw(ui->winMem, "%+2X", state->mem[n]);
        }
      else
        wprintw(ui->winMem, "--");
      wattroff(ui->winMem, A_BOLD);
    }
    wmove(ui->winMem, y+2, 40 -7);
    if(n < (uint64_t) state->mem_size)
    {
      wprintw(ui->winMem, "-");
      wattron(ui->winMem, A_UNDERLINE);
      wprintw(ui->winMem, "%+4X", (n & 0xFFFF));
    }
    else
    {
      wprintw(ui->winMem, "-");
      wattron(ui->winMem, A_UNDERLINE);
      wprintw(ui->winMem, "----");
    }
    wattroff(ui->winMem, A_UNDERLINE);
  }
  wrefresh(ui->winMem);
}

void change_MemAddr(UIState* ui)
{
  char inputStr[9];
  mvwprintw(ui->winMem, 1, 40 -12, "0x        ");
  wrefresh(ui->winMem);
  wmove(ui->winMem, 1, 40 -10);
  wattron(ui->winMem, A_UNDERLINE);
  echo();
  wgetnstr(ui->winMem, inputStr, 8);
  inputStr[8] = '\0'; // just in case
  noecho();
  wattroff(ui->winMem, A_UNDERLINE);
  char *endptr; // tmp, ignore
  ui->memAddr = strtol(inputStr, &endptr, 16);
}

void init_CtrlWin(WINDOW* win)
{
  box(win, 0, 0); // 0, 0 -> default chars for vert and horz
  wattron(win, A_REVERSE);
  mvwprintw(win, 1, 1, "F 1");
  mvwprintw(win, 2, 1, "F 2");
  mvwprintw(win, 1, 21, "F 3");
  mvwprintw(win, 2, 21, "F 9");
  mvwprintw(win, 1, 41, "F12");
  wattroff(win, A_REVERSE);
  mvwprintw(win, 1, 1 +4, "Exit");
  mvwprintw(win, 2, 1 +4, "Set Memory Addr");
  mvwprintw(win, 1, 21 +4, "Toggle Reg Base");
  mvwprintw(win, 2, 21 +4, "Step");
  mvwprintw(win, 1, 41 +4, "Reset");
}

void update_CtrlWin(UIState* ui, EmulatorState* state)
{
  wrefresh(ui->winCtrl);
}

WINDOW* createWin(int height, int width, int starty, int startx)
{
    WINDOW* local_win;

    local_win = newwin(height, width, starty, startx);
    //TODO: find out why this won't work if called by UIState contstructor
    //box(local_win, 0, 0); // 0, 0 -> default chars for vert and horz

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

void update_UI(UIState* ui, EmulatorState* state)
{
  update_PCWin(ui, state);
  update_RegWin(ui, state);
  update_InstrWin(ui, state);
  update_AddrWin(ui, state);
  update_MemWin(ui, state);
  update_CtrlWin(ui, state);
}

void ui_exit(UIState* ui)
{
  destroyWin(ui->winInstr);
  destroyWin(ui->winPC);
  destroyWin(ui->winRegs);
  destroyWin(ui->winMem);
  destroyWin(ui->winAddr);
  endwin();
  free(ui);
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

UIState* initUI()
{
  UIState *ui = (UIState*) malloc(sizeof(UIState));

  if(ui != NULL)
  {
    ui->winInstr = createWin(5, 41, 0, 0);
    ui->winPC = createWin(5, 39, 0, 41);
    ui->winAddr = createWin(4, 80, 5, 0);
    ui->winRegs = createWin(35, 41, 9, 0);
    ui->winMem = createWin(35, 39, 9, 41);
    ui->winCtrl = createWin(4, 80, 44, 0);

    ui->regFileBase = HEX;
    ui->memAddr = 0;
    ui->hasColor = false;
  }
  return ui;
}
