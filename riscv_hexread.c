#include "riscv_emu.h"
#include <stdio.h>
#include <string.h>

int hexread(EmulatorState* state, const char* hexfile)
{
    FILE *fp = fopen(hexfile, "r");
    if(fp == NULL)
    {
        perror("Unable to open File");
        return -1;
    }

    char chunk[523]; //worst case line length: 510 data chars and Windows file
    unsigned int str_len;
    char hex_len[3];
    char hex_addr[5];
    char hex_type[3];
    char hex_data[3];
    char hex_chksum[3];
    unsigned char len       = 0,
                  type      = 0,
                  data      = 0,
                  chksum    = 0;
    unsigned int addr       = 0,
                 base_addr  = 0,
                 sum        = 0;

    //needed by strtol, ignored for now
    char *endptr;

    while(fgets(chunk, sizeof(chunk), fp) != NULL)
    {
        str_len = strlen(chunk);
        //shortest possible line is EOF with 11 chars excluding whitespaces
        if((chunk[0] != ':') || (str_len < 11))
        {
            return -1;
        }
        strncpy(hex_len, chunk +1, 2);
        strncpy(hex_addr, chunk +3, 4);
        strncpy(hex_type, chunk +7, 2);
        len = (unsigned char) strtol(hex_len, &endptr, 16);
        sum = len;
        strncpy(hex_chksum, chunk +9+(len*2), 2);
        hex_chksum[2] = 0; //just in case
        chksum = (unsigned char) strtol(hex_chksum, &endptr, 16);
        type = (unsigned char) strtol(hex_type, &endptr, 16);
        sum += type;
        addr = (unsigned int) strtol(hex_addr, &endptr, 16);
        sum += (addr >> 8) & 0xFF; //upper byte
        sum += addr & 0xFF;        //lower byte

        switch(type)
        {
            case 0x0: // data
                addr = addr + base_addr;
                for(int n = 0; n < len*2; n += 2)
                {
                    if(addr > state->mem_size)
                    {
                        return -1;
                    }
                    strncpy(hex_data, chunk +9+n, 2);
                    data = (unsigned char) strtol(hex_data, &endptr, 16);
                    sum += data;
                    state->mem[addr] = data;
                    addr++;
                }

                if((unsigned char) ((~sum & 0xFF) +1) != chksum)
                {
                    return -1;
                }
            break;

            case 0x1: // EOF
                sum = ((~sum) & 0xFF) +1;
                if((unsigned char) sum != chksum)
                {
                    return -1;
                }
            break;

            case 0x2: // extended segment addr
              // sanity check
              if(len != 0x02)
              {
                return -1;
              }

              for(int n = 0; n < 4; n += 2)
              {
                strncpy(hex_data, chunk +9+n, 2);
                data = strtol(hex_data, &endptr, 16);
                sum += data;
                base_addr = (n < 2)? (data << 8) : (base_addr | data);
              }
              base_addr = base_addr << 4;

              sum = (~sum) & 0xFF;
              if((unsigned char) (sum +1) != chksum)
              {
                  return -1;
              }
            break;

            case 0x3: // start segment addr
            // 80x86 processor specific, ignored
            break;

            case 0x4: // extended linear addr
                //copy-paste of type 0x02...
                if(len != 0x02)
                {
                    return -1;
                }

                for(int n = 0; n < 4; n += 2)
                {
                    strncpy(hex_data, chunk +9+n, 2);
                    data = strtol(hex_data, &endptr, 16);
                    sum += data;
                    base_addr = (n < 2)? (data << 8) : (base_addr | data);
                }
                base_addr = base_addr << 16;

                sum = (~sum) & 0xFF;
                if((unsigned char) (sum +1) != chksum)
                {
                    return -1;
                }
            break;

            case 0x5: // start linear addr
            // 80x86 processor specific, apparently used to pass initial PC...
                if(len != 0x04)
                {
                    return -1;
                }

                uint32_t pc = 0;
                for(int n = 0; n < 8; n += 2)
                {
                    //left shift data by 24, 16, 8, 0
                    int shamt = 24 - 4*n;
                    strncpy(hex_data, chunk +9+n, 2);
                    data = strtol(hex_data, &endptr, 16);
                    sum += data;
                    pc = pc | (data << shamt);
                }
                state->pc = pc;

                sum = (~sum) & 0xFF;
                if((unsigned char) (sum +1) != chksum)
                {
                    return -1;
                }
            break;

            default: // unknown type, abort
            break;
        }
    }
    return 0;
}
