BITS 64

global Load_128x1ASM
global Load_128x2ASM
global Load_256x1ASM
global Load_256x2ASM
global Load_512x1ASM
global Load_512x2ASM


section .text

Load_128x1ASM:
.loop:
  movdqu xmm0, [rdx]
  sub rcx, 2
  jg .loop
  ret
  
Load_128x2ASM:
.loop:
  movdqu xmm0, [rdx]
  movdqu xmm0, [rdx]
  sub rcx, 4 
  jg .loop
  ret
  
Load_256x1ASM:
.loop:
  vmovdqu ymm0, [rdx]
  sub rcx, 4
  jg .loop
  ret
  
Load_256x2ASM:
.loop:
  vmovdqu ymm0, [rdx]
  vmovdqu ymm0, [rdx]
  sub rcx, 8 
  jg .loop
  ret
  
Load_512x1ASM:
.loop:
  vmovdqu32 zmm0, [rdx]
  sub rcx, 8
  jg .loop
  ret
  
Load_512x2ASM:
.loop:
  vmovdqu32 zmm0, [rdx]
  vmovdqu32 zmm0, [rdx]
  sub rcx, 16 
  jg .loop
  ret