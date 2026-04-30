#include <CUnit/CUnit.h>
#include <stdint.h>
#include <stdlib.h>
#include "../riscv_emu.h"
#include "../riscv_decode.h"
#include "../riscv_regfile.h"
#include "../riscv_pc.h"
#include "riscv_test_util.c"

// suite to test various jumps
// no full coverage over all instructions
#ifndef RISCV_TESTSUITE_JUMPS
#define RISCV_TESTSUITE_JUMPS

#define MEM_SIZE_SUITE_JUMPS 0x10000
#define N_RANDOM_SUITE_JUMPS 10

extern EmulatorState* emu_state;

int init_suite_jumps() {
    if((emu_state = initState()) == NULL){
        return -1;
    } else {
        emu_state->mem = (uint8_t*) malloc(sizeof(uint8_t) * MEM_SIZE_SUITE_JUMPS);
        emu_state->mem_size = MEM_SIZE_SUITE_JUMPS;
        if(emu_state->mem == NULL)
            return -1;
        else
            return 0;
    }
}

int clean_suite_jumps() {
    if(emu_state->mem)
        free(emu_state->mem);
    if(emu_state)
        free(emu_state);
    return 0;
}

// test if PC is incremented by 4
void jumps_inc_pc_test() {
    resetState(emu_state);
    uint32_t pc_old = emu_state->pc;
    CU_ASSERT_FALSE(emu_state->branch);
    pc_inc(emu_state);
    uint32_t pc_new = emu_state->pc;
    CU_ASSERT_EQUAL(pc_new - pc_old, 4);
}

// tests unconditional jump-and-link JAL instruction
void jumps_rand_jal_test() {
    for(int i = 0; i < N_RANDOM_SUITE_JUMPS; i++) {
        uint32_t instr = 0;
        uint32_t pc_old;
        int32_t imm;
        uint32_t reg_link = 3;

        resetState(emu_state);
        reg_reset();

        // set PC to middle of possible 32 bit range
        emu_state->pc = 0x7FFFFFF0;
        pc_old = emu_state->pc;
        CU_ASSERT_FALSE(emu_state->branch);

        imm = rand_j_imm();
        instr = built_instr(JAL, imm, 0, 0, reg_link);
        test_util_execute(emu_state, instr);
        CU_ASSERT_TRUE(emu_state->branch);

        pc_inc(emu_state);
        CU_ASSERT_EQUAL(emu_state->pc, emu_state->branch_target);
    }
}


#endif