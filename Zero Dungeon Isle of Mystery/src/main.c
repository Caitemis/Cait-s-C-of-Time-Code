#include <pmdsky.h>
#include <cot.h>

// Removes PP depletion from using moves. Will seal set moves after use, with the moves being the IDs seen below. Check patch.asm for relevant hook.
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

#define ABILITY_SAND_FORCE 0x7C
#define ABILITY_OVERCOAT 0x7D

// Made by adex_8x. Implements Overcoat. Check patch.asm for relevant hook.
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
        enum type_id move_type = GetMoveTypeForMonster(attacker, &move);
        if (move_type == TYPE_ROCK || move_type == TYPE_GROUND || move_type == TYPE_STEEL)
            result = MultiplyByFixedPoint(result, 0x133);
    }
    return result;
}

bool __attribute__((used)) HasSandProtectionAbility(struct entity* entity, int ability_id) { return AbilityIsActiveVeneer(entity, ability_id) || AbilityIsActiveVeneer(entity, ABILITY_OVERCOAT) || AbilityIsActiveVeneer(entity, ABILITY_SAND_FORCE); }

bool __attribute__((used)) HasHailProtectionAbility(struct entity* entity, int ability_id) { return AbilityIsActiveVeneer(entity, ability_id) || AbilityIsActiveVeneer(entity, ABILITY_OVERCOAT); }