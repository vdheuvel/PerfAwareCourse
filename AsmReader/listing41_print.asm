bits 16

add bx, [BX + SI]
add bx, [BP]
add si, 2
add bp, 2
add cx, 8
add bx, [BP]
add cx, [BX + 2]
add bh, [BP + SI + 4]
add di, [BP + DI + 6]
add [BX + SI], bx
add [BP], bx
add [BP], bx
add [BX + 2], cx
add [BP + SI + 4], bh
add [BP + DI + 6], di
add byte [BX], 34
add word [BP + SI + 1000], 29
add ax, [BP]
add al, [BX + SI]
add ax, bx
add al, ah
add ax, 1000
add al, 226
add al, 9
sub bx, [BX + SI]
sub bx, [BP]
sub si, 2
sub bp, 2
sub cx, 8
sub bx, [BP]
sub cx, [BX + 2]
sub bh, [BP + SI + 4]
sub di, [BP + DI + 6]
sub [BX + SI], bx
sub [BP], bx
sub [BP], bx
sub [BX + 2], cx
sub [BP + SI + 4], bh
sub [BP + DI + 6], di
sub byte [BX], 34
sub word [BX + DI], 29
sub ax, [BP]
sub al, [BX + SI]
sub ax, bx
sub al, ah
sub ax, 1000
sub al, 226
sub al, 9
cmp bx, [BX + SI]
cmp bx, [BP]
cmp si, 2
cmp bp, 2
cmp cx, 8
cmp bx, [BP]
cmp cx, [BX + 2]
cmp bh, [BP + SI + 4]
cmp di, [BP + DI + 6]
cmp [BX + SI], bx
cmp [BP], bx
cmp [BP], bx
cmp [BX + 2], cx
cmp [BP + SI + 4], bh
cmp [BP + DI + 6], di
cmp byte [BX], 34
cmp word [4834], 29
cmp ax, [BP]
cmp al, [BX + SI]
cmp ax, bx
cmp al, ah
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