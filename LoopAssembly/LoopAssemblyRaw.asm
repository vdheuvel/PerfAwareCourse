BITS 64

global MoveAllBytesASM
global NoOpASM
global CmpASM
global DecASM

section .text

MoveAllBytesASM:
  xor rax, rax
.loop:
  mov [rdx+rax],cl
  inc rax  
  cmp rax,rcx
  jb .loop
  ret

NoOpASM:
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