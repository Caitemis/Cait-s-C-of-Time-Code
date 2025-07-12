#include <pmdsky.h>
#include <cot.h>

//Resets stat changes.
static void ItemPureSeedVariant(struct entity* target) {
    if (target->type == ENTITY_MONSTER) {
        TryResetStatChanges(target, target, false);
    }
}

//Changes type to pure Water.
static void ItemAquamelon(struct entity* target) {
    if (target->type == ENTITY_MONSTER) {
        struct monster* target_mon = (struct monster*)target->info;
        target_mon->types[0].val = TYPE_WATER;
        target_mon->types[1].val = TYPE_NONE;
        target_mon->statuses.type_changed = true;
        TryInflictSlipStatus(target, target);
        LogMessageWithPopupCheckUser(target, "Type changed to Water!");
    }
}

//Changes type to pure Normal.
static void ItemPlainBerry(struct entity* target) {
    if (target->type == ENTITY_MONSTER) {
        struct monster* target_mon = (struct monster*)target->info;
        target_mon->types[0].val = TYPE_NORMAL;
        target_mon->types[1].val = TYPE_NONE;
        target_mon->statuses.type_changed = true;
        LogMessageWithPopupCheckUser(target, "Type changed to Normal!");
    }
}

//Changes primary type to Fire.
static void ItemFlareBerry(struct entity* target) {
    if (target->type == ENTITY_MONSTER) {
        struct monster* target_mon = (struct monster*)target->info;
        target_mon->types[0].val = TYPE_FIRE;
        target_mon->statuses.type_changed = true;
        LogMessageWithPopupCheckUser(target, "First type changed to Fire!");
    }
}

//Changes primary type to Electric.
static void ItemLightningfruit(struct entity* target) {
    if (target->type == ENTITY_MONSTER) {
        struct monster* target_mon = (struct monster*)target->info;
        target_mon->types[0].val = TYPE_ELECTRIC;
        target_mon->statuses.type_changed = true;
        LogMessageWithPopupCheckUser(target, "First type changed to Electric!");
    }
}

//Changes type to pure Dark.
static void ItemMoonlightCherry(struct entity* target) {
    if (target->type == ENTITY_MONSTER) {
        struct monster* target_mon = (struct monster*)target->info;
        target_mon->types[0].val = TYPE_DARK;
        target_mon->types[1].val = TYPE_NONE;
        target_mon->statuses.type_changed = true;
        LogMessageWithPopupCheckUser(target, "Type changed to Dark!");
    }
}

//Deals damage and inflicts Burn.
static void ItemFirewood(struct entity* user, struct entity* target, bool is_thrown) {
    struct move move;
    InitMove(&move, 0x195);
        DealDamageProjectile(user, target, &move, STICK_POWER, 0x100, ITEM_STICK);
        TryInflictBurnStatus(user, target, false, false, false);
}

//Deals damage and blows away target. Target is blown in direction thrower is facing.
static void ItemHardwood(struct entity* user, struct entity* target, bool is_thrown) {
    struct move move;
    InitMove(&move, 0x195);
    DealDamageProjectile(user, target, &move, IRON_THORN_POWER, 0x100, ITEM_STICK);
        struct monster* monster = (struct monster*)user->info;
        TryBlowAway(user, target, monster->action.direction.val);
}

//Deals damage and inflicts Shadow Hold status.
static void ItemPalmLeaf(struct entity* user, struct entity* target, bool is_thrown) {
    struct move move;
    InitMove(&move, 0x195);
    DealDamageProjectile(user, target, &move, STICK_POWER, 0x100, ITEM_STICK);
    TryInflictShadowHoldStatus(user, target, false);
}

//Deals set 50 damage and inflicts Badly Poisoned status.
static void ItemStrangeSeed(struct entity* user, struct entity* target, bool is_thrown) {
    struct move move;
    InitMove(&move, 0x195);
    struct damage_data damage_out;
    int damage = 50;
    int move_type = GetMoveType(&move);
    union damage_source source = GetDamageSourceWrapper(&move, NULL);
    CalcDamageFixedNoCategory(user, target, damage, true, &damage_out, move_type, source, 0, 1, 0);
    TryInflictBadlyPoisonedStatus(user, target, true, false);
}

//Deals set 100 damage and inflicts Blinker and Cross-Eyed statuses.
static void ItemEversoil(struct entity* user, struct entity* target, bool is_thrown) {
    struct move move;
    InitMove(&move, 0x195);
    struct damage_data damage_out;
    int damage = 100;
    int move_type = GetMoveType(&move);
    union damage_source source = GetDamageSourceWrapper(&move, NULL);
    CalcDamageFixedNoCategory(user, target, damage, true, &damage_out, move_type, source, 0, 1, 0);
    TryInflictBlinkerStatus(user, target, false, false);
    TryInflictCrossEyedStatus(user, target, false);
}

//Deals set 150 damage and inflicts Paused status for 10 turns.
static void ItemMusicalShell(struct entity* user, struct entity* target, bool is_thrown) {
    struct move move;
    InitMove(&move, 0x195);
    struct damage_data damage_out;
    int damage = 150;
    int move_type = GetMoveType(&move);
    union damage_source source = GetDamageSourceWrapper(&move, NULL);
    CalcDamageFixedNoCategory(user, target, damage, true, &damage_out, move_type, source, 0, 1, 0);
    TryInflictPausedStatus(user, target, false, 10, 1, 0);
}

// Change the case's item to whichever you want!
bool CustomApplyItemEffect(
  struct entity* user, struct entity* target, struct item* item, bool is_thrown
) {
  switch (item->id.val) {
    case ITEM_PURE_SEED:
      ItemPureSeedVariant(target);
      return true;
    case ITEM_SLIP_SEED:
        ItemAquamelon(target);
        return true;
    case ITEM_CLEAR_GUMMI:
        ItemPlainBerry(target);
        return true;
    case ITEM_RED_GUMMI:
        ItemFlareBerry(target);
        return true;
    case ITEM_YELLOW_GUMMI:
        ItemLightningfruit(target);
        return true;
    case ITEM_BLACK_GUMMI:
        ItemMoonlightCherry(target);
        return true;
    case ITEM_STICK:
        ItemFirewood(user, target, is_thrown);
        return true;
    case ITEM_IRON_THORN:
        ItemHardwood(user, target, is_thrown);
        return true;
    case ITEM_CACNEA_SPIKE:
        ItemPalmLeaf(user, target, is_thrown);
        return true;
    case ITEM_KINGS_ROCK:
        ItemStrangeSeed(user, target, is_thrown);
        return true;
    case ITEM_WHITE_GUMMI:
        ItemEversoil(user, target, is_thrown);
        return true;
    case ITEM_OREN_BERRY:
        ItemMusicalShell(user, target, is_thrown);
        return true;
    default:
      return false;
  }
}
