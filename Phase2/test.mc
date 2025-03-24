0x0	0x06400213	addi x4 x0 100	addi x4,x0,100 # Load address 0x1000 into x4
0x4	0x00500093	addi x1 x0 5	addi x1,x0,5
0x8	0x00122023	sw x1 0(x4)	sw x1, 0(x4) # Store x1 (10) at memory location 0x1000
0xc	0x00022383	lw x7 0(x4)	lw x7, 0(x4) # Load value from 0x1000 into x7
0x10	0x00209463	bne x1 x2 8	bne x1, x2, branch_skipped
0x14	0x06400413	addi x8 x0 100	li x8, 100 # If branch is taken, load 100 into x8
0x18	0x1F400513	addi x10 x0 500	addi x10,x0,500 # Jump to return address (from JAL)