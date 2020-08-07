
/*
 Imm Variants:
    Bit 31 is always the sign bit and extended as needed
    X means zero extend
 __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
|32|31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10| 9| 8| 7| 6| 5| 4| 3| 2| 1|
 __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
|31                                                            |30:25            |24:21      |20| I-Type

 __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
|31                                                            |30:25            |11:8       | 7| S-Type

 __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
|31                                                         | 7|30:25            |11:8       | X| B-Type

 __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
|31|30:20                           |19:12                  |X                                  | U-Type

 __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
|31                                 |19:12                  |20|30:25            |24:21      | X| J-Type

*/
#define MASK_IMM_31         (0x1    << 31)  //Sign bit, used by all imm variants
                                            //I                                 offset 11,
                                            //S                                 offset 11,
                                            //B                                 offset 12,
                                            //U                                 offset 31,
                                            //J                                 offset 20

#define MASK_IMM_30_25      (0x3F   << 25)  //Used by I, S, B and J variants,   offset 5

#define MASK_IMM_24_21      (0xF    << 21)  //Used by I and J,                  offset 1

#define MASK_IMM_20         (0x1    << 20)  //Used by I,                        offset 0
                                            //Used by J,                        offset 11

#define MASK_IMM_11_8       (0xF    <<  8)  //Used by S and B,                  offset 1

#define MASK_IMM_7          (0x1    <<  7)  //Used by S,                        offset 0
                                            //Used by B,                        offset 11

#define MASK_IMM_30_20      (0x7FF  << 20)  //Used by U,                        offset 20

#define MASK_IMM_19_12      (0xFF   << 12)  //Used by U and J,                  offset 12


//Welcome to macro hell!
//puzzle together imm's from components, then shift left until the sign bit is in the MSB position,
//shift right again to sign extend, assuming the input type is a signed int (int32_t plz...)

#define imm_I(instr) ( ((((instr & MASK_IMM_31)    << 11) \
                        |((instr & MASK_IMM_30_25) <<  5) \
                        |((instr & MASK_IMM_24_21) <<  1) \
                        | (instr & MASK_IMM_20)) \
                        << 20) >> 20)

#define imm_S(instr) ( ((((instr & MASK_IMM_31)    << 11) \
                        |((instr & MASK_IMM_30_25) <<  5) \
                        |((instr & MASK_IMM_11_8)  <<  1) \
                        | (instr & MASK_IMM_7)) \
                        << 20) >> 20)

#define imm_B(instr) ( ((((instr & MASK_IMM_31)    << 12) \
                        |((instr & MASK_IMM_7)     << 11) \
                        |((instr & MASK_IMM_30_25) <<  5) \
                        |((instr & MASK_IMM_11_8)  <<  1))\
                        << 19) >> 18)

#define imm_U(instr) (   ((instr & MASK_IMM_31)    << 31) \
                        |((instr & MASK_IMM_30_20) << 20) \
                        |((instr & MASK_IMM_19_12) << 12))

#define imm_J(instr) ( ((((instr & MASK_IMM_31)    << 20) \
                        |((instr & MASK_IMM_19_12) << 12) \
                        |((instr & MASK_IMM_20)    << 11) \
                        |((instr & MASK_IMM_30_25) <<  5) \
                        |((instr & MASK_IMM_24_21) <<  1))\
                        << 11) >> 10)

#define MASK_FUNC3      (0x7    << 12)
#define MASK_FUNC7      (0x7F   << 25)
#define MASK_RS1        (0x1F   << 15)
#define MASK_RS2        (0x1F   << 20)
#define MASK_RD         (0x1F   <<  7)
#define MASK_OP         (0x7F)

#define func3(instr) ((instr & MASK_FUNC3) >> 12)
#define func7(instr) ((instr & MASK_FUNC7) >> 25)
#define rs1(instr)   ((instr & MASK_RS1)   >> 15)
#define rs2(instr)   ((instr & MASK_RS2)   >> 20)
#define rd(instr)    ((instr & MASK_RD)    >>  7)
#define op(instr)     (instr & MASK_OP)

//opcodes
#define OPC_B_BRANCHES  0x63
//0b1100011
#define OPC_I_LOADS     0x03
//0b0000011
#define OPC_S_STORES    0x23
//0b0100011
#define OPC_I_REGIMM    0x13
//0b0010011
#define OPC_R_REGREG    0x33
//0b0110011
#define OPC_U_LUI       0x37
//0b0110111
#define OPC_U_AUIPC     0x17
//0b0010111
#define OPC_J_JAL       0x6F
//0b1101111
#define OPC_I_JALR      0x67
//0b1100111
#define OPC_I_FENCE     0x0F
//0b0001111
#define OPC_I_ENV       0x73
//0b1110011
