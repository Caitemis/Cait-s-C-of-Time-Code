FloorCardFadeInTop:
    push {r14}
    mov  r0,#0x1
    mov  r1,#0x1000
    mov  r2,#0x1
    bl   StartFadeDungeonWrapper
    mov  r0,#0 // original instruction
    pop  {r15}

FloorCardFadeOutTop:
    push {r14}
    mov  r0,#0x2
    mov  r1,#0x1000
    mov  r2,#0x1
    bl   StartFadeDungeonWrapper
    mov  r0,#0 // original instruction
    pop  {r15}

FloorCardSaveFloorNum:
    mov r1,r2 // original instruction
    mov r3,r2 // save floor num to r3 for later (technically, this is against convention, but r3 gets push/popped by GetApparentDungeonId so it's fine)
    bx  lr
