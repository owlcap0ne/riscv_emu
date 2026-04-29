#include "CUnit/Basic.h"
#include "CUnit/Automated.h"
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdint.h>
#include <stdlib.h>
#include "../riscv_emu.h"
#include "../riscv_decode.h"
#include "../riscv_regfile.h"
#include "../riscv_execute.h"
#include "../riscv_memory.h"
#include "../riscv_ui.h"
#include "riscv_test_util.c"

#define ROOT_NAME "Test-Init"

#define MEM_SIZE 0x10000
#define TESTING
#define TEST_AUTOMATIC

static EmulatorState* emu_state = NULL;

int init_suite() {
    if((emu_state = initState()) == NULL){
        return -1;
    } else {
        emu_state->mem = (uint8_t*) malloc(sizeof(uint8_t) * MEM_SIZE);
        emu_state->mem_size = MEM_SIZE;
        if(emu_state->mem == NULL)
            return -1;
        else
            return 0;
    }
}

int clean_suite() {
    if(emu_state->mem)
        free(emu_state->mem);
    if(emu_state)
        free(emu_state);
    return 0;
}

void init_test() {
    CU_ASSERT_EQUAL(emu_state->branch, false);
    CU_ASSERT_EQUAL(emu_state->pc, 0);
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, 0);
    CU_ASSERT_PTR_NOT_NULL(emu_state->mem);
}

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

void test_util_execute(EmulatorState* state, uint32_t instr) {
    resetState(state);

    state->instr = instr;
    decode(state);

    state->rs1_dat = reg_read(state->rs1);
    state->rs2_dat = reg_read(state->rs2);

    execute(state);

    if(state->memory)
        if(memory(state))
        {
        fprintf(stderr, "Memory Error @ %x, Op = %s, PC = %x\n", state->mem_addr,
              OpcodeS[state->op], state->pc);
        }

    if(state->write)
        reg_write(state->rd, state->rd_dat);
}

void addi_rand_test() {
    for(int i = 0; i < 5; i++) {
        int32_t imm = rand_i_imm();
        uint32_t instr = built_instr(ADDI, imm, 0, 0, 1);
        reg_reset();
        test_util_execute(emu_state, instr);
        CU_ASSERT_EQUAL((int32_t) reg_read(1), imm);
    }
}

void execute_add_test() {
    uint32_t instr = built_instr(ADDI, 1, 0, 0, 1);
    resetState(emu_state);

    emu_state->instr = instr;
    decode(emu_state);

    emu_state->rs1_dat = reg_read(emu_state->rs1);
    emu_state->rs2_dat = reg_read(emu_state->rs2);

    execute(emu_state);

    if(emu_state->write)
        reg_write(emu_state->rd, emu_state->rd_dat);

    CU_ASSERT_EQUAL((int) reg_read(1), 1);
}

void decode_signed_test() {
    uint32_t instr = built_instr(ADDI, -1, 0, 0, 0);
    resetState(emu_state);
    emu_state->instr = instr;
    decode(emu_state);
    CU_ASSERT_EQUAL(emu_state->op, ADDI);
    CU_ASSERT_EQUAL(emu_state->itype, I_Type);
    CU_ASSERT_EQUAL(emu_state->imm, -1);
    CU_ASSERT_EQUAL(emu_state->rs1, 0);
    CU_ASSERT_EQUAL(emu_state->rd, 0);
}

#ifdef TESTING
int main() {

    CU_pSuite pSuite = NULL;

    // init CUnit test registry
    if(CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    // add suite to registry
    pSuite = CU_add_suite("init_test_suite", init_suite,clean_suite);
    // check success
    if(pSuite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // add test to suite
    if(CU_add_test(pSuite, "init_test", init_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite, "decode_nop_test", decode_nop_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite, "decode_signed_test", decode_signed_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite, "execute_add_test", execute_add_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite, "addi_rand_test", addi_rand_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    #ifdef TEST_AUTOMATIC

    CU_set_output_filename(ROOT_NAME);
    CU_ErrorCode ret = CU_list_tests_to_file();
    CU_automated_run_tests();

    #else
    // CU_BRM_VERBOSE - max level of details
    // other opts: CU_BRM_SILENT, CU_BRM_NORMAL
    CU_basic_set_mode(CU_BRM_VERBOSE);

    CU_basic_run_tests();
    #endif

    CU_cleanup_registry();

    return CU_get_error();
}
#endif


