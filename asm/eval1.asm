mov eax, [0x600000]
neg eax
add eax, [0x600004]
sub eax, [0x600008]
mov [0x60000c], eax
done:
