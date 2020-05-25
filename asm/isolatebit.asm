mov ebx, eax
and ebx, 0xfe0
shr ebx, 5
mov [0x600000], bl
done:
