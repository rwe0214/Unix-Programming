mov eax, [0x600000]
mov ebx, 5
mul ebx
neg eax

mov ecx, eax
mov eax, [0x600004]
neg eax
mov ebx, [0x600008]
cdq
idiv ebx
mov ebx, edx

mov eax, ecx
cdq
idiv ebx
mov [0x60000c], eax

done:
