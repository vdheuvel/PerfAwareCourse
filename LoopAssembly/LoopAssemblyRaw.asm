BITS 64

global MoveAllBytesASM
global NoOp3x1ASM
global CmpASM
global DecASM
global NoOp1x3ASM
global NoOp1x9ASM
global NoOp3x1CondASM

section .text

MoveAllBytesASM:
  xor rax, rax
.loop:
  mov [rdx+rax],cl
  inc rax  
  cmp rax,rcx
  jb .loop
  ret

NoOp3x1ASM:
  xor rax, rax
.loop:
  db 0x0f, 0x1f, 0x00; NOP instruction (3-byte)
  inc rax  
  cmp rax,rcx
  jb .loop
  ret

CmpASM:
  xor rax, rax
.loop:
  inc rax  
  cmp rax,rcx
  jb .loop
  ret

DecASM:
  xor rax, rax
.loop:
  dec rcx
  jnz .loop
  ret

NoOp1x3ASM:
  xor rax, rax
.loop:
  nop
  nop
  nop
  inc rax  
  cmp rax,rcx
  jb .loop
  ret

NoOp1x9ASM:
  xor rax, rax
.loop:
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  nop
  inc rax  
  cmp rax,rcx
  jb .loop
  ret

NoOp3x1CondASM:
  xor rax, rax
.loop:
  cmp [rdx + rax], 0
  je .skip
  db 0x0f, 0x1f, 0x00; NOP instruction (3-byte)
  .skip:
  inc rax  
  cmp rax,rcx
  jb .loop
  ret