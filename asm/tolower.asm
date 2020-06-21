; tolower: convert the single character in val1 to uppercase and store in val2
; ======
;       val1 @ 0x600000-600001
;       val2 @ 0x600001-600002
; ======
; Enter your codes: (type 'done:' when done)

mov eax, [0x600000]
sub eax, 32
mov [0x600001], eax
done:
