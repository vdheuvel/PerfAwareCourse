BITS 64

global CacheTestASM

section .text

CacheTestASM:
  xor rax, rax
.loop:
  mov r9, rcx ; create a copy
  and r9, r8 ; apply mask 
  add r9, rdx ; add array start
  vmovdqu32 zmm0, [r9] ; load
  vmovdqu32 zmm1, [r9]
  sub rcx, 128
  jg .loop
  ret