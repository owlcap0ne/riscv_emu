#include "riscv_emu.h"
#include "riscv_decode.h"

#define _instr state->instr
#define _rd state->rd
#define _rs1 state->rs1
#define _rs2 state->rs2
#define _imm state->imm
#define _op state->op
#define _write state->write
#define _memory state->memory


void decode(EmulatorState *state){

    uint8_t op_tmp = op(_instr);
    uint8_t rd_tmp = rd(_instr);
    uint8_t rs1_tmp = rs1(_instr);
    uint8_t rs2_tmp = rs2(_instr);
    uint8_t func3_tmp = func3(_instr);
    uint8_t func7_tmp = func7(_instr);
    int32_t imm_I_tmp = imm_I(_instr);
    int32_t imm_S_tmp = imm_S(_instr);
    int32_t imm_B_tmp = imm_B(_instr);
    int32_t imm_U_tmp = imm_U(_instr);
    int32_t imm_J_tmp = imm_J(_instr);
    if(op_tmp == OPC_U_LUI)
    {
        _rd = rd(_instr);
        _imm = imm_U(_instr);
        _op = LUI;
        _write = true;
        _memory = false;
    }
    else
    if(op_tmp == OPC_U_AUIPC)
    {
        _rd = rd(_instr);
        _imm = imm_U(_instr);
        _op = AUIPC;
        _write = true;
        _memory = false;
    }
    else
    if(op_tmp == OPC_B_BRANCHES)
    {
        _rs1 = rs1(_instr);
        _rs2 = rs2(_instr);
        _imm = imm_B(_instr);
        _write = false;
        _memory = false;
        switch(func3(_instr))
        {
            case 0:
                _op = BEQ;
                break;

            case 1:
                _op = BNE;
                break;

            case 4:
                _op = BLT;
                break;

            case 5:
                _op = BGE;
                break;

            case 6:
                _op = BLTU;
                break;

            case 7:
                _op = BGEU;
                break;
        }
    }
    else
    if(op_tmp == OPC_I_LOADS)
    {
        _rd = rd(_instr);
        _rs1 = rs1(_instr);
        _imm = imm_I(_instr);
        _write = true;
        _memory = true;
        switch(func3(_instr))
        {
            case 0:
                _op = LB;
                break;

            case 1:
                _op = LH;
                break;

            case 2:
                _op = LW;
                break;

            case 4:
                _op = LBU;
                break;

            case 5:
                _op = LHU;
                break;
        }
    }
    else
    if(op_tmp == OPC_S_STORES)
    {
        _rs1 = rs1(_instr);
        _rs2 = rs2(_instr);
        _imm = imm_S(_instr);
        _write = false;
        _memory = true;
        switch(func3(_instr))
        {
            case 0:
                _op = SB;
                break;

            case 1:
                _op = SH;
                break;

            case 2:
                _op = SW;
                break;
        }
    }
    else
    if(op_tmp == OPC_I_REGIMM)
    {
        _rd = rd(_instr);
        _rs1 = rs1(_instr);
        _imm = imm_I(_instr);
        _write = true;
        _memory = false;
        switch(func3(_instr))
        {
            case 0:
                _op = ADDI;
                break;

            case 2:
                _op = SLTI;
                break;

            case 3:
                _op = SLTIU;
                break;

            case 4:
                _op = XORI;
                break;

            case 6:
                _op = ORI;
                break;

            case 7:
                _op = ANDI;
                break;

            //shifts
            //re-use rs2-macro to overwrite imm, not pretty but works
            case 1:
                _op = SLLI;
                _imm = rs2(_instr);
                break;

            case 5:
                if(_instr & MASK_FUNC7)
                    _op = SRLI;
                else
                    _op = SRAI;
                _imm = rs2(_instr);
                break;
        }
    }
    else
    if(op_tmp == OPC_R_REGREG)
    {
        _rd = rd(_instr);
        _rs1 = rs1(_instr);
        _rs2 = rs2(_instr);
        _write = true;
        _memory = false;
        switch(func3(_instr))
        {
            case 0:
                if(_instr & MASK_FUNC7)
                    _op = SUB;
                else
                    _op = ADD;
                break;

            case 1:
                _op = SLL;
                break;

            case 2:
                _op = SLT;
                break;

            case 3:
                _op = SLTU;
                break;

            case 4:
                _op = XOR;
                break;

            case 5:
                if(_instr & MASK_FUNC7)
                    _op = SRA;
                else
                    _op = SRL;
                break;

            case 6:
                _op = OR;
                break;

            case 7:
                _op = AND;
                break;
        }
    }
    else
    if(op_tmp == OPC_J_JAL)
    {
        _rd = rd(_instr);
        _imm = imm_J(_instr);
        _write = true;
        _op = JAL;
        _memory = false;
    }
    else
    if(op_tmp == OPC_I_JALR)
    {
        _rd = rd(_instr);
        _rs1 = rs1(_instr);
        _imm = imm_I(_instr);
        _write = true;
        _memory = false;
        if(func3(_instr) == 0) //TODO: add trap for invalid _instructions
            _op = JALR;
    }
    else
    if(op_tmp == OPC_I_FENCE)
    {
        //NOP - ADDI 0
        _rd = rd(_instr);
        _rs1 = rs1(_instr);
        _imm = 0;
        _write = true;
        _memory = false;
        if(func3(_instr) == 0)
            _op = ADDI;
    }
    else
    if(op_tmp == OPC_I_ENV)
    {
        //NOP - ADDI 0
        _imm = 0;
        _write = false;
        _memory = false;
        //technically imm_I == (0, 1) selects between ECALL and EBREAK
        //two types of NOP, so don't care
        if(func3(_instr) == 0)
            _op = ADDI;
    }
    return;
}
