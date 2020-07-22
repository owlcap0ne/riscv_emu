#include "riscv_emu.h"
#include "riscv_decode.h"

void decode(EmulatorState *state){
    
    int32_t instr = (int32_t) state->mem[state->pc];

    if(instr & OPC_U_LUI)
    {
        state->rd = rd(instr);
        state->imm = imm_U(instr);
        state->op = LUI;
        state->write = true;
    }
    else
    if(instr & OPC_U_AUIPC)
    {
        state->rd = rd(instr);
        state->imm = imm_U(instr);
        state->op = AUIPC;
        state->write = true;
    }
    else
    if(instr & OPC_B_BRANCHES)
    {
        state->rs1 = rs1(instr);
        state->rs2 = rs2(instr);
        state->imm = imm_B(instr);
        state->write = false;
        switch(func3(instr))
        {
            case 0:
                state->op = BEQ;
                break;

            case 1:
                state->op = BNE;
                break;

            case 4:
                state->op = BLT;
                break;

            case 5:
                state->op = BGE;
                break;

            case 6:
                state->op = BLTU;
                break;

            case 7:
                state->op = BGEU;
                break;
        }
    }
    else
    if(instr & OPC_I_LOADS)
    {
        state->rd = rd(instr);
        state->rs1 = rs1(instr);
        state->imm = imm_I(instr);
        state->write = true;
        switch(func3(instr))
        {
            case 0:
                state->op = LB;
                break;

            case 1:
                state->op = LH;
                break;

            case 2:
                state->op = LW;
                break;

            case 4:
                state->op = LBU;
                break;

            case 5:
                state->op = LHU;
                break;
        }
    }
    else
    if(instr & OPC_S_STORES)
    {
        state->rs1 = rs1(instr);
        state->rs2 = rs2(instr);
        state->imm = imm_S(instr);
        state->write = false;
        switch(func3(instr))
        {
            case 0:
                state->op = SB;
                break;

            case 1:
                state->op = SH;
                break;

            case 2:
                state->op = SW;
                break;
        }
    }
    else
    if(instr & OPC_I_REGIMM)
    {
        state->rd = rd(instr);
        state->rs1 = rs1(instr);
        state->imm = imm_I(instr);
        state->write = true;
        switch(func3(instr))
        {
            case 0:
                state->op = ADDI;
                break;

            case 2:
                state->op = SLTI;
                break;

            case 3:
                state->op = SLTIU;
                break;

            case 4:
                state->op = XORI;
                break;

            case 6:
                state->op = ORI;
                break;

            case 7:
                state->op = ANDI;
                break;

            //shifts
            //re-use rs2-macro to overwrite imm, not pretty but works
            case 1:
                state->op = SLLI;
                state->imm = rs2(instr);
                break;

            case 5:
                if(instr & MASK_FUNC7)
                    state->op = SRLI;
                else
                    state->op = SRAI;
                state->imm = rs2(instr);
                break;
        }
    }
    else
    if(instr & OPC_R_REGREG)
    {
        state->rd = rd(instr);
        state->rs1 = rs1(instr);
        state->rs2 = rs2(instr);
        state->write = true;
        switch(func3(instr))
        {
            case 0:
                if(instr & MASK_FUNC7)
                    state->op = SUB;
                else
                    state->op = ADD;
                break;

            case 1:
                state->op = SLL;
                break;

            case 2:
                state->op = SLT;
                break;

            case 3:
                state->op = SLTU;
                break;

            case 4:
                state->op = XOR;
                break;

            case 5:
                if(instr & MASK_FUNC7)
                    state->op = SRA;
                else
                    state->op = SRL;
                break;

            case 6:
                state->op = OR;
                break;

            case 7:
                state->op = AND;
                break;
        }
    }
    else
    if(instr & OPC_J_JAL)
    {
        state->rd = rd(instr);
        state->imm = imm_J(instr);
        state->write = true;
        state->op = JAL;
    }
    else
    if(instr & OPC_I_JALR)
    {
        state->rd = rd(instr);
        state->rs1 = rs1(instr);
        state->imm = imm_I(instr);
        state->write = true;
        if(func3(instr) == 0) //TODO: add trap for invalid instructions
            state->op = JALR;
    }
    else
    if(instr & OPC_I_FENCE)
    {
        //NOP - ADDI 0
        state->rd = rd(instr);
        state->rs1 = rs1(instr);
        state->imm = 0;
        state->write = true;
        if(func3(instr) == 0)
            state->op = ADDI;
    }
    else
    if(instr & OPC_I_ENV)
    {
        //NOP - ADDI 0
        state->imm = 0;
        state->write = true;
        //technically imm_I == (0, 1) selects between ECALL and EBREAK
        //two types of NOP, so don't care
        if(func3(instr) == 0)
            state->op = ADDI;
    }
    return;
}
