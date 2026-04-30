// ungodly amount of masking and shifting
// to herd opcodes, reg addresses and
// immediate bits to their correct locations
//
// WELCOME TO HELL!

#include <stdint.h>
#include <time.h>
#include "../riscv_emu.h"
#include "../riscv_decode.h"
#include "../riscv_regfile.h"
#include "../riscv_execute.h"
#include "../riscv_memory.h"
#include "../riscv_ui.h"

#ifndef RISCV_TEST_UTIL
#define RISCV_TEST_UTIL

typedef struct {
    enum Opcode op;
    enum IType itype;
    uint32_t funct7;
    uint32_t funct3;
    uint32_t opcode;
} op_encode_t;

// silently clamping immediate values for random test inputs
#define RISCV_I_TYPE_MAX ((2^12) / 2) -1
#define RISCV_I_TYPE_MIN -((2^12) / 2)

#define RISCV_S_TYPE_MAX ((2^12) / 2) -1
#define RISCV_S_TYPE_MIN -((2^12) / 2)

#define RISCV_B_TYPE_MAX ((2^13) / 2) -1
#define RISCV_B_TYPE_MIN -((2^13) / 2)

#define RISCV_U_TYPE_MAX ((2^32) / 2) -1
#define RISCV_U_TYPE_MIN -((2^32) / 2)

#define RISCV_J_TYPE_MAX ((2^21) / 2) -1
#define RISCV_J_TYPE_MIN -((2^21) / 2)

// masks and shifts for immediates
#define RISCV_MASK_I_11_0       0x00000FFF
#define RISCV_SHIFT_I_11_0      20
#define RISCV_MASK_S_11_5       0x00000FE0
#define RISCV_SHIFT_S_11_5      25
#define RISCV_MASK_S_4_0        0x0000001F
#define RISCV_SHIFT_S_4_0       7
#define RISCV_MASK_B_12         0x00001000
#define RISCV_SHIFT_B_12        31
#define RISCV_MASK_B_10_5       0x000007E0
#define RISCV_SHIFT_B_10_5      25
#define RISCV_MASK_B_4_1        0x0000001E
#define RISCV_SHIFT_B_4_1       8
#define RISCV_MASK_B_11         0x00000800
#define RISCV_SHIFT_B_11        7
#define RISCV_MASK_U_31_12      0xFFFFF000
#define RISCV_SHIFT_U_31_12     12
#define RISCV_MASK_J_20         0x00100000
#define RISCV_SHIFT_J_20        31
#define RISCV_MASK_J_10_1       0x000007FE
#define RISCV_SHIFT_J_10_1      21
#define RISCV_MASK_J_11         0x00000800
#define RISCV_SHIFT_J_11        20
#define RISCV_MASK_J_19_12      0x000FF000
#define RISCV_SHIFT_J_19_12     12

// masks and shifts for function bits, opcodes and registers
#define RISCV_MASK_FUNC3        0x7
#define RISCV_MASK_FUNC7        0x7F
#define RISCV_MASK_RS1          0x1F
#define RISCV_MASK_RS2          0x1F
#define RISCV_MASK_RD           0x1F
#define RISCV_MASK_OP           0x7F

#define RISCV_SHIFT_FUNCT7      25
#define RISCV_SHIFT_FUNCT3      12
#define RISCV_SHIFT_RS2         20
#define RISCV_SHIFT_RS1         15
#define RISCV_SHIFT_RD          7
#define RISCV_SHIFT_OP          0

// helper macros
#define RISCV_FUNCT7(funct7)    ((funct7 & RISCV_MASK_FUNC7) << 25)
#define RISCV_FUNCT3(funct3)    ((funct3 & RISCV_MASK_FUNC3) << 12)
#define RISCV_RS2(rs2)          ((rs2 & RISCV_MASK_RS2) << 20)
#define RISCV_RS1(rs1)          ((rs1 & RISCV_MASK_RS1) << 15)
#define RISCV_RD(rd)            ((rd & RISCV_MASK_RD) << 7)
#define RISCV_OP(opcode)        ((opcode & RISCV_MASK_OP))

