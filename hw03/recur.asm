mov rcx, 27
call r
jmp exit

r:
    cmp rcx, 1  
    jg l3  ; greater than 1

    cmp rcx, 0  ; greater than 0
    jg l2

    mov rax, 0
    ret

    l2:  ; 1
        mov rax, 1
        ret
    l3:  ; > 1

        sub rcx, 2
        call r
        add rcx, 2

        mov rdx, rax
        add rdx, rax
        add rdx, rax

        push rdx

        ;-----------------

        sub rcx, 1
        call r
        add rcx, 1

        mov rbx, rax
        add rbx, rax
        mov rax, rbx
        
        pop rdx
        add rax, rdx

        ret

exit:
done: