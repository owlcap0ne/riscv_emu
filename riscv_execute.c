#include "riscv_emu.h"

#define _imm state->imm
#define _rs1_dat state->rs1_dat
#define _rs2_dat state->rs2_dat
#define _rd_dat state->rd_dat
#define _mem_addr state->mem_addr
#define _write state->write
#define _branch_target state->branch_target
#define _branch state->branch
#define _op state->op
#define _pc state->pc

void execute(EmulatorState* state)
{
    _branch = false; //default
    
    switch(_op)
    {
        case LUI:
            _rd_dat = _imm;
            break;

        case AUIPC:
            _rd_dat = _pc + _imm;
            break;

        case JAL:
            _rd_dat = _pc + 4;
            _branch_target = _pc + _imm; //TODO: target addr unaligned exception
            _branch = true;
            break;

        case JALR:
            _rd_dat = _pc + 4;
            _branch_target = (_rs1_dat + _imm) & 0xFFFFFFFE; //delete LSB
            _branch = true;
            break;


        case BEQ:
            _branch_target = _pc + _imm;
            _branch = _rs1_dat == _rs2_dat;
            break;

        case BNE:
            _branch_target = _pc + _imm;
            _branch = _rs1_dat != _rs2_dat;
            break;

        case BLT:
            _branch_target = _pc + _imm;
            _branch = _rs1_dat < _rs2_dat;
            break;

        case BGE:
            _branch_target = _pc + _imm;
            _branch = _rs1_dat >= _rs2_dat;
            break;

        case BLTU:
            _branch_target = _pc + _imm;
            _branch = (uint32_t) _rs1_dat < (uint32_t) _rs2_dat;
            break;

        case BGEU:
            _branch_target = _pc + _imm;
            _branch = (uint32_t) _rs1_dat >= (uint32_t) _rs2_dat;
            break;

        case LB:
            _mem_addr = _rs1_dat + _imm;
            break;

        case LH:
            _mem_addr = _rs1_dat + _imm;
            break;

        case LW:
            _mem_addr = _rs1_dat + _imm;
            break;

        case LBU:
            _mem_addr = _rs1_dat + _imm;
            break;

        case LHU:
            _mem_addr = _rs1_dat + _imm;
            break;

        case SB:
            _mem_addr = _rs1_dat + _imm;
            break;

        case SH:
            _mem_addr = _rs1_dat + _imm;
            break;

        case SW:
            _mem_addr = _rs1_dat + _imm;
            break;

        case ADDI:
            _rd_dat = _rs1_dat + _imm;
            break;

        case SLTI:
            _rd_dat = (_rs1_dat < _imm) ? 1 : 0;
            break;

        case SLTIU:
            _rd_dat = ((uint32_t) _rs1_dat < (uint32_t) _imm) ? 1 : 0;
            break;

        case XORI:
            _rd_dat = _rs1_dat ^ _imm;
            break;

        case ORI:
            _rd_dat = _rs1_dat | _imm;
            break;

        case ANDI:
            _rd_dat = _rs1_dat & _imm;
            break;

        case SLLI:
            // cast of shift ammount to unsigned probably not neccessary...
            _rd_dat = _rs1_dat << (uint32_t) _imm;
            break;

        case SRLI:
            _rd_dat = (uint32_t) _rs1_dat >> (uint32_t) _imm;
            break;

        case SRAI:
            _rd_dat = _rs1_dat >> (uint32_t) _imm;
            break;

        case ADD:
            _rd_dat = _rs1_dat + _rs2_dat;
            break;

        case SUB:
            _rd_dat = _rs1_dat - _rs2_dat;
            break;

        case SLL:
            _rd_dat = _rs1_dat << (_rs2_dat & 0x1F);
            break;

        case SLT:
            _rd_dat = (_rs1_dat < _rs2_dat) ? 1 : 0;
            break;

        case SLTU:
            _rd_dat = ((uint32_t) _rs1_dat < (uint32_t) _rs2_dat) ? 1 : 0;
            break;

        case XOR:
            _rd_dat = _rs1_dat ^ _rs2_dat;
            break;

        case SRL:
            _rd_dat = (uint32_t) _rs1_dat >> (_rs2_dat & 0x1F);
            break;

        case SRA:
            _rd_dat = _rs1_dat >> (_rs2_dat & 0x1F);
            break;

        case OR:
            _rd_dat = _rs1_dat | _rs2_dat;
            break;

        case AND:
            _rd_dat = _rs1_dat & _rs2_dat;
            break;

        case FENCE:
            //NOP
            //_rd_dat = _rs1_dat + _imm;
            break;

        case ECALL:
            //NOP
            //_rd_dat = _rs1_dat + _imm:
            break;

        case EBREAK:
            //NOP
            //_rd_dat = _rs1_dat + _imm;
            break;
    }
    return;
}
