; recur: implement a recursive function

;    r(n) = 0, if n <= 0
;         = 1, if n == 1
;         = 2*r(n-1) + 3*r(n-2), otherwise
   
;    please call r(19) and store the result in RAX
; ======
; ======
; Enter your codes: (type 'done:' when done)

	call  r
	jmp   EXIT

r:
	push  rbx
	cmp   rbx, 0
	jz    ret0
	cmp   rbx, 1
	jz    ret1
	jmp   L1
ret0:
	mov   rax, 0
	pop   rbx
	ret
ret1:
	mov   rax, 1
	pop   rbx
	ret
L1:
	sub   rbx, 1
	call  r
	mov   ecx, 2
	mul   ecx
	sub   rbx, 1
	push  rax
	call  r
	mov   ecx, 3
	mul   ecx
	pop   rdx
	add   rax, rdx
	pop   rbx
	ret

EXIT:
done:
