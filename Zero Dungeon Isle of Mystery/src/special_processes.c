#include <pmdsky.h>
#include <cot.h>

extern void RefreshStorage();

// Special process 100: Change belly to 70
static int SpChangeBelly(short arg1) {
    TEAM_MEMBER_TABLE_PTR->active_roster[0].belly = 70;
  return 0;
}

// Special process 101: Change belly to 50
static int SpChangeBellyMore(short arg1) {
    TEAM_MEMBER_TABLE_PTR->active_roster[0].belly = 50;
    return 0;
}

// Special process 102: Raise HP by 5
static int SpRested(short arg1) {
    TEAM_MEMBER_TABLE_PTR->active_roster[0].max_hp += 5;
    TEAM_MEMBER_TABLE_PTR->active_roster[0].current_hp += 5;
    return 0;
}

// Special process 103: Reset 102
static int SpReset(short arg1) {
    TEAM_MEMBER_TABLE_PTR->active_roster[0].max_hp -= 5;
    TEAM_MEMBER_TABLE_PTR->active_roster[0].current_hp -= 5;
    return 0;
}

// Special process 104: Change belly and max belly to 120
static int SpBigApple(short arg1) {
    TEAM_MEMBER_TABLE_PTR->active_roster[0].belly = 120;
    TEAM_MEMBER_TABLE_PTR->active_roster[0].max_belly = 120;
    return 0;
}

// Special process 105: Reset 104
static int SpBellyReset(short arg1) {
    TEAM_MEMBER_TABLE_PTR->active_roster[0].belly = 100;
    TEAM_MEMBER_TABLE_PTR->active_roster[0].max_belly = 100;
    return 0;
}

// Special process 106: Counts the number of a stackable item in storage. Replace ITEM_STICK with any stackable item.
static int CountSticksInStorage(short arg1) {
    struct bulk_item item = { .id = { ITEM_STICK }}; // quantity shouldn't matter, as CountItemTypeInStorage doesn't seem to access it
    return CountItemTypeInStorage(&item);
}

// Special process 107: Made by adex_8x, removes a set amount of items from storage! First parameter is Item ID, second is number of items.
static int SpRemoveItemsFromStorage(short item_id, short quantity_to_remove) {
    struct bulk_item bulk_item = { {item_id}, quantity_to_remove };
    int total_count = CountItemTypeInStorage(&bulk_item);
    // hacky b/c this hasn't been documented all that well Yet :tm:
    int16_t* STORAGE_ITEM_IDS = (int16_t*)((uint32_t) & (BAG_ITEMS_PTR_MIRROR[0]) + 0x38A);
    int16_t* STORAGE_ITEM_AMT = (int16_t*)((uint32_t) & (BAG_ITEMS_PTR_MIRROR[0]) + 0xB5A);
    // Return false if we have fewer items than we're trying to remove...
    // Or, to remove those items anwyay, replace "return false" w/ "quantity_to_remove = total_count"
    if (total_count < quantity_to_remove)
        return false;
    bool is_thrown = IsThrownItem(item_id);
    for (int i = 0; i < GetRankStorageSize() && quantity_to_remove > 0; i++) {
        if (STORAGE_ITEM_IDS[i] == item_id) {
            if (is_thrown) {
                int16_t current_item_amount = STORAGE_ITEM_AMT[i];
                if (quantity_to_remove >= current_item_amount) {
                    STORAGE_ITEM_AMT[i] = 0x0;
                    STORAGE_ITEM_IDS[i] = 0x0;
                    quantity_to_remove -= current_item_amount;
                }
                else {
                    STORAGE_ITEM_AMT[i] -= quantity_to_remove;
                    quantity_to_remove = 0;
                }
            }
            else {
                STORAGE_ITEM_AMT[i] = 0x0;
                STORAGE_ITEM_IDS[i] = 0x0;
                quantity_to_remove--;
            }
        }
        else if (STORAGE_ITEM_IDS[i] == 0)
            break;
    }
    RefreshStorage(); // Need to declare "RefreshStorage = 0x0201007c;" in custom_NA.ld
    return true;
}

__attribute((naked)) static struct item* GetCurrentBagItems(void) {
    asm volatile("ldr r0,=BAG_ITEMS_PTR_MIRROR");
    asm volatile("ldr r0,[r0,#0x0]");
    asm volatile("ldr r0,[r0,#0x384]");
    asm volatile("bx r14");

    asm volatile(".ltorg");
}

// Special Process 108: Another adex SP. Moves specific items from bag to storage. Returns true if any amount of items from the bag were removed. Only relevant parameter in the SP is the Item ID.
static bool SpTransferItemFromBagToStorage(short item_id) {
    int quantity_to_store = CountItemTypeInBag(item_id), capacity = GetCurrentBagCapacity();
    bool is_thrown = IsThrownItem(item_id), item_was_stored = false;
    struct item* CURRENT_BAG_ITEMS = GetCurrentBagItems();
    if (quantity_to_store <= 0)
        return false;
    for (int i = 0; i < capacity && quantity_to_store > 0 && !IsStorageFull(); i++) {
        struct item current_item = CURRENT_BAG_ITEMS[i];
        if (current_item.id.val == item_id) {
            quantity_to_store -= is_thrown ? current_item.quantity : 1;
            AddItemToStorage(&current_item);
            RemoveItem(i);
            item_was_stored = true;
        }
    }
    RemoveEmptyItems(CURRENT_BAG_ITEMS, 0x32);
    return item_was_stored;
}

// Called for special process IDs 100 and greater.
//
// Set return_val to the return value that should be passed back to the game's script engine. Return true,
// if the special process was handled.
bool CustomScriptSpecialProcessCall(undefined4* unknown, uint32_t special_process_id, short arg1, short arg2, int* return_val) {
  switch (special_process_id) {
    case 100:
      *return_val = SpChangeBelly(arg1);
      return true;
    case 101:
        *return_val = SpChangeBellyMore(arg1);
        return true;
    case 102:
        *return_val = SpRested(arg1);
        return true;
    case 103:
        *return_val = SpReset(arg1);
        return true;
    case 104:
        *return_val = SpBigApple(arg1);
        return true;
    case 105:
        *return_val = SpBellyReset(arg1);
        return true;
    case 106:
        *return_val = CountSticksInStorage(arg1);
        return true;
    case 107:
        *return_val = SpRemoveItemsFromStorage(arg1, arg2);
        return true;
    case 108:
        *return_val = SpTransferItemFromBagToStorage(arg1);
        return true;

    // Add your own SP's here...

    default:
      return false;
  }
}