; ul+lu: convert the alphabet in CH from upper to lower or from lower to upper
; ======
; ======
; Enter your codes: (type 'done:' when done)

cmp ch, 90
jg toupper
or ch, 32
jmp end

toupper:
mov eax, 32
not eax
and ch, al
end:
done:
