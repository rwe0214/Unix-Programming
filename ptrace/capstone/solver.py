from pwn import *
import binascii as b
from capstone import *

def xxx(s):
	ans = ''
	for c in s:
		ans += '{:02x}'.format(int(ord(c)))
	return ans

conn = remote('aup.zoolab.org', 2530)
md = Cs(CS_ARCH_X86, CS_MODE_64)

for i in range(10):
	conn.recvuntil(b'> ')
	quiz = conn.recvline(keepends=False)
	
	#convert into binary format
	b_quiz = b.a2b_hex(quiz)

	#disasm
	asm = ''
	for inst in md.disasm(b_quiz, 0):
		asm += (inst.mnemonic + ' ' + inst.op_str + '\n')

	#revise output form
	ans = xxx(asm)
	conn.sendline(ans)
	print(conn.recvline(keepends=False).decode('utf-8'))

print(conn.recvline(keepends=False).decode('utf-8'))
conn.close()
