#include "game/StdAfx.h"
#include "AiModExample.h"

#include <game/AI.h>
#include <game/Blocks.h>
#include <game/Sector.h>


#define ADD_ACTION(TYPE, ...)                       \
    if (TYPE::supportsConfig(ai->getConfig()))      \
        ai->addAction(new TYPE(ai, __VA_ARGS__));


//=============================================================================
// Exported functions
//=============================================================================

void GetApiVersion(int * major, int * minor) {
    *major = 1;
    *minor = 0;
}

// tournament mode AI
bool CreateAiActions(AI* ai) {
    const AICommandConfig &         config = ai->getConfig();
    const ECommandFlags::value_type flags  = config.flags;

    if (config.isMobile >= 2 && (config.flags & SerialCommand::DODGES)) {
        ai->addActionVanilla(VANILLA_ACTION_TYPE_AVOID_WEAPON);
    }

    ai->addActionVanilla(VANILLA_ACTION_TYPE_WEAPONS);

    ai->addActionVanilla(VANILLA_ACTION_TYPE_FALLBACK_TARGET);
    ai->addActionVanilla(VANILLA_ACTION_TYPE_TARGET_ENEMY);
    ai->addActionVanilla(VANILLA_ACTION_TYPE_AVOID_CLUSTER);
    ai->addActionVanilla(VANILLA_ACTION_TYPE_ATTACK);
    ai->addActionVanilla(VANILLA_ACTION_TYPE_HEALERS); // notice this isn't used by the interceptor, due to supportsConfig()
    ai->addActionVanilla(VANILLA_ACTION_TYPE_INVESTIGATE);

    if (config.features&Block::ASSEMBLER)
    {
        ai->addActionVanilla(VANILLA_ACTION_TYPE_HEAL);
        if (config.flags&SerialCommand::TRACTOR_TRANSIENT) {
            ai->addActionVanilla(VANILLA_ACTION_TYPE_SCAVENGE_WEAPON);
        }
        if (!config.hasFreeRes || kAIEnableNoResReproduce)
        {
            if (config.flags&SerialCommand::METAMORPHOSIS) {
                ai->addActionVanilla(VANILLA_ACTION_TYPE_METAMORPHOSIS);
            }
            ai->addActionVanilla(VANILLA_ACTION_TYPE_BUD_REPRODUCE);
        }
        // else ADonate: find allies and give them resources?
    }
    else if (config.features&Block::REGROWER)
    {
        ai->addActionVanilla(VANILLA_ACTION_TYPE_HEAL);
    }

    if (config.isMobile && config.isRoot() && !config.isAttached)
    {
        // FIXME need to either metamorphasize or plant self
        // FIXME very bad to overwrite to non-plant blueprint and then get planted
        ai->addActionVanilla(VANILLA_ACTION_TYPE_PLANT_SELF);
        ai->addActionVanilla(VANILLA_ACTION_TYPE_METAMORPHOSIS);
    }

    if (config.isMobile && !nearZero(ai->command->sb.command->destination))
    {
        ai->appendCommandDest(ai->command->sb.command->destination, 0.25f * kSectorSize);
    }

    if (config.isMobile &&
        !(flags&(SerialCommand::FOLLOWER)) &&
        !config.hasParent &&
        (flags&SerialCommand::WANDER))
    {
        ai->addActionVanilla(VANILLA_ACTION_TYPE_WANDER);
    }

    return true; // we handled it; no need for default AI actions
}
