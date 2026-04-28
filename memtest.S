
memtest.out:     file format ihex


Disassembly of section .sec1:

0000000002000000 <.sec1>:
 2000000:	00010197          	auipc	gp,0x10
 2000004:	80018193          	addi	gp,gp,-2048 # 0x200f800
 2000008:	000082b7          	lui	t0,0x8
 200000c:	00004333          	xor	t1,zero,zero
 2000010:	00504393          	xori	t2,zero,5
 2000014:	76543e37          	lui	t3,0x76543
 2000018:	208e0e13          	addi	t3,t3,520 # 0x76543208
 200001c:	02736263          	bltu	t1,t2,0x2000040
 2000020:	01c2a023          	sw	t3,0(t0) # 0x8000
 2000024:	0002ae83          	lw	t4,0(t0)
 2000028:	00328f03          	lb	t5,3(t0)
 200002c:	0032cf83          	lbu	t6,3(t0)
 2000030:	006e1e33          	sll	t3,t3,t1
 2000034:	01e281a3          	sb	t5,3(t0)
 2000038:	00130313          	addi	t1,t1,1
 200003c:	fe1ff0ef          	jal	ra,0x200001c
 2000040:	00006013          	ori	zero,zero,0
 2000044:	fe000ee3          	beqz	zero,0x2000040
 2000048:	0014                	0x14
 200004a:	0000                	unimp
 200004c:	0000                	unimp
 200004e:	0000                	unimp
 2000050:	7a01                	lui	s4,0xfffe0
 2000052:	0052                	c.slli	zero,0x14
 2000054:	7c01                	lui	s8,0xfffe0
 2000056:	0101                	addi	sp,sp,0
 2000058:	07020d1b          	0x7020d1b
 200005c:	0001                	nop
 200005e:	0000                	unimp
 2000060:	0010                	0x10
 2000062:	0000                	unimp
 2000064:	001c                	0x1c
 2000066:	0000                	unimp
 2000068:	ff98                	fsw	fa4,56(a5)
 200006a:	ffff                	0xffff
 200006c:	0048                	addi	a0,sp,4
 200006e:	0000                	unimp
 2000070:	0000                	unimp
	...
