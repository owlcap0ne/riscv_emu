ELF = sumArray.elf

test: $(ELF)
	riscv32-unknown-elf-objcopy $(ELF) -O ihex $@.hex

disassemble: $(ELF)
	riscv32-unknown-elf-objdump  -D $(ELF) > test.lss

%.elf: %.o rv32Start.o
	riscv32-unknown-elf-ld -T RV32_emu.lds -o $@ rv32Start.o $<
%.o: %.c
	riscv32-unknown-elf-gcc -Wall -nostdlib -nostartfiles -ffreestanding -o $@ -c $<
rv32Start.o: rv32Start.S
	riscv32-unknown-elf-as -mabi=ilp32 rv32Start.S -o rv32Start.o
