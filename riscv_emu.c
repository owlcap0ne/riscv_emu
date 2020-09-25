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
  if(state == NULL)
  {
    fprintf(stderr, "Something went wrong: Can't allocate EmulatorState");
    return -1;
  }

  int mem_size = 0x10000; //fixed 64k for now
  //int mem_size = 0x20000FF; // required for memtest.hex?
  state->mem = (uint8_t*) malloc(sizeof(uint8_t)*mem_size);
  if(state->mem == NULL)
  {
    fprintf(stderr, "Something went wrong: Can't allocate Emulator Memory");
    return -1;
  }

  state->mem_size = mem_size;
  if(hexread(state, argv[1]) < 0)
  {
    fprintf(stderr, "Something went wrong with the Hexfile");
    return -1;
  }

  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  int tmp = curs_set(0);

  // Windows can only be created after initscr()...
  // DON'T MOVE THIS BLOCK
  UIState *ui = initUI();
  if(ui == NULL)
  {
    fprintf(stderr, "Something went wrong: Can't allocate UIState");
    return -1;
  }

  if(has_colors())
  {
    start_color();
    ui->hasColor = true;
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);
    init_pair(3, COLOR_WHITE, COLOR_BLUE);
    init_pair(4, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(5, COLOR_WHITE, COLOR_CYAN);
    init_pair(6, COLOR_WHITE, COLOR_YELLOW);
  }

  refresh();

  init_InstrWin(ui->winInstr);
  init_PCWin(ui->winPC);
  init_RegWin(ui->winRegs);
  init_PCWin(ui->winPC);
  init_AddrWin(ui->winAddr);
  init_MemWin(ui->winMem);
  init_CtrlWin(ui->winCtrl);

  // only needed for the UI...
  emu_cycle(state);

  update_UI(ui, state);

  pc_inc(state);

  int input;

  while(1)
  {
    input = getch();
    if(input == KEY_F(9))
    {
      emu_cycle(state);
      update_UI(ui, state);
      pc_inc(state);
    }
    else if(input == KEY_F(1))
    {
      ui_exit(ui);
      emu_exit(state);
      break;
    }
    else if(input == KEY_F(3))
    {
      ui->regFileBase = (ui->regFileBase +1) %3;
      update_RegWin(ui, state);
    }
    else if(input == KEY_F(2))
    {
      change_MemAddr(ui);
      update_MemWin(ui, state);
    }
    else if(input == KEY_F(12))
    {
      if(reset(state, argv[1]))
      {
        ui_exit(ui);
        emu_exit(state);
        return -1;
      }
      else
      {
        emu_cycle(state);
        update_UI(ui, state);
        pc_inc(state);
      }
    }
    else if(input == KEY_F(5))
    {
      if(memory_dump(state, "memdump.hex"))
        fprintf(stderr, "Error dumping memory");
    }

    input = 0;
  }

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
    fprintf(stderr, "Something went wrong with the Hexfile");
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
      fprintf(stderr, "Memory Error @ %x, Op = %s, PC = %x", state->mem_addr,
              OpcodeS[state->op], state->pc);
    }
  if(state->write)
    reg_write(state->rd, state->rd_dat);
}

void emu_exit(EmulatorState* state)
{
  free(state->mem);
  free(state);
}
