#ifndef RISCV_UI
#define RISCV_UI

#include "riscv_emu.h"
#include <curses.h>

/*
 * Enums and Strings
 */
extern const char* OpcodeS[];

extern const char* ITypeS[];

extern const char* ABINameS[];

enum UIBase
{
    BIN,
    DEC,
    HEX
};

typedef struct
{
  WINDOW* winInstr;
  WINDOW* winPC;
  WINDOW* winAddr;
  WINDOW* winRegs;
  WINDOW* winMem;
  WINDOW* winCtrl;

  enum UIBase regFileBase;
  uint32_t memAddr;
  bool hasColor;
} UIState;

/*
* Window init functions
*/

/*
*
*/
void init_InstrWin(WINDOW* win);

/*
*
*/
void init_PCWin(WINDOW* win);

void init_RegWin(WINDOW* win);

void init_AddrWin(WINDOW* win);

void init_MemWin(WINDOW* win);

void init_CtrlWin(WINDOW* win);

/*
* Window update functions
*/

void update_InstrWin(UIState* ui, EmulatorState* state);

void update_PCWin(UIState* ui, EmulatorState* state);

void update_RegWin(UIState* ui, EmulatorState* state);

void update_AddrWin(UIState* ui, EmulatorState* state);

void update_MemWin(UIState* ui, EmulatorState* state);

void update_CtrlWin(UIState* ui, EmulatorState* state);

void update_UI(UIState* ui, EmulatorState* state);

void change_MemAddr(UIState* ui);

/*
* Helpers
*/

WINDOW* createWin(int height, int width, int starty, int startx);

void destroyWin(WINDOW* local_win);

/*
 * prints number as binary into specified window at specified cursor position
 * stolen from https://stackoverflow.com/a/3974138
 */
void mvwprintwBits(WINDOW* win, int y, int x, size_t const size, void const* ptr);

void waddBit(WINDOW* win, uint32_t data, unsigned int pos);


UIState* initUI(void);

void ui_exit(UIState* ui);


#endif
