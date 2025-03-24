0x0	0x01400E13	addi x28 x0 20	addi x28,x0,20
0x4	0x10000EB7	lui x29 65536	li x29,0x10000100
0x8	0x100E8E93	addi x29 x29 256	li x29,0x10000100
0xc	0x05C05463	bge x0 x28 72	blez x28,exit # if n <= 0, just directly exit
0x10	0x00000293	addi x5 x0 0	li x5,0 # load register x5 with immediate value 0
0x14	0x005EA023	sw x5 0(x29)	sw x5,0(x29) # store immediate value in x5 to address pointed by x29, i.e. f(1) = 0
0x18	0xFFFE0E13	addi x28 x28 -1	addi x28,x28,-1 # decrement n by 1, as f(1) is stored now
0x1c	0x020E0C63	beq x28 x0 56	beqz x28,exit # if n was initially 1, now it's 0. i.e., only f(1) was to be stored that is done and now we can exit
0x20	0x00100313	addi x6 x0 1	li x6,1 # load register x6 with immediate value 1
0x24	0x006EA223	sw x6 4(x29)	sw x6,4(x29) # store immediate value in x6 to address pointed by x29 +4, i.e. f(2) = 1
0x28	0xFFFE0E13	addi x28 x28 -1	addi x28,x28,-1 # decrement n by 1 again, as f(2) is stored now
0x2c	0x020E0463	beq x28 x0 40	beqz x28,exit # if n was initially 2, now it's 0. i.e., only till f(2) was to be stored that is done and now we can exit
0x30	0x000E1263	bne x28 x0 4	bnez x28,fibonacci # if still n>0, jump to fibonacci and execute
0x34	0x000EA283	lw x5 0(x29)	lw x5,0(x29) # load f(i-2) to x5
0x38	0x004EA303	lw x6 4(x29)	lw x6,4(x29) # load f(i-1) to x6
0x3c	0x005303B3	add x7 x6 x5	add x7,x6,x5 # x7 = x6 + x5 || f(i) = f(i-1) + f(i-2)
0x40	0x007EA423	sw x7 8(x29)	sw x7,8(x29) # store f(i) to ptr + 2 mem_adr,
0x44	0xFFFE0E13	addi x28 x28 -1	addi x28,x28,-1 # decrement n by 1
0x48	0x004E8E93	addi x29 x29 4	addi x29,x29,4 # increment pointer by 1
0x4c	0xFE0E14E3	bne x28 x0 -24	bnez x28,fibonacci # if still n>0, jump to fibonacci and execute
0x50	0x004000EF	jal x1 4	jal exit # if n=0, exit