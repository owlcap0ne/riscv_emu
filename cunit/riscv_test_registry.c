#include "CUnit/Basic.h"
#include "CUnit/Automated.h"
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdint.h>
#include <stdlib.h>
#include "../riscv_emu.h"

#define ROOT_NAME "Init"

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
    free(emu_state->mem);
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


