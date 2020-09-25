#ifndef RISCV_MEMORY
#define RISCV_MEMORY

int memory(EmulatorState* state);

void memory_zero(EmulatorState* state);

int memory_dump(EmulatorState* state, const char* dumpfile);

#endif