//opcodes
#define RISCV_OPC_B_BRANCHES    0x63
//0b1100011
#define RISCV_OPC_I_LOADS       0x03
//0b0000011
#define RISCV_OPC_S_STORES      0x23
//0b0100011
#define RISCV_OPC_I_REGIMM      0x13
//0b0010011
#define RISCV_OPC_R_REGREG      0x33
//0b0110011
#define RISCV_OPC_U_LUI         0x37
//0b0110111
#define RISCV_OPC_U_AUIPC       0x17
//0b0010111
#define RISCV_OPC_J_JAL         0x6F
//0b1101111
#define RISCV_OPC_I_JALR        0x67
//0b1100111
#define RISCV_OPC_I_FENCE       0x0F
//0b0001111
#define RISCV_OPC_I_ENV         0x73
//0b1110011

// shuffle immediate value bits to their locations
uint32_t built_i_imm(int32_t imm){
    uint32_t i_imm = 0;
    i_imm = ((imm & RISCV_MASK_I_11_0) << RISCV_SHIFT_I_11_0);
    return i_imm;
}

uint32_t built_s_imm(int32_t imm){
    uint32_t s_imm = 0;
    s_imm = ((imm & RISCV_MASK_S_11_5) << RISCV_SHIFT_S_11_5) |
            ((imm & RISCV_MASK_S_4_0) << RISCV_SHIFT_S_4_0);
    return s_imm;
}

uint32_t built_b_imm(int32_t imm){
    uint32_t b_imm = 0;
    b_imm = ((imm & RISCV_MASK_B_12) << RISCV_SHIFT_B_12) |
            ((imm & RISCV_MASK_B_10_5) << RISCV_SHIFT_B_10_5) |
            ((imm & RISCV_MASK_B_4_1) << RISCV_SHIFT_B_4_1) |
            ((imm & RISCV_MASK_B_11) << RISCV_SHIFT_B_11);
    return b_imm;
}

uint32_t built_u_imm(int32_t imm){
    uint32_t u_imm = 0;
    u_imm = ((imm & RISCV_MASK_U_31_12) << RISCV_SHIFT_U_31_12);
    return u_imm;
}

uint32_t built_j_imm(int32_t imm){
    uint32_t j_imm = 0;
    j_imm = ((imm & RISCV_MASK_J_20) << RISCV_SHIFT_J_20) |
            ((imm & RISCV_MASK_J_10_1) << RISCV_SHIFT_J_10_1) |
            ((imm & RISCV_MASK_J_11) << RISCV_SHIFT_J_11) |
            ((imm & RISCV_MASK_J_19_12) << RISCV_SHIFT_J_19_12);
    return j_imm;
}

// construct opcodes
uint32_t built_r_type(uint32_t funct7, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode){
    uint32_t instr = 0;
    instr = RISCV_FUNCT7(funct7) |
            RISCV_RS2(rs2) |
            RISCV_RS1(rs1) |
            RISCV_FUNCT3(funct3) |
            RISCV_RD(rd) |
            RISCV_OP(opcode);
    return instr;
}

uint32_t built_i_type(int32_t imm, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode){
    uint32_t instr = built_i_imm(imm);
    instr = instr |
            RISCV_RS1(rs1) |
            RISCV_FUNCT3(funct3) |
            RISCV_RD(rd) |
            RISCV_OP(opcode);
    return instr;
}

// special case needed for shifts
// immediate is partially replaced with funct7
uint32_t built_i_type_shifts(uint32_t funct7, int32_t imm, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode){
    uint32_t instr = built_i_type(imm, rs1, funct3, rd, opcode);
    instr = (instr & ~RISCV_MASK_FUNC7) << RISCV_SHIFT_FUNCT7; 
    instr = instr | RISCV_FUNCT7(funct7);
    return instr;
}

uint32_t  built_s_type(int32_t imm, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode){
    uint32_t instr = built_s_imm(imm);
    instr = instr |
            RISCV_RS2(rs2) |
            RISCV_RS1(rs1) |
            RISCV_FUNCT3(funct3) |
            RISCV_OP(opcode);
    return instr;
}

uint32_t built_b_type(int32_t imm, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode){
    uint32_t instr = built_b_imm(imm);
    instr = instr |
            RISCV_RS2(rs2) |
            RISCV_RS1(rs1) |
            RISCV_FUNCT3(funct3) |
            RISCV_OP(opcode);
    return instr;
}

uint32_t built_u_type(int32_t imm, uint32_t rd, uint32_t opcode){
    uint32_t instr = built_u_imm(imm);
    instr = instr |
            RISCV_RD(rd) |
            RISCV_OP(opcode);
    return instr;
}

uint32_t built_j_type(int32_t imm, uint32_t rd, uint32_t opcode){
    uint32_t instr = built_j_imm(imm);
    instr = instr |
            RISCV_RD(rd) |
            RISCV_OP(opcode);
    return instr;
}

// ultimate instruction construction
// FENCE needs fm | pred | succ (4bit each) passed as imm
// not very elegant, but FENCE is ignored anyway...
uint32_t built_instr(enum Opcode opcode, int32_t imm, uint32_t rs2, uint32_t rs1, uint32_t rd){
    uint32_t instr = 0;
    uint32_t funct3 = 0;
    uint32_t funct7 = 0;
    switch (opcode) {
        case LUI:
            instr = built_u_type(imm, rd, RISCV_OPC_U_LUI);
            break;

        case AUIPC:
            instr = built_u_type(imm, rd, RISCV_OPC_U_AUIPC);
            break;

        case JAL:
            instr = built_j_type(imm, rd, RISCV_OPC_J_JAL);
            break;

        case JALR:
            instr = built_i_type(imm, rs1, 0, rd, RISCV_OPC_I_JALR);
            break;

        case BEQ:
            instr = built_b_type(imm, rs2, rs1, 0, RISCV_OPC_B_BRANCHES);
            break;

        case BNE:
            instr = built_b_type(imm, rs2, rs1, 1, RISCV_OPC_B_BRANCHES);
            break;

        case BLT:
            instr = built_b_type(imm, rs2, rs1, 4, RISCV_OPC_B_BRANCHES); 
            break;

        case BGE:
            instr = built_b_type(imm, rs2, rs1, 5, RISCV_OPC_B_BRANCHES); 
            break;

        case BLTU:
            instr = built_b_type(imm, rs2, rs1, 6, RISCV_OPC_B_BRANCHES); 
            break;

        case BGEU:
            instr = built_b_type(imm, rs2, rs1, 7, RISCV_OPC_B_BRANCHES); 
            break;

        case LB:
            instr = built_i_type(imm, rs1, 0, rd, RISCV_OPC_I_LOADS);
            break;

        case LH:
            instr = built_i_type(imm, rs1, 1, rd, RISCV_OPC_I_LOADS);
            break;

        case LW:
            instr = built_i_type(imm, rs1, 2, rd, RISCV_OPC_I_LOADS); 
            break;

        case LBU:
            instr = built_i_type(imm, rs1, 4, rd, RISCV_OPC_I_LOADS);
            break;

        case LHU:
            instr = built_i_type(imm, rs1, 5, rd, RISCV_OPC_I_LOADS);
            break;

        case SB:
            instr = built_s_type(imm, rs2, rs1, 0, RISCV_OPC_S_STORES);
            break;

        case SH:
            instr = built_s_type(imm, rs2, rs1, 1, RISCV_OPC_S_STORES);
            break;

        case SW:
            instr = built_s_type(imm, rs2, rs1, 2, RISCV_OPC_S_STORES);
            break;

        case ADDI:
            instr = built_i_type(imm, rs1, 0, rd, RISCV_OPC_I_REGIMM);
            break;

        case SLTI:
            instr = built_i_type(imm, rs1, 2, rd, RISCV_OPC_I_REGIMM);
            break;

        case SLTIU:
            instr = built_i_type(imm, rs1, 3, rd, RISCV_OPC_I_REGIMM);
            break;

        case XORI:
            instr = built_i_type(imm, rs1, 4, rd, RISCV_OPC_I_REGIMM);
            break;

        case ORI:
            instr = built_i_type(imm, rs1, 6, rd, RISCV_OPC_I_REGIMM);
            break;

        case ANDI:
            instr = built_i_type(imm, rs1, 7, rd, RISCV_OPC_I_REGIMM);
            break;

        case SLLI:
            instr = built_i_type_shifts(0, imm, rs1, 1, rd, RISCV_OPC_I_REGIMM);
            break;

        case SRLI:
            instr = built_i_type_shifts(0, imm, rs1, 5, rd, RISCV_OPC_I_REGIMM);
            break;

        case SRAI:
            instr = built_i_type_shifts(32, imm, rs1, 5, rd, RISCV_OPC_I_REGIMM);
            break;

        case ADD:
            instr = built_r_type(0, rs2, rs1, 0, rd, RISCV_OPC_R_REGREG);
            break;

        case SUB:
            instr = built_r_type(32, rs2, rs1, 0, rd, RISCV_OPC_R_REGREG);
            break;

        case SLL:
            instr = built_r_type(0, rs2, rs1, 1, rd, RISCV_OPC_R_REGREG);
            break;

        case SLT:
            instr = built_r_type(0, rs2, rs1, 2, rd, RISCV_OPC_R_REGREG);
            break;

        case SLTU:
            instr = built_r_type(0, rs2, rs1, 3, rd, RISCV_OPC_R_REGREG);
            break;

        case XOR:
            instr = built_r_type(0, rs2, rs1, 4, rd, RISCV_OPC_R_REGREG);
            break;

        case SRL:
            instr = built_r_type(0, rs2, rs1, 5, rd, RISCV_OPC_R_REGREG);
            break;

        case SRA:
            instr = built_r_type(32, rs2, rs1, 5, rd, RISCV_OPC_R_REGREG);
            break;

        case OR:
            instr = built_r_type(0, rs2, rs1, 6, rd, RISCV_OPC_R_REGREG);
            break;

        case AND:
            instr = built_r_type(0, rs2, rs1, 7, rd, RISCV_OPC_R_REGREG);
            break;

        case FENCE:
            instr = built_i_type(imm, rs1, 0, rd, RISCV_OPC_I_FENCE);
            break;

        case ECALL:
            instr = built_i_type(0, 0, 0, 0, RISCV_OPC_I_ENV);
            break;

        case EBREAK:
            instr = built_i_type(1, 0, 0, 0, RISCV_OPC_I_ENV);
            break;

        // ADDI imm 0, rs 10, rd 0 -> NOP
        default:
            instr = built_i_type(0, 0, 0, 0, RISCV_OPC_I_REGIMM);
            break;
    }
    return instr;
}

