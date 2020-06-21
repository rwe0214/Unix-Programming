; isolatebit:
;     get the value bit-11 ~ bit-5 in AX and store the result in val1
;     (zero-based bit index)
; ======
;       val1 @ 0x600000-600001
;       val2 @ 0x600001-600002
; ======
; Enter your codes: (type 'done:' when done)

mov ebx, eax
and ebx, 0xfe0
shr ebx, 5
mov [0x600000], bl
done:
