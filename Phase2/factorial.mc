0x0	0x00A00E13	addi x28 x0 10	addi x28,x0,10
0x4	0x00A00293	addi x5 x0 10	addi x5,x0,10
0x8	0x10000EB7	lui x29 65536	li x29,0x10000100
0xc	0x100E8E93	addi x29 x29 256	li x29,0x10000100
0x10	0x03C05063	bge x0 x28 32	blez x28,n_0 # if n = 0
0x14	0x000E1263	bne x28 x0 4	bnez x28, compute_fact
0x18	0xFFFE0E13	addi x28 x28 -1	addi x28,x28,-1 # x28 will hold current n-1
0x1c	0x000E0663	beq x28 x0 12	beqz x28,store_fact # n-1 = 0, implies now current n=1, we can now store the computed factorial value in x5 to address n_fact and can exit
0x20	0x03C282B3	mul x5 x5 x28	mul x5,x5,x28 # x5 = x5*x28
0x24	0xFE0E1AE3	bne x28 x0 -12	bnez x28,compute_fact
0x28	0x005EA023	sw x5 0(x29)	sw x5,0(x29) # store computed factorial in x5 to address pointed by x29 i.e. n_fact
