push 1000
pop rex

inp
pop rax
inp
pop rbx
inp
pop rcx


push rax
push 0
jne :7

push rbx
push 0
jne :8

push rcx
push 0
jne :9

INF
jmp :0

9:
NO_SOL
jmp :0

8:
push rcx
push -1
mul
push rex
mul
push rbx
div
pop rfx

6:
push rfx
push 0
ja :1

push -1
push rfx
mul

push -1
mul
pop rfx
1:
push rfx
push rfx
push rex
div
pop [0]
push [0]
push rex
mul
sub
push [0]
ROOT_PR
jmp :0

7:
push rbx
push rbx
mul
push 4
push rax
mul
push rcx
mul
sub
push rex
push rex
mul
mul
pop rdx
push rdx
push 0
jb :9

push rbx
push -1
mul
push rex
mul
push 2
push rax
mul
div
pop rfx
push rdx
push 0
je :6

push rfx
push rdx
SQRT
push 2
push rax
mul
div
pop rdx
push rdx
add
pop rgx
push rgx
push 0
ja :2
push -1
push rgx
mul
push -1
mul
pop rgx
2:
push rgx
push rgx
push rex
div
pop [0]
push [0]
push rex
mul
sub
push [0]
ROOT_PR
push rfx
push rdx
sub
pop rgx
push rgx
push 0
ja :3
push -1
push rgx
mul
push -1
mul
pop rgx
3:
push rgx
push rgx
push rex
div
pop [0]
push [0]
push rex
mul
sub
push [0]
ROOT_PR

0:
hlt
