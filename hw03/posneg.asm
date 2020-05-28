mov rdi, 1
mov rsi, -1
cmp eax, 0
jge l1
jmp l2
l1:
	mov [0x600000], rdi
    jmp l3
l2:
	mov [0x600000], rsi
l3:
    cmp ebx, 0
    jge a1
    jmp a2
a1:
	mov [0x600004], rdi
    jmp a3
a2:
	mov [0x600004], rsi
a3:
    cmp ecx, 0
    jpe b1
    jmp b2
b1:
	mov [0x600008], rdi
    jmp b3
b2:
	mov [0x600008], rsi
b3:
    cmp edx, 0
    jpe c1
    jmp c2
c1:
	mov [0x60000c], rdi
    jmp c3
c2:
	mov [0x60000c], rsi
c3:

done: