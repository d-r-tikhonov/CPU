push 1
pop [0]
inp
pop rax
push 1
pop rbx
push 1
pop rcx
0:
push rcx
push rbx
add
push [0]
mul
pop [0]
push rcx
push 1
add
pop rcx
push rcx
push rax
jne :0
push [0]
out
hlt
