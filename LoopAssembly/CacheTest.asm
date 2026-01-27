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
  vmovdqu32 zmm1, [r9+64]
  vmovdqu32 zmm2, [r9+128]
  vmovdqu32 zmm3, [r9+192]
  sub rcx, 256
  jg .loop
  ret