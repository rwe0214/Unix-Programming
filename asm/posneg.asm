cmp eax, 0
jge set1
mov edi, 1
neg edi
mov [0x600000], edi
jmp next1
set1:
mov edi, 1
mov [0x600000], edi

next1:
cmp ebx, 0
jge set2
mov edi, 1
neg edi
mov [0x600004], edi
jmp next2
set2:
mov edi, 1
mov [0x600004], edi

next2:
cmp ecx, 0
jge set3
mov edi, 1
neg edi
mov [0x600008], edi
jmp next3
set3:
mov edi, 1
mov [0x600008], edi

next3:
cmp edx, 0
jge set4
mov edi, 1
neg edi
mov [0x60000c], edi
jmp end
set4:
mov edi, 1
mov [0x60000c], edi
end:
done:
