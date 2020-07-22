#ifndef RISCV_EMU_H
#define RISCV_EMU_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

enum Opcode
{
    LUI,
    AUIPC,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    FENCE,  //ignored
    ECALL,  //ignored
    EBREAK  //ignored
};

/*
//helper function, *regfile should be state->regfile and nothing else
void reg_write(uint8_t reg, int32_t data, int32_t *regfile)
{
    if((reg == 0) || (reg > 31))
        return;
    else
    {
        *(regfile + reg) = data;
        return;
    }
}

//helper function, *regfile should be state->regfile and nothing else
int32_t reg_read(uint8_t reg, int32_t *regfile)
{
    if((reg == 0) || (reg > 31))
        return 0;
    else
        return *(regfile + reg);
}
*/

//treat all register data as signed by default,
//only cast to unsigned as needed
typedef struct
{
    uint32_t pc;
    uint32_t pc_next;           //+1 or branch target
    bool branch;                //to branch or not to branch
    int32_t instr;              //signed, for easy sign extension
    enum Opcode op;
    int32_t imm;                //imm's are always signed
    uint8_t rs1, rs2, rd;       //addr's of the 2 read, 1 write
                                //ported regfile
    int32_t rs1_dat, rs2_dat,   //read data from regfile
            rd_dat;             //writeback data from ALU
    uint32_t mem_addr;          //computed by ALU
    bool write;                 //ALU will write RES or not
    uint8_t *mem;              //move to seperate struct maybe
    size_t mem_size;            //... ^^

} EmulatorState;

EmulatorState* initState()
{
    EmulatorState *state = (EmulatorState*)malloc(sizeof(EmulatorState));

    if(state != NULL)
    {
        state->pc = 0;
        state->pc_next = 0;
        state->branch = false;
        state->instr = 0x13;    //ADDI 0, 0, 0 - NOP
        state->op = ADDI;
        state->imm = 0;
        state->rs1 = 0;
        state->rs2 = 0;
        state->rd = 0;
        state->rs1_dat = 0;
        state->rs2_dat = 0;
        state->rd_dat = 0;
        state->mem_addr = 0;
        state->write = false;
        state->mem = NULL;
        state->mem_size = 0;
    }
    return state;
}

#endif //RISCV_EMU_H
