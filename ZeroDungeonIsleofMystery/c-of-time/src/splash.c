#include <pmdsky.h>
#include <cot.h>
#include "splash_extern.h"

// Courtesy of Irdkwia, ported to C using Chesyon's struct documentation
void __attribute__((used)) TitleCardResetBG3(){
    // these technically refer to screen 0, despite the fact that we're working on screen 1 in this non-standard usage. nothing seems to break, so it's probably fine...? idk what these fields do.
    BGP21C_PTR->bgp18_table[0][1].bgp1c_ptr->unk_0x4 = 0;
    BGP21C_PTR->bgp18_table[0][1].bgp1c_ptr->unk_0x8 = 0;
}

// Loosely based on Irdkwia's code
// IMPORTANT: Due to how things are loaded, text will use palette #14 of the BGP file
void __attribute__((used)) SetTitleCardBackground(int floor, int dungeon_id){
    bool use_background = false;
    char path[20];
    // Insert your own logic here! Below is a sample I've written for Zero Dungeon: Isle of Mystery.
    if(floor < 31){
        use_background = true;
        int stratum = _s32_div_f(floor - 1, 5) + 1;
        sprintf(path, "BACK/stratum%d.bgp", stratum);
    }
    // This should probably stay untouched unless you want to take a different approach than the use_background bool
    if(use_background) LoadBackgroundBgp(path, 0, 0, 0, 1, 1); // note: i made this whole patch general purpose and then immediately used it in a non-conventional way lmao. as mentione by splash_extern, the last param would normally be 0 to put it on the bottom screen.
}

int __attribute__((naked)) SetTitleCardBackgroundWrapper(){
    asm("push {lr}");
    asm("mov  r0,r6");
    asm("mov  r1,r7");
    asm("bl   SetTitleCardBackground");
    asm("mov  r0,#1"); // original instruction
    asm("pop  {pc}");
}
