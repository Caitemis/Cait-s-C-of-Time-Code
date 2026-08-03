.nds

.include "symbols.asm"

; Insert description of patch here
; 2022/07/17
; Insert a name

; US
;.definelabel LoadBGP, 0x02052060
;.definelabel BGStruct, 0x020B09B0
;.definelabel Ov10FreeSection, 0x022D9BB0
;.definelabel WaitOneFrame, 0x022E9FE0
;.definelabel HookLoad1, 0x0234834C
;.definelabel HookLoad2, 0x02348368
;.definelabel HookLoad3, 0x02348450

; EU
;.definelabel LoadBGP, 0x02052398
;.definelabel BGStruct, 0x020B12CC
.definelabel Ov10FreeSection, 0x022DA508
;.definelabel WaitOneFrame, 0x022EA990
;.definelabel HookLoad1, 0x02348F4C
;.definelabel HookLoad2, 0x02348F68
;.definelabel HookLoad3, 0x02349050

;.definelabel MAIN, 0
;.definelabel SUB, 1

;.definelabel BG2, 0
;.definelabel BG3, 1

.relativeinclude off

.open "overlay29.bin", overlay29_start
	.org TitleCardHookLoad1
	.area 0xC
		bl TitleCardResetBG3
		nop
		nop
	.endarea
	
	.org TitleCardHookLoad2
	.area 0x4
		bl  SetTitleCardBackgroundWrapper
	.endarea
	.org TitleCardHookLoad3
	.area 0xC
		mov r0,#0x2
		bl  AdvanceFrame ; Wait for complete screen fading
		nop
	.endarea
.close