int32_t rand_imm(int32_t min, int32_t max) {
    unsigned int seed = time(0);
    return (int32_t) rand_r(&seed) % (max - min + 1) + min;
}
// i s b u j
int32_t rand_i_imm() {
    return rand_imm(RISCV_I_TYPE_MIN, RISCV_I_TYPE_MAX);
}

int32_t rand_s_imm() {
    return rand_imm(RISCV_S_TYPE_MIN, RISCV_S_TYPE_MAX);
}

int32_t rand_b_imm() {
    return rand_imm(RISCV_B_TYPE_MIN, RISCV_B_TYPE_MAX);
}

int32_t rand_u_imm() {
    return rand_imm(RISCV_U_TYPE_MIN, RISCV_U_TYPE_MAX);
}

int32_t rand_j_imm() {
    return rand_imm(RISCV_J_TYPE_MIN, RISCV_J_TYPE_MAX);
}

int32_t clamp_i_imm(int32_t imm) {
    if(imm > RISCV_I_TYPE_MAX)
        imm = RISCV_I_TYPE_MAX;
    if(imm < RISCV_I_TYPE_MIN)
        imm = RISCV_I_TYPE_MIN;
    return imm;
}

int32_t clamp_s_imm(int32_t imm) {
    if(imm > RISCV_S_TYPE_MAX)
        imm = RISCV_S_TYPE_MAX;
    if(imm < RISCV_S_TYPE_MIN)
        imm = RISCV_S_TYPE_MIN;
    return imm;
}

int32_t clamp_b_imm(int32_t imm) {
    if(imm > RISCV_B_TYPE_MAX)
        imm = RISCV_B_TYPE_MAX;
    if(imm < RISCV_B_TYPE_MIN)
        imm = RISCV_B_TYPE_MIN;
    return imm;
}

int32_t clamp_u_imm(int32_t imm) {
    if(imm > RISCV_U_TYPE_MAX)
        imm = RISCV_U_TYPE_MAX;
    if(imm < RISCV_U_TYPE_MIN)
        imm = RISCV_U_TYPE_MIN;
    return imm;
}

int32_t clamp_j_imm(int32_t imm) {
    if(imm > RISCV_J_TYPE_MAX)
        imm = RISCV_J_TYPE_MAX;
    if(imm < RISCV_J_TYPE_MIN)
        imm = RISCV_J_TYPE_MIN;
    return imm;
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

#endif