mov ecx, 0
mov eax, 0x600000
mov edx, 0x600010
l1:
    cmp ecx, 15
    jge l2

    mov ebx, [eax]
    or ebx, 32
    mov [edx], ebx

    add eax, 1
    add edx, 1

    add ecx, 1
    jmp l1

l2:
done: