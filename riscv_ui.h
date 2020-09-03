#include "riscv_emu.h"
#include <curses.h>

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

struct EmulatorWin
{
    //coordinates of *usable* screen space, without borders
    int y;          //top
    int x;          //left
    int w;          //width
    int h;          //height
    WINDOW* win;    //curses window
};

/*
 * prints number as binary into specified window at specified cursor position
 * stolen from https://stackoverflow.com/a/3974138
 *
 */
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
