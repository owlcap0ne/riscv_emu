#include <CUnit/CUnit.h>
#include "../riscv_emu.h"
#include "riscv_test_util.c"

#ifndef RISCV_TESTSUITE_DUMMY
#define RISCV_TESTSUITE_DUMMY

#define MEM_SIZE_SUITE_DUMMY 0x10000

extern EmulatorState* emu_state;

int init_suite_dummy() {
    if((emu_state = initState()) == NULL){
        return -1;
    } else {
        emu_state->mem = (uint8_t*) malloc(sizeof(uint8_t) * MEM_SIZE_SUITE_DUMMY);
        emu_state->mem_size = MEM_SIZE_SUITE_DUMMY;
        if(emu_state->mem == NULL)
            return -1;
        else
            return 0;
    }
}

int clean_suite_dummy() {
    if(emu_state->mem)
        free(emu_state->mem);
    if(emu_state)
        free(emu_state);
    return 0;
}

void dummy_test() {
    CU_ASSERT_EQUAL(1, 1);
}

#endif