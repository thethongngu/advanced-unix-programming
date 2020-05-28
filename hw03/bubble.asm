mov eax, 0

l1:
    cmp eax, 10
    jge l2

    mov ebx, eax
    add ebx, 1

    l5:
        cmp ebx, 10
        jge l3

        mov edi, [0x600000 + eax * 4]
        mov esi, [0x600000 + ebx * 4]

        cmp edi, esi
        jle l4
        
        mov [0x600000 + eax * 4], esi
        mov [0x600000 + ebx * 4], edi

        l4:
            add ebx, 1
            jmp l5


    l3:
        add eax, 1
        jmp l1
l2:
done: