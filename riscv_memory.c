#include "riscv_emu.h"

#define _mem state->mem
#define _mem_addr state->mem_addr
#define _rd_dat state->rd_dat

static void write_mem_word(EmulatorState* state)
{
    uint32_t *int_ptr = (uint32_t*) _mem + _mem_addr;
    *int_ptr = _rd_dat;
    return;
}

static void write_mem_half(EmulatorState* state)
{
    uint16_t *half_ptr = (uint16_t*) _mem + _mem_addr;
    *half_ptr = (uint16_t) _rd_dat;
    return;
}

static void write_mem_byte(EmulatorState* state)
{
    uint8_t *byte_ptr = (uint8_t*) _mem + _mem_addr;
    *byte_ptr = (uint8_t) _rd_dat;
    return;
}

static void read_mem_word(EmulatorState* state)
{
    _rd_dat = *((uint32_t*) (_mem + _mem_addr));
    return;
}

static void read_mem_half(EmulatorState* state)
{
    _rd_dat = (*((int16_t*) (_mem + _mem_addr)) << 16 >> 16);
    return;
}

static void read_mem_halfu(EmulatorState* state)
{
    _rd_dat = *((uint16_t*) (_mem + _mem_addr));
    return;
}

static void read_mem_byte(EmulatorState* state)
{
    _rd_dat = (*((int8_t*) (_mem + _mem_addr)) << 24 >> 24);
    return;
}

static void read_mem_byteu(EmulatorState* state)
{
    _rd_dat = *((uint8_t*) (_mem + _mem_addr));
    return;
}

void memory(EmulatorState* state)
{
    switch(state->op)
    {
        case LB:
            read_mem_byte(state);
            break;

        case LH:
            read_mem_half(state);
            break;

        case LW:
            read_mem_word(state);
            break;

        case LBU:
            read_mem_byteu(state);
            break;

        case LHU:
            read_mem_byteu(state);
            break;

        case SB:
            write_mem_byte(state);
            break;

        case SH:
            write_mem_half(state);
            break;

        case SW:
            write_mem_word(state);
            break;
    }
    return;
}
