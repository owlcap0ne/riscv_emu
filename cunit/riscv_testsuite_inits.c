#include <CUnit/CUnit.h>
#include <stdint.h>
#include <stdlib.h>
#include "../riscv_emu.h"
#include "../riscv_decode.h"
#include "../riscv_regfile.h"
#include "../riscv_memory.h"
#include "riscv_test_util.c"

// suite to test various init/zero functions
// prerequisite for most other tests
#ifndef RISCV_TESTSUITE_INITS
#define RISCV_TESTSUITE_INITS

#define MEM_SIZE_SUITE_INITS 0x10000

extern EmulatorState* emu_state;

int init_suite_inits() {
    if((emu_state = initState()) == NULL){
        return -1;
    } else {
        emu_state->mem = (uint8_t*) malloc(sizeof(uint8_t) * MEM_SIZE_SUITE_INITS);
        emu_state->mem_size = MEM_SIZE_SUITE_INITS;
        if(emu_state->mem == NULL)
            return -1;
        else
            return 0;
    }
}

int clean_suite_inits() {
    if(emu_state->mem)
        free(emu_state->mem);
    if(emu_state)
        free(emu_state);
    return 0;
}

// test if EmulatorState is constructed properly
void init_state_test() {
    CU_ASSERT_EQUAL(emu_state->branch, false);
    CU_ASSERT_EQUAL(emu_state->pc, 0);
    CU_ASSERT_EQUAL(emu_state->instr, built_instr(ADDI, 0, 0, 0, 0));
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, 0);
    CU_ASSERT_PTR_NOT_NULL(emu_state->mem);
}

// test if regfile is correctly reset
void init_regs_test() {
    reg_reset();
    for(int i = 0; i < 32; i++) {
        CU_ASSERT_EQUAL(reg_read(i), 0);
    }
}

// check if entire memory is reset
void init_memory_test() {
    long int mem_sum = 0;
    memory_zero(emu_state);
    for(uint32_t i = 0; i < emu_state->mem_size; i++) {
        mem_sum += emu_state->mem[i];
    }
    CU_ASSERT_EQUAL(mem_sum, 0);
}

#endif