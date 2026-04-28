#include <stdint.h>

static uint32_t regfile[32]; //[0] goes unused but whatever...

uint32_t reg_read(uint8_t reg)
{
    if((reg == 0) || (reg > 31))
        return 0;
    else
        return regfile[reg];
}

void reg_write(uint8_t reg, uint32_t val)
{
    if((reg == 0) || (reg > 31))
        return;
    else
    {
        regfile[reg] = val;
        return;
    }
}

void reg_reset()
{
  for(int n = 0; n < 32; n++)
    regfile[n] = 0;
}
