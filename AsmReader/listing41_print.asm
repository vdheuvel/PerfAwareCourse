bits 16

add BX, [BX + SI]
add BX, [BP]
add SI, 2
add BP, 2
add CX, 8
add BX, [BP]
add CX, [BX + 2]
add BH, [BP + SI + 4]
add DI, [BP + DI + 6]
add [BX + SI], BX
add [BP], BX
add [BP], BX
add [BX + 2], CX
add [BP + SI + 4], BH
add [BP + DI + 6], DI
add byte [BX], 34
add word [BP + SI + 1000], 29
add AX, [BP]
add AL, [BX + SI]
add AX, BX
add AL, AH
add ax, 1000
add al, 226
add al, 9
sub BX, [BX + SI]
sub BX, [BP]
sub SI, 2
sub BP, 2
sub CX, 8
sub BX, [BP]
sub CX, [BX + 2]
sub BH, [BP + SI + 4]
sub DI, [BP + DI + 6]
sub [BX + SI], BX
sub [BP], BX
sub [BP], BX
sub [BX + 2], CX
sub [BP + SI + 4], BH
sub [BP + DI + 6], DI
sub byte [BX], 34
sub word [BX + DI], 29
sub AX, [BP]
sub AL, [BX + SI]
sub AX, BX
sub AL, AH
sub ax, 1000
sub al, 226
sub al, 9
cmp BX, [BX + SI]
cmp BX, [BP]
cmp SI, 2
cmp BP, 2
cmp CX, 8
cmp BX, [BP]
cmp CX, [BX + 2]
cmp BH, [BP + SI + 4]
cmp DI, [BP + DI + 6]
cmp [BX + SI], BX
cmp [BP], BX
cmp [BP], BX
cmp [BX + 2], CX
cmp [BP + SI + 4], BH
cmp [BP + DI + 6], DI
cmp byte [BX], 34
cmp word [BP], 29
cmp AX, [BP]
cmp AL, [BX + SI]
cmp AX, BX
cmp AL, AH
cmp ax, 1000
cmp al, 226
cmp al, 9
jnz  $4
jnz  $-2
jnz  $-4
jnz  $-2
je  $+0
jl  $-2
jle  $-4
jb  $-6
jbe  $-8
jp  $-10
jo  $-12
js  $-14
jnz  $-16
jnl  $-18
jg  $-20
jnb  $-22
ja  $-24
jnp  $-26
jno  $-28
jns  $-30
loop  $-32
loopz  $-34
loopnz  $-36
jcxz  $-38