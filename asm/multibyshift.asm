; mulbyshift: multiply val1 by 26 and store the result in val2
; ======
;       val1 @ 0x600000-600004
;       val2 @ 0x600004-600008
; ======
; Enter your codes: (type 'done:' when done)

mov eax, [0x600000]
shl eax, 1
mov ebx, eax
shl eax, 1
mov ecx, eax
shl eax, 3
mov [0x600004], eax
sub [0x600004], ecx
sub [0x600004], ebx
done:
