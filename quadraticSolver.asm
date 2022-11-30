name copy_coefs_ram_regs 0
name discrim 1
name high_p_parab 2
name count_various_part 3
name CountTwoRoots 4
name CheckDiscrim 5
name CountOneRoot 6
name NoRoots 7
name CheckNULLS 8
name Linear 9
name TwoNulls 10
name PrintInfSol 11


in
copy
pop [0]

in
copy
pop [1]

in
copy
pop [2]

call CheckNULLS
call high_p_parab
call discrim
call CheckDiscrim
call count_various_part
call CountTwoRoots
hlt

lbl count_various_part
    call copy_coefs_ram_regs
    push [3]
    sqrt
    push 2
    push rax
    mul
    div
    pop [5]
    ret

lbl CheckNULLS
    call copy_coefs_ram_regs
    push rax
    push 0
    je Linear
    ret

lbl Linear
    push rbx
    push 0
    je TwoNulls
    call copy_coefs_ram_regs
    push rcx
    push rbx
    push -1
    mul
    div
    out
    hlt
    ret

lbl TwoNulls
    push rcx
    push 0
    je PrintInfSol
    call NoRoots
    ret

lbl PrintInfSol
    push 8888
    out
    hlt
    ret

lbl high_p_parab
    call copy_coefs_ram_regs
    push rbx
    push -1
    mul
    push 2
    push rax
    mul
    div
    pop [4]
    ret

lbl discrim
    call copy_coefs_ram_regs
    push rbx
    copy
    mul
    push -4
    push rax
    push rcx
    mul
    mul
    add
    copy
    pop [3]
    ret


lbl CheckDiscrim
    push [3]
    copy
    copy
    copy
    pop  [3]
    push 0
    ja NoRoots
    push 0
    je CountOneRoot
    ret


lbl NoRoots
    push -69420
    out
    hlt
    ret

lbl copy_coefs_ram_regs
    push [0]
    copy
    pop [0]
    pop rax

    push [1]
    copy
    pop [1]
    pop rbx

    push [2]
    copy
    pop [2]
    pop rcx
    ret

lbl CountTwoRoots
    push [4]
    copy
    pop [4]
    push [5]
    copy
    pop [5]
    add
    out
    push [4]
    push [5]
    sub
    out
    ret


lbl CountOneRoot
    call high_p_parab
    push [4]
    out
    hlt
    ret
