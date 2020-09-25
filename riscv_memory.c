#include "riscv_emu.h"

#define _mem state->mem
#define _mem_addr state->mem_addr
#define _mem_size state->mem_size
#define _rd_dat state->rd_dat
#define _rs2_dat state->rs2_dat

static int write_mem_word(EmulatorState* state)
{
    if(_mem_addr + sizeof(uint32_t) > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    uint32_t *int_ptr = (uint32_t*) (_mem + _mem_addr);
    *int_ptr = _rs2_dat;
    return 0;
}

static int write_mem_half(EmulatorState* state)
{
    if(_mem_addr + sizeof(uint16_t) > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    uint16_t *half_ptr = (uint16_t*) (_mem + _mem_addr);
    *half_ptr = (uint16_t) _rs2_dat;
    return 0;
}

static int write_mem_byte(EmulatorState* state)
{
    if(_mem_addr > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    uint8_t *byte_ptr = (uint8_t*) (_mem + _mem_addr);
    *byte_ptr = (uint8_t) _rs2_dat;
    return 0;
}

static int read_mem_word(EmulatorState* state)
{
    if(_mem_addr + sizeof(uint32_t) > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    _rd_dat = *((uint32_t*) (_mem + _mem_addr));
    return 0;
}

static int read_mem_half(EmulatorState* state)
{
    if(_mem_addr + sizeof(uint16_t) > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    _rd_dat = (*((int16_t*) (_mem + _mem_addr)) << 16 >> 16);
    return 0;
}

static int read_mem_halfu(EmulatorState* state)
{
    if(_mem_addr + sizeof(uint16_t) > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    _rd_dat = *((uint16_t*) (_mem + _mem_addr));
    return 0;
}

static int read_mem_byte(EmulatorState* state)
{
    if(_mem_addr > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    _rd_dat = (*((int8_t*) (_mem + _mem_addr)) << 24 >> 24);
    return 0;
}

static int read_mem_byteu(EmulatorState* state)
{
    if(_mem_addr > _mem_size)
    {
        printf("ERROR: invalid memory access @ %x\n", _mem_addr);
        return -1;
    }
    _rd_dat = *((uint8_t*) (_mem + _mem_addr));
    return 0;
}

int memory(EmulatorState* state)
{
    int ret;
    switch(state->op)
    {
        case LB:
            ret = read_mem_byte(state);
            break;

        case LH:
            ret = read_mem_half(state);
            break;

        case LW:
            ret = read_mem_word(state);
            break;

        case LBU:
            ret = read_mem_byteu(state);
            break;

        case LHU:
            ret = read_mem_halfu(state);
            break;

        case SB:
            ret = write_mem_byte(state);
            break;

        case SH:
            ret = write_mem_half(state);
            break;

        case SW:
            ret = write_mem_word(state);
            break;

        default:
            ret = -1;
            break;
    }
    return ret;
}

void memory_zero(EmulatorState* state)
{
  for(uint32_t n = 0; n < state->mem_size; n++)
  {
    state->mem[n] = 0;
  }
}

int memory_dump(EmulatorState* state, const char* dumpfile)
{
  FILE *fp = fopen(dumpfile, "w");
  if(fp == NULL)
  {
    fprintf(stderr, "Unable to open File for writing");
    return -1;
  }

  fprintf(fp, "//RISCV Emulator memory dump\n//uses Verilog-style .hex syntax\n//each symbol represents one memory byte, seperated by whitespaces\n//little endian ordering, starting from addr zero\n");

  for(int i = 0; i < state->mem_size; i++)
  {
    fprintf(fp, "%+02x", state->mem[i]);
    // group four bytes onto a line
    if(((i +1) % 4) == 0)
      fprintf(fp, "\n");
    else
      fprintf(fp, " ");
  }

  fclose(fp);
  return 0;
}
