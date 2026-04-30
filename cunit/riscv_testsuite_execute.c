#include <CUnit/CUnit.h>
#include <stdint.h>
#include <stdlib.h>
#include "../riscv_emu.h"
#include "../riscv_decode.h"
#include "../riscv_regfile.h"
#include "riscv_test_util.c"

// suite to test instruction execution
// only actually tests a few select instructions
// full coverage would require way more testcases
#ifndef RISCV_TESTSUITE_EXECUTE
#define RISCV_TESTSUITE_EXECUTE

#define MEM_SIZE_SUITE_EXECUTE 0x10000
#define N_RANDOM_SUITE_EXECUTE 10

extern EmulatorState* emu_state;

int init_suite_execute() {
    if((emu_state = initState()) == NULL){
        return -1;
    } else {
        emu_state->mem = (uint8_t*) malloc(sizeof(uint8_t) * MEM_SIZE_SUITE_EXECUTE);
        emu_state->mem_size = MEM_SIZE_SUITE_EXECUTE;
        if(emu_state->mem == NULL)
            return -1;
        else
            return 0;
    }
}

int clean_suite_execute() {
    if(emu_state->mem)
        free(emu_state->mem);
    if(emu_state)
        free(emu_state);
    return 0;
}

// test IMM-REG instruction
void execute_addi_rand_test() {
    for(int i = 0; i < N_RANDOM_SUITE_EXECUTE; i++) {
        int32_t imm = rand_i_imm();
        uint32_t instr = built_instr(ADDI, imm, 0, 0, 1);
        reg_reset();
        test_util_execute(emu_state, instr);
        CU_ASSERT_EQUAL((int32_t) reg_read(1), imm);
    }
}

// test REG-REG instruction with random input values
void execute_add_rand_test() {
    for(int i = 0; i < N_RANDOM_SUITE_EXECUTE; i++) {
        reg_reset();
        // result: reg_3 = reg_1 + reg_2
        uint32_t reg_1 = 1;
        uint32_t reg_2 = 2;
        uint32_t reg_res = 3;
        uint32_t instr = 0;
        int32_t imm;
        int32_t val_1;
        int32_t val_2;
        int32_t val_res;

        // load first imm
        imm = rand_i_imm();
        val_1 = imm;
        instr = built_instr(ADDI, imm, 0, 0, reg_1);
        test_util_execute(emu_state, instr);

        imm = rand_i_imm();
        val_2 = imm;
        instr = built_instr(ADDI, imm, 0, 0, reg_2);
        test_util_execute(emu_state, instr);
        val_res = val_1 + val_2;

        // perform val_1 + val_2
        instr = built_instr(ADD, 0, reg_2, reg_1, reg_res);
        test_util_execute(emu_state, instr);
        CU_ASSERT_EQUAL((int32_t) reg_read(reg_res), val_res);
    }
}



#endif