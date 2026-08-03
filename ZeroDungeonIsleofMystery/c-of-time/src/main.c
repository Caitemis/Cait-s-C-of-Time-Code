#include <pmdsky.h>
#include <cot.h>

// Remove the comment in patches/patch.asm to enable this example patch.
// `attribute((used))` is required to prevent the compiler from optimizing out the function
// if it's only used in a patch.

uint16_t text_numerator = 1;
uint16_t text_denominator = 1;
uint16_t text_count = 0;

void ResetTextSpeedValues(void) {
    text_numerator = 1;
    text_denominator = 1;
    text_count = 0;
}

// Don't make shopkeepers go Boom (TM)
__attribute((used)) bool TryExplosionForceMiss(struct entity* target, struct move* move) {
    // There was already a validity check before this code is run, so no need to do it again
    struct monster* target_mon = target->info;
    return target_mon->shopkeeper.val == 0 ? TwoTurnMoveForcedMiss(target, move) : true;
}

// Try to prioritize spawning in a tile that isn't in the same room as the stairs or in a monster house!
__attribute((used)) void MarkSpawnableTile(int valid_tiles_nb, struct dungeon* dungeon, struct spawn_position* spawn_positions) {
    int alloc_size = sizeof(struct spawn_position) * valid_tiles_nb;
    struct spawn_position* stairs_roomies = MemAlloc(alloc_size, 0);
    struct spawn_position* ideal_spawn_positions = MemAlloc(alloc_size, 0);
    struct spawn_position* final_spawn_positions = spawn_positions;
    int final_tile_nb = valid_tiles_nb;
    int stairs_roomie_nb = 0;
    int ideal_nb = 0;
    struct tile* stairs_tile = GetTile(dungeon->gen_info.stairs_pos.x, dungeon->gen_info.stairs_pos.y);
    if (stairs_tile != NULL) {
        int stairs_room = stairs_tile->room;
        // Search all the current valid spawn positions for any ones that aren't in the same room as the stairs...
        for (int i = 0; i < valid_tiles_nb; i++) {
            struct spawn_position current_spawn_pos = spawn_positions[i];
            struct tile* current_tile = GetTile(current_spawn_pos.x, current_spawn_pos.y);
            // Ban Monster House tiles from being valid spawns...
            if (current_tile != NULL && !current_tile->f_in_monster_house) {
                if (stairs_room != current_tile->room) {
                    // Prioritize this tile if it's not in the same room as the stairs!
                    ideal_spawn_positions[ideal_nb].x = current_spawn_pos.x;
                    ideal_spawn_positions[ideal_nb].y = current_spawn_pos.y;
                    ideal_nb++;
                }
                else {
                    // Slightly prioritize this, just in case we don't find any ideal positions.
                    stairs_roomies[stairs_roomie_nb].x = current_spawn_pos.x;
                    stairs_roomies[stairs_roomie_nb].y = current_spawn_pos.y;
                    stairs_roomie_nb++;
                }
            }
        }
        // If we have at least one ideal tile to spawn in, take it!
        if (ideal_nb > 0) {
            final_spawn_positions = ideal_spawn_positions;
            final_tile_nb = ideal_nb;
        }
        // If there is at least one tile that isn't in the same room as the stairs, prioritize it!
        else if (stairs_roomie_nb > 0) {
            final_spawn_positions = stairs_roomies;
            final_tile_nb = stairs_roomie_nb;
        }
    }
    int idx = DungeonRandInt(final_tile_nb);
    dungeon->gen_info.team_spawn_pos.x = final_spawn_positions[idx].x;
    dungeon->gen_info.team_spawn_pos.y = final_spawn_positions[idx].y;
    MemFree(ideal_spawn_positions);
    MemFree(stairs_roomies);
}

__attribute((naked)) void CannotForgetSealedMoves(void) {
    asm volatile("ldrh r1,[r4,#0x2]");
    asm volatile("tst r1,#0x1");
    asm volatile("movne r3,#0x1");
    asm volatile("cmp r3,#0x0"); // Original instruction
    asm volatile("bx r14");
}

__attribute((used)) int TwoTileMoveAiAdjustment(int current_num_targets, struct move_target_and_range move_ai_range, struct entity* user, struct entity* target, struct move* move, bool check_all_conditions) {
    return CanSeeTarget(user, target) ? TryAddTargetToAiTargetList(current_num_targets, move_ai_range, user, target, move, check_all_conditions) : 0;
}

