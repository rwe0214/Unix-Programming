; swapmem: swap the values in val1 and val2
; ======
;       val1 @ 0x600000-600008
;       val2 @ 0x600008-600010
; ======
; Enter your codes: (type 'done:' when done)

lea eax, [0x600000]
mov ebx, [0x600008]
xor [eax], ebx
xor ebx, [eax]
xor [eax], ebx
mov [0x600008], ebx
done:
