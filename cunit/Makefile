CC = gcc
CFLAGS = -Wall -g -std=c23
LIBS = -lcunit -lncurses
SRCS = riscv_test_registry.c ../riscv_emu.c ../riscv_pc.c ../riscv_ui.c ../riscv_fetch.c ../riscv_decode.c ../riscv_memory.c ../riscv_execute.c ../riscv_hexread.c ../riscv_regfile.c
OBJS = $(SRCS: .c=.o)
MAIN = riscv_test.bin

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o *~ $(MAIN)
