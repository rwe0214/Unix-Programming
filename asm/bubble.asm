	mov edi, 9
	mov	esi, 0
L1:
	cmp esi, edi
	jg	end
	mov	ecx, 9
	lea eax, [0x600000]
L2:
	lea ebx, [eax+4]
	mov edx, [ebx]
	cmp	[eax], edx
	jg 	SWAP
	jmp	NEXT
SWAP:
	xor edx, [eax]
	xor [eax], edx
	xor edx, [eax]
	mov [ebx], edx
NEXT:
	add	eax, 4
	loop L2
	inc	esi
	jmp L1
end:
done:

