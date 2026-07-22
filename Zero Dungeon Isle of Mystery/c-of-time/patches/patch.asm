// Replace the "GetMovePower" function with a custom one.
// Since a branch is inserted at the start of the function, the function is practically
// replaced with our own. The "b" instruction doesn't modify the link register, so
// execution will continue after the call to `GetMovePower` once our function returns.

.nds
.include "symbols.asm"
NEW_MAX_MONEY equ 0xF423F

.open "arm9.bin", arm9_start
    // Remove "Can Learn" prompt for partner
    .org 0x02099ca6
        .halfword 0xFFFF
    .org 0x02099cc6
        .halfword 0xFFFF

    // Max Money Limit
    .org 0x0200ed50
        .word NEW_MAX_MONEY
    .org 0x02042a7c
        .word NEW_MAX_MONEY

    // Don't Ever Forget (sealed moves)
    .org 0x02040394
        bl CannotForgetSealedMoves
.close

.open "overlay11.bin", overlay11_start

    .org 0x023076C4 ; TM check
        cmp r5,r15
    .org 0x0230778C ; IsGummi callsite
        mov r0,#0x0

.close

.open "overlay29.bin", overlay29_start
    .org UpdateMovePP
        b CustomUpdateMovePP
    .org 0x0230bbfc ; Before Me First multiplier
    .area 0xC
        mov r2,r10
        ldr r3,[sp,#0xf4]
        nop
    .endarea

    .org 0x02346A34
    .area 0x14
        bic r0,r0,#0x2
        strb r0,[r9]
        nop :: nop :: nop
    .endarea

    .org 0x0230bc14 
    // Me First multiplier
    .area 0x4
        bl CalcMeFirstAndAbilityMultipliers
    .endarea

    .org 0x02310210 
    // Sand Veil check
    .area 0x4
        bl HasSandProtectionAbility
    .endarea

    .org 0x02310188 
    // Snow Cloak check
    .area 0x4
        bl HasHailProtectionAbility
    .endarea

    .org 0x02319b48
        bl TwoTileMoveAiAdjustment
    .org 0x02319b98
        bl TwoTileMoveAiAdjustment
    // Shopkeepers won't blow up
    .org 0x02320ae4
        bl TryExplosionForceMiss
    // No spawning in monster houses or in the stairs room
    .org 0x02341430
    .area 0x24
        mov r1,r5
        mov r2,r13
        bl MarkSpawnableTile
        nop :: nop :: nop :: nop :: nop
    .endarea
.close