__attribute__((used)) int CustomUpdateMovePP(struct entity* monster, struct move* move) {
    struct monster* monster_info = (struct monster*)(monster->info);
    for (int i = 0; i < 4; i++) {
        if (((struct monster*)(monster_info))->moves[i].f_last_used) {
            struct move* move = &(((struct monster*)(monster_info))->moves[i]);
            if (move->id.val == 117)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 104)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 394)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 123)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 319)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 186)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 215)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 11)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 219)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 183)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 95)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 56)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 130)
            {
                move->f_sealed = true;
            }
            if (move->id.val == 438)
            {
                move->f_sealed = true;
            }
        }
    }
    return false;
}

/*
    Parses custom uppercase text tags. Made by Irdkwia, ported to CoT by Adex.
        - "VS:X:Y" ("VITESSE") modifies text speed by X/Y. For example, "[VS:1:2]" halves speed, but "[VS:4]" quadruples it. The second parameter is optional, and if missing, will default to 1.
        - "VR" reverts text speed to normal (equivalent to "[VS:1:1]" and "[VS:1]").

    To ignore a text tag in a textbox that doesn't scroll, check for dialogue_display_state::flags.timer_2.
*/
__attribute((used)) bool ParseCustomUppercaseTextTags(struct dialogue_display_state* state, const char* tag, const char** tag_params, int tag_param_count) {
    int tag_vals[4] = { 0 };
    if (tag_param_count > ARRAY_LENGTH(tag_vals))
        tag_param_count = ARRAY_LENGTH(tag_vals);

    for (int i = 0; i < tag_param_count; i++)
        tag_vals[i] = AtoiTag(tag_params[i]);

    // Checking for an actual tag...
    if (StrcmpTag(tag, "VS")) {
        if (state->flags.timer_2)
            return true;
        if (tag_param_count > 0) {
            text_numerator = tag_vals[0];
            text_denominator = tag_param_count == 1 ? 1 : tag_vals[1];
            text_count = 0;
            state->text_scrolling_done = 0;
        }
        return true;
    }
    else if (StrcmpTag(tag, "VR")) {
        if (state->flags.timer_2)
            return true;
        ResetTextSpeedValues();
        state->text_scrolling_done = 0;
        return true;
    }
    return false;
}

__attribute((used)) uint32_t TryChangeTextSpeed(struct dialogue_display_state* state) {
    unsigned long long result = _s32_div_f((state->text_speed * text_numerator + text_count), text_denominator);
    text_count = result >> 32;
    return (uint32_t)result;
}

__attribute((naked)) void HijackTextSpeed(void) {
    asm("mov r0,r4");
    asm("b TryChangeTextSpeed");
}

__attribute((naked)) void HijackTextLoop(void) {
    asm("ldr r0,[r4,#0x80]");
    asm("cmp r0,#0x0");
    asm("bxne r14");
    // Per the original comment: "Here is the fun part"
    asm("mov r0,#0x0");
    asm("b AnalyzeTextReturn");
}

#define ABILITY_SAND_FORCE 0x7C
#define ABILITY_OVERCOAT 0x7D

int __attribute__((used)) CalcMeFirstAndAbilityMultipliers(int x, int mult_fp, struct entity* attacker, int move_id)
{
    struct monster* attacker_mon = attacker->info;
    int result = x;
    if (attacker_mon->me_first_flag)
        result = MultiplyByFixedPoint(result, mult_fp);
    if (AbilityIsActiveVeneer(attacker, ABILITY_SAND_FORCE) && GetApparentWeather(attacker) == WEATHER_SANDSTORM)
    {
        struct move move;
        InitMove(&move, move_id);
            result = MultiplyByFixedPoint(result, 0x133);
    }
    return result;
}

bool __attribute__((used)) HasSandProtectionAbility(struct entity* entity, int ability_id) { return AbilityIsActiveVeneer(entity, ability_id) || AbilityIsActiveVeneer(entity, ABILITY_OVERCOAT) || AbilityIsActiveVeneer(entity, ABILITY_SAND_FORCE); }

bool __attribute__((used)) HasHailProtectionAbility(struct entity* entity, int ability_id) { return AbilityIsActiveVeneer(entity, ability_id) || AbilityIsActiveVeneer(entity, ABILITY_OVERCOAT); }
