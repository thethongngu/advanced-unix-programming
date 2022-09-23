mov rdi, [0x600001]
and ax, 4064
shr ax, 5
mov [0x600000], ax
mov [0x600001], rdi
done: