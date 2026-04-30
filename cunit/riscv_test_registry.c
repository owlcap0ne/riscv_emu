#include "CUnit/Basic.h"
#include "CUnit/Automated.h"
#include <CUnit/CUError.h>
#include <CUnit/CUnit.h>
#include <CUnit/TestDB.h>
#include <stdint.h>
#include "../riscv_emu.h"
#include "../riscv_decode.h"
#include "riscv_test_util.c"

// includes for testsuites go below
static EmulatorState* emu_state = NULL;
#include "riscv_testsuite_dummy.c"

#include "riscv_testsuite_inits.c"
#include "riscv_testsuite_decode.c"

#define ROOT_NAME "Test-Emulator"

#define MEM_SIZE 0x10000
#define TESTING
#define TEST_AUTOMATIC


#ifdef TESTING
int main() {

    /*
    *   Test Suite Pointers
    */

    CU_pSuite pSuite_dummy = NULL;
    CU_pSuite pSuite_decode = NULL;

    CU_pSuite pSuite_inits = NULL;

    // init CUnit test registry
    if(CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();


    /*
    *   Add Test Suites
    */

    // add test suites

    pSuite_dummy = CU_add_suite("dummy_test_suite", init_suite_dummy, clean_suite_dummy);
    if(pSuite_dummy == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }
    pSuite_inits = CU_add_suite("inits_test_suite", init_suite_inits,clean_suite_inits);
    if (pSuite_inits == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    pSuite_decode = CU_add_suite("decode_test_suite", init_suite_decode, clean_suite_decode);
    if(pSuite_decode == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }

    /*
    *   dummy_suite tests
    */

    if(CU_add_test(pSuite_dummy, "dummy_test", dummy_test) == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(pSuite_inits, "init_state_test", init_state_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(pSuite_inits, "init_regs_test", init_regs_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(pSuite_inits, "init_memory_test", init_memory_test) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    /*
    *   decode_suite tests
    */

    if(CU_add_test(pSuite_decode, "decode_nop_test", decode_nop_test) == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite_decode, "decode_imm_negative_test", decode_imm_negative_test) == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite_decode, "decode_imm_positive_test", decode_imm_positive_test) == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite_decode, "decode_imm_posmax_test", decode_imm_posmax_test) == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite_decode, "decode_imm_negmin_test", decode_imm_negmin_test) == NULL){
        CU_cleanup_registry();
        return CU_get_error();
    }

    if(CU_add_test(pSuite_decode, "decode_imm_random_test", decode_imm_random_test) == NULL){
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


