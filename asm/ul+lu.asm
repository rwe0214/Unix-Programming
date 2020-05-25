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
