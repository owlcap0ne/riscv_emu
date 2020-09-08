#ifndef RISCV_UI
#define RISCV_UI

#include "riscv_emu.h"
#include <curses.h>

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

/*
* Window update functions
*/

void update_InstrWin(WINDOW* win, EmulatorState* state, bool color);

void update_PCWin(WINDOW* win, EmulatorState* state, bool color);

void update_RegWin(WINDOW* win, EmulatorState* state, bool color);

void update_AddrWin(WINDOW* win, EmulatorState* state, bool color);

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

extern const char* OpcodeS[];

extern const char* ITypeS[];

extern const char* ABINameS[];

#endif
