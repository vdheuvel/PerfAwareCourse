BITS 64

global Load_1xASM
global Load_2xASM
global Load_3xASM
global Load_4xASM
global Store_1xASM
global Store_2xASM
global Store_3xASM
global Store_4xASM

section .text

Load_1xASM:
  xor rax, rax
.loop:
  mov rax, [rdx]
  sub rcx, 1
  jg .loop
  ret
  
Load_2xASM:
  xor rax, rax
.loop:
  mov rax, [rdx]
  mov rax, [rdx]
  sub rcx, 2 
  jg .loop
  ret
  
Load_3xASM:
  xor rax, rax
.loop:
  mov rax, [rdx]
  mov rax, [rdx]
  mov rax, [rdx]
  sub rcx, 3
  jg .loop
  ret
  
Load_4xASM:
  xor rax, rax
.loop:
  mov rax, [rdx]
  mov rax, [rdx]
  mov rax, [rdx]
  mov rax, [rdx]
  sub rcx, 4
  jg .loop
  ret
  
Store_1xASM:
  xor rax, rax
.loop:
  mov [rdx], rax
  sub rcx, 1
  jg .loop
  ret
  
Store_2xASM:
  xor rax, rax
.loop:
  mov [rdx], rax
  mov [rdx], rax
  sub rcx, 2 
  jg .loop
  ret
  
Store_3xASM:
  xor rax, rax
.loop:
  mov [rdx], rax
  mov [rdx], rax
  mov [rdx], rax
  sub rcx, 3
  jg .loop
  ret
  
Store_4xASM:
  xor rax, rax
.loop:
  mov [rdx], rax
  mov [rdx], rax
  mov [rdx], rax
  mov [rdx], rax
  sub rcx, 4
  jg .loop
  ret