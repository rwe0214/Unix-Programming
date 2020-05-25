lea eax, [0x600000]
mov ebx, [0x600008]
xor [eax], ebx
xor ebx, [eax]
xor [eax], ebx
mov [0x600008], ebx
done:
