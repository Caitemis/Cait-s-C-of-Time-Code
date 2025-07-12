#include <pmdsky.h>
#include <cot.h>

//Implements Soak, thanks to Sudzoof for writing this! I now have too much power. Changes target's type to pure Water.
static bool MoveSoak(struct entity* user, struct entity* target, struct move* move) {
    struct monster* target_mon = (struct monster*)target->info;
    target_mon->types[0].val = TYPE_WATER;
    target_mon->types[1].val = TYPE_NONE;
    target_mon->statuses.type_changed = true;
    LogMessageWithPopupCheckUser(user, "Type changed to Water!");
    return false;
}

//Vacuum Cut variant, deals damage equal to user's Level times 4.
static bool MoveVacuumCutVariant(struct entity* user, struct entity* target, struct move* move)
{
    struct damage_data damage_out;
    struct monster* user_monster = (struct monster*)user->info;
    int damage = user_monster->level * 4;
    int move_type = GetMoveType(move);
    int move_category = GetMoveCategory(move->id.val);
    union damage_source source = GetDamageSourceWrapper(move, NULL);
    CalcDamageFixedWrapper(user, target, damage, false, &damage_out, move_type, move_category, source, 0, 1, 0);
    return false;
}

//Essentially Explosion, but doesn't destroy items or walls.
static bool MoveImplode(struct entity* user, struct entity* target, struct move* move)
{
    struct damage_data damage_out;
    int damage = 80;
    int move_type = GetMoveType(move);
    int move_category = GetMoveCategory(move->id.val);
    union damage_source source = GetDamageSourceWrapper(move, NULL);
    CalcDamageFixedWrapper(user, target, damage, false, &damage_out, move_type, move_category, source, 0, 1, 0);
    if (TWINEEDLE_HIT_TRACKER == 0) {
        int fixed_damage = (int)*(short*)((int)user->info + 0x10) / 2;
        if (fixed_damage == 0) {
        }
        else {
            CalcDamageFixedWrapper(user, user, fixed_damage, true, &damage_out, move_type, move_category, source, 0, 1, 0);
        }
        TWINEEDLE_HIT_TRACKER = 1;
    }
    return false;
}

//Implements Anchor Shot. Damages target and inflicts the Shadow Hold status.
static bool MoveAnchorShot(struct entity* user, struct entity* target, struct move* move) {
    if (!DealDamage(user, target, move, 0x100, ITEM_NOTHING)) {
        return false;
    }
    TryInflictShadowHoldStatus(user, target, false);
    return true;
}

//Psybeam variant. Always confuses.
static bool MovePsybeamVariant(struct entity* user, struct entity* target, struct move* move) {
    if (!DealDamage(user, target, move, 0x100, ITEM_NOTHING)) {
        return false;
    }
    TryInflictConfusedStatus(user, target, false, false);
    return true;
}

//Inflicts Blinker status.
static bool MoveCrystalFlash(struct entity* user, struct entity* target, struct move* move) {
    TryInflictBlinkerStatus(user, target, false, false);
    return false;
}

//Psychic variant. Chance of Special Defense drop. Goes in a Hyper Beam variant slot (Giga Impact, Frenzy Plant, etc.) to force recharge.
static bool MovePsychicVariant(struct entity* user, struct entity* target, struct move* move) {
    if (!DealDamage(user, target, move, 0x100, ITEM_NOTHING)) {
        return false;
    }
    if (!DungeonRandOutcomeUserTargetInteraction(user, target, 20)) {
        return false;
    }
    LowerDefensiveStat(user, target, 1, 1, true, true);
    return true;
}

//Raises all stats by 1 (except Speed). Based on SMT Luster Candy.
static bool MoveLusterCandy(struct entity* user, struct entity* target, struct move* move) {
    BoostOffensiveStat(user, target, 0, true);
    BoostOffensiveStat(user, target, 1, true);
    BoostDefensiveStat(user, target, 0, true);
    BoostDefensiveStat(user, target, 1, true);
    BoostHitChanceStat(user, target, 0);
    BoostHitChanceStat(user, target, 1);
    return false;
}

//Implements Quiver Dance. Raises Special Attack, Special Defense, and Speed.
static bool MoveQuiverDance(struct entity* user, struct entity* target, struct move* move) {
    BoostOffensiveStat(user, target, 1, true);
    BoostDefensiveStat(user, target, 1, true);
    BoostSpeedOneStage(user, target, 0, false);
    return false;
}

//Inflicts Invisible status.
static bool MoveInvisible(struct entity* user, struct entity* target, struct move* move) {
    TryInflictInvisibleStatus(user, target);
    return false;
}

//Bubble Beam, deals damage without the speed drop chance.
static bool MoveBubbleBeamNSD(struct entity* user, struct entity* target, struct move* move) {
    DealDamage(user, target, move, 0x100, ITEM_NOTHING);
    return false;
}

//Created by adex_8x
static void DoMoveMitosis(struct entity* user, struct entity* target, struct move* move) {
#define SPECIAL_MONSTER_ID 537 // Change to whichever monster ID you'd like!
#define SPAWN_MAX 2

    union damage_source source = { 0x257 };
    struct position user_pos = { user->pos.x, user->pos.y };
    struct monster* user_mon = user->info;
    // Is the target Pokémon in the spawn list?
    struct monster_spawn_entry* target_spawn_entry = NULL;
    int spawnable_monster_id;
    for (int i = 0; i < 16; i++) {
        struct monster_spawn_entry* spawn_entry = &(DUNGEON_PTR->spawn_entries[i]);
        spawnable_monster_id = GetMonsterIdFromSpawnEntry(spawn_entry);
        if (FemaleToMaleForm(spawnable_monster_id) == SPECIAL_MONSTER_ID) {
            target_spawn_entry = spawn_entry;
            break;
        }
    }
    if (target_spawn_entry) {
        // How many enemies can we spawn?
        int enemy_spawnable_count = 0;
        for (int i = 0x4; i < 0x10; i++) {
            struct entity* enemy = &(DUNGEON_PTR->entity_table.entities[i]);
            if (!EntityIsValid(enemy)) {
                enemy_spawnable_count++;
                if (enemy_spawnable_count >= SPAWN_MAX)
                    break;
            }
        }
        if (user_mon->is_not_team_member && !user_mon->is_ally && enemy_spawnable_count < SPAWN_MAX)
            enemy_spawnable_count++; // If an enemy uses this, there'll always be at least one spawnable monster, since it'll faint!
        if (enemy_spawnable_count == 0) {
            // Cannot spawn due to the enemy spawn limit.
            LogMessageWithPopupCheckUser(user, "But nothing happened!");
            return;
        }
        // Check if it's possible to spawn somewhere adjacent...
        enum direction_id valid_directions[6];
        enum direction_id prioritized_directions[2];
        int valid_directions_counter = 0;
        int prioritized_directions_counter = 0;
        memset(valid_directions, -1, sizeof(valid_directions));
        memset(prioritized_directions, -1, sizeof(prioritized_directions));
        // Get all valid tiles to spawn in...
        for (enum direction_id dir = DIR_DOWN; dir < DIR_CURRENT; dir++) {
            struct position candidate_pos = { DIRECTIONS_XY[dir][0] + user_pos.x, DIRECTIONS_XY[dir][1] + user_pos.y };
            if (!PosIsOutOfBounds(candidate_pos.x, candidate_pos.y) && !IsInvalidSpawnTile(SPECIAL_MONSTER_ID, GetTileSafe(candidate_pos.x, candidate_pos.y))) {
                // The special monster can stand on this potential tile--mark this direction as valid!
                if (dir == DIR_LEFT || dir == DIR_RIGHT) {
                    prioritized_directions[prioritized_directions_counter] = dir;
                    prioritized_directions_counter++;
                }
                else {
                    valid_directions[valid_directions_counter] = dir;
                    valid_directions_counter++;
                }
            }
        }
        if (user_mon->is_not_team_member && !user_mon->is_ally)
            HandleFaint(user, source, user);
        int spawns = 0;
        struct spawned_monster_data mitosis_data_base = {
          .monster_id = {spawnable_monster_id},
          .behavior = {BEHAVIOR_NORMAL_ENEMY_0x0},
          .level = target_spawn_entry->level_mult_512 >> 9,
          .cannot_be_asleep = true,
          .pos = {user_pos.x, user_pos.y}
        };
        struct spawned_monster_data mitosis_data;
        struct entity* spawned_mon = NULL;
        if (prioritized_directions_counter + valid_directions_counter > 0) {
            for (int i = 0; i < prioritized_directions_counter && spawns < SPAWN_MAX; i++) {
                MemcpySimple(&mitosis_data, &mitosis_data_base, sizeof(mitosis_data_base));
                mitosis_data.pos.x += DIRECTIONS_XY[prioritized_directions[i]][0];
                mitosis_data.pos.y += DIRECTIONS_XY[prioritized_directions[i]][1];
                spawned_mon = SpawnMonster(&mitosis_data, true);
                if (spawned_mon)
                    spawns++;
            }
            // Additional spawning if we couldn't spawn in two prioritized directions!
            for (int i = prioritized_directions_counter; i < 2 && spawns < SPAWN_MAX; i++) {
                MemcpySimple(&mitosis_data, &mitosis_data_base, sizeof(mitosis_data_base));
                // Pick a random direction from the valid list...
                if (valid_directions_counter > 0) {
                    int rand = DungeonRandInt(valid_directions_counter);
                    mitosis_data.pos.x += DIRECTIONS_XY[valid_directions[rand]][0];
                    mitosis_data.pos.y += DIRECTIONS_XY[valid_directions[rand]][1];
                    // Shift the list to ensure we don't pick the same direction twice...
                    for (int j = rand; j < 5; j++) {
                        valid_directions[j] = valid_directions[j + 1];
                    }
                    valid_directions_counter--;
                    spawned_mon = SpawnMonster(&mitosis_data, true);
                    if (spawned_mon)
                        spawns++;
                }
            }
        }
        if (spawns < SPAWN_MAX && !IsInvalidSpawnTile(SPECIAL_MONSTER_ID, GetTileSafe(user_pos.x, user_pos.y))) {
            spawned_mon = SpawnMonster(&mitosis_data, true);
            if (spawned_mon)
                spawns++;
        }
        if (spawns == 0) {
            // Absolutely nothing could be spawned!
            LogMessageWithPopupCheckUser(GetLeader(), "But nothing happened!");
        }
        else if (!user_mon->is_not_team_member || user_mon->is_ally)
            user_mon->hp = 1;
    }
    else {
        // Cannot spawn due to monster SPECIAL_MONSTER_ID not being in the spawn list for this floor.
        LogMessageWithPopupCheckUser(user, "But nothing happened!");
    }
}

// Called when using moves. Should return true if a custom effect was applied.
// This function is only called if the move doesn't fail due to a missing target
bool CustomApplyMoveEffect(
    move_effect_input* data, struct entity* user, struct entity* target, struct move* move)
{
    switch (data->move_id)
    {
    case MOVE_SURF:
        data->out_dealt_damage = MoveSoak(user, target, move);
        return true;
    case MOVE_VACUUM_CUT:
        data->out_dealt_damage = MoveVacuumCutVariant(user, target, move);
        return true;
    case MOVE_SELFDESTRUCT:
        data->out_dealt_damage = MoveImplode(user, target, move);
        return true;
    case MOVE_PSYCHIC:
        data->out_dealt_damage = MoveAnchorShot(user, target, move);
        return true;
    case MOVE_PSYBEAM:
        data->out_dealt_damage = MovePsybeamVariant(user, target, move);
        return true;
    case MOVE_FLASH:
        data->out_dealt_damage = MoveCrystalFlash(user, target, move);
        return true;
    case MOVE_GIGA_IMPACT:
        data->out_dealt_damage = MovePsychicVariant(user, target, move);
        return true;
    case MOVE_CALM_MIND:
        data->out_dealt_damage = MoveLusterCandy(user, target, move);
        return true;
    case MOVE_DOUBLE_TEAM:
        data->out_dealt_damage = MoveQuiverDance(user, target, move);
        return true;
    case MOVE_MIST:
        data->out_dealt_damage = MoveInvisible(user, target, move);
        return true;
    case MOVE_AQUA_RING:
        DoMoveMitosis(user, target, move);
        return true;
    case MOVE_BUBBLEBEAM:
        MoveBubbleBeamNSD(user, target, move);
        return true;
    default:
        return false;
    }
}