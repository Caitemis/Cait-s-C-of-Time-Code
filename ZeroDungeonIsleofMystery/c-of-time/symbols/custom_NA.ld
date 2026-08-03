/* !file overlay11 */
OpcodeCheck = 0x022DDDD4;
GetParameterCount = 0x022DDD8C;
ScriptEngineReturnTwo = 0x022E246C;

/* !file overlay29 */
ApplyMoveEffect = 0x0232E864;
ApplyMoveEffectHookAddr = 0x0232F8A4;
ApplyMoveEffectJumpAddr = 0x023326CC;
ApplyItemEffectHookAddr = 0x0231B9D8;
ApplyItemEffectJumpAddr = 0x0231CB14;

/* Add your own symbols here... */
RefreshStorage = 0x0201007c;

/* Splash screen */
LoadBackgroundBgp = 0x2052060; /* EU: 0x2052398 */
BGP21C_PTR = 0x20B09B0; /* EU: 0x20B12CC */
TitleCardHookLoad1 = 0x234834C; /* EU: 0x2348F4C */
TitleCardHookLoad2 = 0x2348368; /* EU: 0x2348F68 */
TitleCardHookLoad3 = 0x2348450; /* EU: 0x2349050 */

/* Fade in top screen for floor card */
FloorCardFadeInHook = 0x2348378; /* EU: 0x2348F78 */
FloorCardFadeOutHook = 0x234843C; /* EU: 0x234903C */
FloorCardSaveFloorNumHook = 0x204F704; /* EU: 0x204FA3C */
GetStratumNameStringId = 0x204F74C; /* EU: 0x204FA84 */

/* !file arm9 */
ChangeGlobalBorderColor = 0x02027A80;
UppercaseTextTagFatalError = 0x02021A10;
AfterUppercaseTagIsFound = 0x02021AA0;
GetTextSpeed = 0x02020ED0;
DoTextLoop = 0x02020EE0;
AnalyzeTextReturn = 0x02021EC8;

/**
 * @file symbol_NA.ld
 * @author Irdkwia & Adakite
 * @brief SND Stream Library C Edition
 * @details Port of SND Stream Library to C
 * @version 0.8.6
 * @date 2026-02-22
 */
FunctionUnk1 = 0x02002CB4;

SoundMemoryArenaPtr = 0x022A4E54;

EndHookStartBGM = 0x020198D4;
EndHookStartBGM2 = 0x02019970;
EndOfStartBGM = 0x02019AF8;
EndHookStopBGM = 0x02019B34;
EndHookChangeBGM = 0x02019C68;

HookCheckOverlayArm9 = 0x020B0C80;
EndHookSoundProcess = 0x0207103C;
HookChannel1 = 0x020743EC;
HookChannel2 = 0x020743F4;

SetChannelVolume = 0x0207CA24;
SetChannelGlobal = 0x0207CA6C;

ChannelsStruct = 0x022B7A30;

TMR2 = 0x04000108;

os_sleepthread = 0x2079888;
