; clear17: clear bit-17 in eax (zero-based index)
; ======
; ======
; Enter your codes: (type 'done:' when done)

mov ebx, 1
shl ebx, 17
not ebx
and eax, ebx
done:
