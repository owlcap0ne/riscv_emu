#include <CUnit/CUnit.h>
#include <stdint.h>
#include <stdlib.h>
#include "../riscv_emu.h"
#include "../riscv_decode.h"
#include "riscv_test_util.c"

// suite to test instruction decode
// only really handles IMM-REG type
// comprehensive suite for all instruction types would be way larger
#ifndef RISCV_TESTSUITE_DECODE
#define RISCV_TESTSUITE_DECODE

#define MEM_SIZE_SUITE_DECODE 0x10000
#define N_RANDOM_SUITE_DECODE 10

extern EmulatorState* emu_state;

int init_suite_decode() {
    if((emu_state = initState()) == NULL){
        return -1;
    } else {
        emu_state->mem = (uint8_t*) malloc(sizeof(uint8_t) * MEM_SIZE_SUITE_DECODE);
        emu_state->mem_size = MEM_SIZE_SUITE_DECODE;
        if(emu_state->mem == NULL)
            return -1;
        else
            return 0;
    }
}

int clean_suite_decode() {
    if(emu_state->mem)
        free(emu_state->mem);
    if(emu_state)
        free(emu_state);
    return 0;
}

// simplest operation: ADDI 0, 0, 0 = NOP
void decode_nop_test() {
    uint32_t instr = built_instr(ADDI, 0, 0, 0, 0);
    resetState(emu_state);
    emu_state->instr = instr;
    decode(emu_state);
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, 0);
    CU_ASSERT_EQUAL(emu_state->rs1, 0);
    CU_ASSERT_EQUAL(emu_state->rd, 0);
}

// check if sign extension works for -1
void decode_imm_negative_test() {
    uint32_t instr = built_instr(ADDI, -1, 0, 0, 0);
    resetState(emu_state);
    emu_state->instr = instr;
    decode(emu_state);
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, -1);
}

// see if sign extension works for +1
void decode_imm_positive_test() {
    uint32_t instr = built_instr(ADDI, 1, 0, 0, 0);
    resetState(emu_state);
    emu_state->instr = instr;
    decode(emu_state);
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, 1);
}

// try maximum positive immediate value
void decode_imm_posmax_test() {
    uint32_t instr = built_instr(ADDI, RISCV_I_TYPE_MAX, 0, 0, 0);
    resetState(emu_state);
    emu_state->instr = instr;
    decode(emu_state);
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, RISCV_I_TYPE_MAX);
}

// try minimum negative immediate value
void decode_imm_negmin_test() {
    uint32_t instr = built_instr(ADDI, RISCV_I_TYPE_MIN, 0, 0, 0);
    resetState(emu_state);
    emu_state->instr = instr;
    decode(emu_state);
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, RISCV_I_TYPE_MIN);
}

// try random immediates
void decode_imm_random_test() {
    for(int i = 0; i < N_RANDOM_SUITE_DECODE; i++) {
        int32_t imm = rand_i_imm();
        uint32_t instr = built_instr(ADDI, imm, 0, 0, 0);
        resetState(emu_state);
        emu_state->instr = instr;
        decode(emu_state);
        CU_ASSERT_EQUAL(emu_state->imm, imm);
    }
}

#endif