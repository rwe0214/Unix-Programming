from pwn import *
import binascii as b

def xxx(s):
	tmp = ''
	for i in range(len(s)-1):
		if s[i] == ' ' and s[i+1] == ' ':
			continue
		else:
			tmp += s[i]
	tmp += s[-1]
	tmp += '\n'
	ans = ''
	for c in tmp:
		ans += '{:02x}'.format(int(ord(c)))
	return ans

conn = remote('aup.zoolab.org', 2530)
context.clear()
context.arch = 'x86_64'

for i in range(10):
	print(conn.recvuntil(b'> ').decode('utf-8'), end='')
	quiz = conn.recvline(keepends=False)
	print(quiz.decode('utf-8'))
	#convert into binary format
	b_quiz = b.a2b_hex(quiz)

	#disasm
	asm = disasm(b_quiz, byte=0, offset=0)

	#revise output form
	ans = xxx(asm)
	conn.sendline(ans)
	print(conn.recvline(keepends=False).decode('utf-8'))
	if conn.can_recv(timeout=1) == False:
		print('Something went wrong...')
		exit(0)

print(conn.recvline(keepends=False).decode('utf-8'))
conn.close()
