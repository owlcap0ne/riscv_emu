#include "riscv_emu.h"
#include "riscv_decode.h"

#define _instr state->instr
#define _rd state->rd
#define _rs1 state->rs1
#define _rs2 state->rs2
#define _imm state->imm
#define _op state->op
#define _itype state->itype
#define _write state->write
#define _memory state->memory


void decode(EmulatorState *state){

    uint8_t op = OP(_instr);
    uint8_t func3 = FUNC3(_instr);
    uint8_t func7 = FUNC7(_instr);
    /*
     * just for debug
    int32_t imm_I_tmp = IMM_I(_instr);
    int32_t imm_S_tmp = IMM_S(_instr);
    int32_t imm_B_tmp = IMM_B(_instr);
    int32_t imm_U_tmp = IMM_U(_instr);
    int32_t imm_J_tmp = IMM_J(_instr);

    uint8_t rd_tmp = RD(_instr);
    uint8_t rs1_tmp = RS1(_instr);
    uint8_t rs2_tmp = RS2(_instr);
    */

    //defaults
    _rd   = 0;
    _rs1  = 0;
    _rs2  = 0;
    _imm  = 0;

    if(op == OPC_U_LUI)
    {
        _rd = RD(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_U(_instr);
        _itype = U_Type;
        _op = LUI;
        _write = true;
        _memory = false;
    }
    else
    if(op == OPC_U_AUIPC)
    {
        _rd = RD(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_U(_instr);
        _itype = U_Type;
        _op = AUIPC;
        _write = true;
        _memory = false;
    }
    else
    if(op == OPC_B_BRANCHES)
    {
        _rs1 = RS1(_instr);
        _rs2 = RS2(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_B(_instr);
        _itype = B_Type;
        _write = false;
        _memory = false;
        switch(func3)
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
    if(op == OPC_I_LOADS)
    {
        _rd = RD(_instr);
        _rs1 = RS1(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_I(_instr);
        _itype = I_Type;
        _write = true;
        _memory = true;
        switch(func3)
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
    if(op == OPC_S_STORES)
    {
        _rs1 = RS1(_instr);
        _rs2 = RS2(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_S(_instr);
        _itype = S_Type;
        _write = false;
        _memory = true;
        switch(func3)
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
    if(op == OPC_I_REGIMM)
    {
        _rd = RD(_instr);
        _rs1 = RS1(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_I(_instr);
        _itype = I_Type;
        _write = true;
        _memory = false;
        switch(func3)
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
                _imm = RS2(_instr);
                break;

            case 5:
                if(func7)
                    _op = SRLI;
                else
                    _op = SRAI;
                _imm = RS2(_instr);
                break;
        }
    }
    else
    if(op == OPC_R_REGREG)
    {
        _rd = RD(_instr);
        _rs1 = RS1(_instr);
        _rs2 = RS2(_instr);
        _itype = R_Type;
        _write = true;
        _memory = false;
        switch(func3)
        {
            case 0:
                if(func7)
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
                if(func7)
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
    if(op == OPC_J_JAL)
    {
        _rd = RD(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_J(_instr);
        _itype = J_Type;
        _write = true;
        _op = JAL;
        _memory = false;
    }
    else
    if(op == OPC_I_JALR)
    {
        _rd = RD(_instr);
        _rs1 = RS1(_instr);
// cppcheck-suppress shiftTooManyBitsSigned
        _imm = IMM_I(_instr);
        _itype = I_Type;
        _write = true;
        _memory = false;
        if(func3 == 0) //TODO: add trap for invalid _instructions
            _op = JALR;
    }
    else
    if(op == OPC_I_FENCE)
    {
        //NOP - ADDI 0
        _rd = RD(_instr);
        _rs1 = RS1(_instr);
        _imm = 0;
        _itype = I_Type;
        _write = true;
        _memory = false;
        if(func3 == 0)
            _op = ADDI;
    }
    else
    if(op == OPC_I_ENV)
    {
        //NOP - ADDI 0
        _imm = 0;
        _itype = I_Type;
        _write = false;
        _memory = false;
        //technically imm_I == (0, 1) selects between ECALL and EBREAK
        //two types of NOP, so don't care
        if(func3 == 0)
            _op = ADDI;
    }
}
