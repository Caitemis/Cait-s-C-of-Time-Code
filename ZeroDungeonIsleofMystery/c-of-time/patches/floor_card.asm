.nds
.include "symbols.asm"

.open "arm9.bin", arm9_start
	.org FloorCardSaveFloorNumHook
	.area 0x4
		bl FloorCardSaveFloorNum
	.endarea

	.org GetStratumNameStringId
	.area 0x10
        sub r0,r3,#1 ; floor num - 1
        mov r1,#5
        bl _s32_div_f ; stratum = (floor num - 1) / 5
        add r4,r0,#0x1EC ; string = stratum + 492
	.endarea
.close

.open "overlay29.bin", overlay29_start
	.org FloorCardFadeInHook
	.area 0x4
		bl FloorCardFadeInTop
	.endarea

	.org FloorCardFadeOutHook
	.area 0x4
        bl FloorCardFadeOutTop
	.endarea
.close
