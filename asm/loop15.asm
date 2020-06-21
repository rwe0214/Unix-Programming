; loop15:
;     str1 is a string contains 15 lowercase and uppercase alphbets.
;     implement a loop to convert all alplabets to lowercase,
;     and store the result in str2.
; ======
;       str1 @ 0x600000-600010
;       str2 @ 0x600010-600020
; ======
; Enter your codes: (type 'done:' when done)

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
