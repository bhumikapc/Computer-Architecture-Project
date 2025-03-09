.data
var3: .asciz "abcdef"

.text
add: add x11, x12, sp
sub sp, x1, x8
addi x3, x2, -40
ori x5,x6, 0x40
lw x7, 40, x8
lw x5, 10001000, x9
andi x10, x10, -0x40
loop:
lui x1, 0x200
