; dispbin:
;     given a number in AX, store the corresponding bit string in str1.
;     for example, if AX = 0x1234, the result should be:
;     str1 = 0001001000111000
; ======
;       str1 @ 0x600000-600014
; ======
; Enter your codes: (type 'done:' when done)

mov ecx, 16
lea edx, [0x600000]

l1:
shl ax, 1
mov ebx, 48
jnc noset 
mov ebx, 49

noset:
or [edx], bl
inc edx
loop l1
done:
