from pwn import *
import binascii as b
from capstone import *

def to_cmd(quiz):
    return Cs(CS_ARCH_X86, CS_MODE_64).disasm(b.a2b_hex(quiz), 0x0)

def to_ascii(s):
    return b.b2a_hex(s.encode('utf-8'))


conn = remote('aup.zoolab.org',2530)
conn.recvline()
conn.recvline()
conn.recvline()
conn.recvline()
conn.recvline()
conn.recvline()
conn.recvline()

while True:
    s = conn.recvuntil('Your answer: ').decode('utf-8').split('\n')
    quiz = s[0].split(' ')[1]
    
    res = ''
    lines = to_cmd(quiz)
    for line in lines:
        res += line.mnemonic + ' ' + line.op_str + '\n'

    acs = to_ascii(res)
    conn.sendline(acs)

    print('quiz = ', quiz)
    print('answer = ', acs)
    print('---------------------------------------------------------')

    conn.recvline()

    flag = conn.recvline().decode('utf-8')
    if flag[:4] == 'Flag':
        print(flag)
        break


    

