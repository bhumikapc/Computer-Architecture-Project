0x0	0x10000F17	auipc x30 65536	auipc x30, 65536
0x4	0x000F0F13	addi x30 x30 0	addi x30, x30, 0
0x8	0x10000F97	auipc x31 65536	auipc x31, 65536
0xc	0x00CFAF83	lw x31 12(x31)	lw x31, 12(x31)
0x10	0x00000E13	addi x28 x0 0	addi x28, x0, 0
0x14	0x00000E93	addi x29 x0 0	addi x29, x0, 0
0x18	0xFFFF8F93	addi x31 x31 -1	addi x31,x31,-1
0x1c	0x03F05A63	bge x0 x31 52	bge x0, x31, exit
0x20	0x00000E93	addi x29 x0 0	addi x29, x0, 0
0x24	0x002E9293	slli x5 x29 2	slli x5,x29,2
0x28	0x01E282B3	add x5 x5 x30	add x5,x5,x30
0x2c	0x0002A303	lw x6 0(x5)	lw x6,0(x5)
0x30	0x0042A383	lw x7 4(x5)	lw x7,4(x5)
0x34	0x0063D663	bge x7 x6 12	bge x7, x6, no_swap
0x38	0x0062A223	sw x6 4(x5)	sw x6, 4(x5)
0x3c	0x0072A023	sw x7 0(x5)	sw x7,0(x5)
0x40	0x001E8E93	addi x29 x29 1	addi x29,x29,1
0x44	0xFFFEC0E3	blt x29 x31 -32	blt x29,x31,inner_loop
0x48	0x001E0E13	addi x28 x28 1	addi x28,x28,1
0x4c	0xFDCFDAE3	bge x31 x28 -44	bge x31, x28, outer_loop
