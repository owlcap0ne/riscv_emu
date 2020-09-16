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

/*
* Window update functions
*/

void update_InstrWin(WINDOW* win, EmulatorState* state, bool color);

void update_PCWin(WINDOW* win, EmulatorState* state, bool color);

void update_RegWin(WINDOW* win, EmulatorState* state, bool color, enum UIBase base);

void update_AddrWin(WINDOW* win, EmulatorState* state, bool color);

void update_MemWin(WINDOW* win, EmulatorState* state, bool color, uint32_t startAddr);

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


#endif
