#include "../riscv_emu/riscv_ui.h"
#include "../riscv_emu/riscv_regfile.c"

#define str(x)
#define xstr(x) str(x)

WINDOW *createWin(int height, int width, int starty, int startx);
void destroyWin(WINDOW* local_win);

int main(int argc, char* argv[])
{
    WINDOW* winInstr;
    WINDOW* winPC;
    WINDOW* winAddr;
    WINDOW* winRegs;
    WINDOW* winMem;
    int startx, starty, width, height;
    int ch;

    enum Opcode op = FENCE;

    initscr();
    cbreak();
    keypad(stdscr, TRUE);

    height = 3;
    width = 35;
    startx = (LINES - height) / 2;
    starty = (COLS - width) / 2;

    printw("Press F1 to exit");
    refresh();

    //    createWin(height, width, starty, startx);
    winInstr = createWin(5, 41, 0, 0);
    winPC = createWin(5, 39, 0, 41);
    winAddr = createWin(4, 80, 5, 0);
    winRegs = createWin(35, 41, 9, 0);
    winMem = createWin(35, 39, 9, 41);
    mvwprintw(winInstr, 1, 1, "Instruction:");
    mvwprintw(winInstr, 1, 14, "%s", OpcodeS[op]);
    mvwprintw(winInstr, 1, 32, "(I-Type)");
    uint32_t instr = 0x100073;
    mvwprintw(winInstr, 2, 40-10, "0x%+8X", instr);
    //mvwprintwBits(winInstr, 3, 40-32, sizeof(instr), &instr);
    wmove(winInstr, 3, 40-32);
    for(int i = 31; i >= 0; i--)
    {
        waddBit(winInstr, instr, i);
    }

    wrefresh(winInstr);

    for(int j = 0; j < 32; j++)
    {
        mvwprintw(winRegs, 2 + j, 1, "%+2u", j);
        wattron(winRegs, A_REVERSE);
        wprintw(winRegs, "%+5s", ABINameS[j]);
        wattroff(winRegs, A_REVERSE);
        for(int b = 31; b >= 0; b--)
        {
            waddBit(winRegs, reg_read(j), b);
        }
    }

    wrefresh(winRegs);

    mvwprintw(winPC, 1, 1, "Program Counter - curr:");
    mvwprintw(winPC, 2, 1 +16, "- next:");
    mvwprintw(winPC, 3, 1 +16, "branch:");

    wrefresh(winPC);

    while((ch = getch()) != KEY_F(1))
    {

    }

    destroyWin(winInstr);
    destroyWin(winPC);
    destroyWin(winRegs);
    destroyWin(winMem);
    endwin();
    return 0;
}

WINDOW* createWin(int height, int width, int starty, int startx)
{
    WINDOW* local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0); // 0, 0 -> default chars for vert and horz

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
