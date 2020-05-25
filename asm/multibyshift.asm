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
