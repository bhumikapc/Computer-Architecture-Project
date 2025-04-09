.data
arr: .word 5 7 4 -1 3
n: .word 5
.text
la x30,arr
lw x31,n
li x28,0
li x29,0
addi x31,x31,-1
blez x31,exit
outer_loop:
li x29,0
inner_loop:
slli x5,x29,2
add x5,x5,x30
lw x6,0(x5)
lw x7,4(x5)
ble x6,x7 no_swap
sw x6,4(x5)
sw x7,0(x5)
no_swap:
addi x29,x29,1
blt x29,x31,inner_loop
addi x28,x28,1
ble x28,x31,outer_loop
exit:
