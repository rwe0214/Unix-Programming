mov ecx, 15
lea eax, [0x600000]
lea ebx, [0x600010]

l1:
mov edx, [eax]
cmp dl, 90
jg next
add dl, 32

next:
mov [ebx], dl
inc eax
inc ebx
loop l1
done:
