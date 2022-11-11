name factorial 0
name if_one 1
name end 2
push 6
call factorial
push rcx
out
hlt


lbl factorial
copy
push 1
je if_one

pop
copy
push -1
add
call factorial
push rcx
mul
pop rcx
lbl end
ret


lbl if_one
pop
pop rcx
jump end
