#ifndef RISCV_REGFILE
#define RISCV_REGFILE

uint32_t reg_read(uint8_t reg);

void reg_write(uint8_t reg, uint32_t val);

#endif
