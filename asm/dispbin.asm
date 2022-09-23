mov ecx, 0
mov rdi, 0x60000f

l1:
    cmp ecx, 16
    jge l2

    mov bx, ax
    and bx, 1
    cmp bx, 1
    jae a2
    a1:
        mov dh, 48 
        jmp a3
    a2:
        mov dh, 49
        
    a3:

    mov [rdi], dh
    sub rdi, 1
    shr ax

    add ecx, 1
    jmp l1

l2:
done: