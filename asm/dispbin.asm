mov ecx, 16
lea edx, [0x600000]

l1:
shl ax, 1
mov ebx, 48
jnc noset 
mov ebx, 49

noset:
or [edx], bl
inc edx
loop l1
done:
