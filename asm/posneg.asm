; posneg: test if registers are positive or negative.
;     if ( eax >= 0 ) { var1 = 1 } else { var1 = -1 }
;     if ( ebx >= 0 ) { var2 = 1 } else { var2 = -1 }
;     if ( ecx >= 0 ) { var3 = 1 } else { var3 = -1 }
;     if ( edx >= 0 ) { var4 = 1 } else { var4 = -1 } 
; ======
;       var1 @ 0x600000-600004
;       var2 @ 0x600004-600008
;       var3 @ 0x600008-60000c
;       var4 @ 0x60000c-600010
; ======
; Enter your codes: (type 'done:' when done)

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
