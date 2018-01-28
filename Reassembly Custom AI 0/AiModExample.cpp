#include "game/StdAfx.h"
#include "AiModExample.h"

#include <game/AI.h>
#include <game/Blocks.h>

struct AWeapons_Saiyan final : AIAction
{
    //*
    int  enabled = 0;        // number of weapons enabled
    bool isFallback = false;

    /*
    static bool supportsConfig(const AICommandConfig& cfg)
    {
        return cfg.hasWeapons && (cfg.features&FIREABLE_WEAPONS);
    }
    */

    AWeapons_Saiyan(AI* ai) : AIAction(ai, LANE_SHOOT) {}

    uint update(uint blockedLanes) override
    {
        blockedLanes;
        enabled = 0;
        if (m_ai->target) {
            FiringData data(m_ai->target);
            data.filter = ~Block::AUTOFIRE;
            enabled = m_ai->fireWeaponsAt(data);
            isFallback = false;
        }
        if (!enabled && m_ai->fallbackTarget && nearZero(m_ai->rushDir)) {
            FiringData data(m_ai->fallbackTarget);
            data.filter = ~Block::AUTOFIRE;
            enabled = m_ai->fireWeaponsAt(data);
            isFallback = true;
        }
        return enabled ? LANE_SHOOT : LANE_NONE;
    }

    string toStringEx() const override
    {
        //return str_format("enabled %d at %s target", enabled, isFallback ? "fallback" : "main");
        //return "AWeaponsExample";
        string result("enabled ");
        result += enabled;
        result += " at ";
        result += isFallback ? "fallback" : "main";
        result += " target";
        return result;
    }
    //*/

    string toStringName() const override { return "AWeapons_Saiyan"; }
};


// target based on faction: attack enemy ships
struct ATargetEnemy_Saiyan final : public ATargetBase {

    ATargetEnemy_Saiyan(AI* ai) : ATargetBase(ai) { }

    virtual uint update(uint blockedLanes)
    {
        return findSetTarget();
    }

    // copy-paste from Reassembly ATargetBase, just to document
    #if 0
    uint findSetTarget()
    {
        // cvars in Reassembly; lazy way to get them here
        int aiTargetMin = 500;
        float aiTargetThreshold = 0.25f;

        if (!m_ai->getAttackCaps().weapons)
        {
            status = "No Weapons";
            m_ai->setTarget(NULL, AIMood::NEUTRAL);
            return LANE_TARGET;
        }

        Target target = testAcceptTarget(m_ai->priorityTarget.get());
        if (!target.first)
            target = testAcceptTarget(m_ai->target.get());

        if (!(target.first && m_ai->priorityTarget) && m_ai->isBigUpdate())
        {
            targets.clear();
            const bool isAttack = (m_ai->getConfig().flags&ECommandFlags::ATTACK);
            const int deadlyThreshold = isAttack ? 10 : min(aiTargetMin, int(aiTargetThreshold * getCluster()->getDeadliness()));
            foreach (const Block* tgt, m_ai->getEnemies())
            {
                AIMood mood = AIMood::NEUTRAL;
                if (tgt->getBlueprintDeadliness() >= deadlyThreshold &&
                    (!isAttack || !tgt->sb.isTransient()) &&
                    (mood = acceptTarget(tgt)) != AI::NEUTRAL)
                {
                    targets.push_back(make_pair(tgt, mood));
                }
            }

            // pick closest remaining target
            const float2 defPos = !nearZero(m_ai->defendPos) ? m_ai->defendPos : getClusterPos();
            target = vec_min(
                targets, [&](const Target& tgt) { return targetDistanceMetric(defPos, tgt.first); },
                target, (target.first ? (0.7 * targetDistanceMetric(defPos, target.first)) :
                    FLT_MAX));
        }

        if (!target.first)
            return noAction("No Target");

        status = "Found Target";
        m_ai->setTarget(target.first, target.second);
        return LANE_TARGET;
    }
    #endif

    string toStringName() const override { return "ATargetEnemy_Saiyan"; }
};


//=============================================================================
// Exported functions
//=============================================================================
/*
bool SupportsConfig(const char * name, const AICommandConfig& cfg)
{
    if (!std::strcmpi(name, "AWeapons"))
        return AWeaponsExample::supportsConfig(cfg);
    return false;
}
*/

AIAction * CreateAiAction(const char * name, AI* ai) {
    if (!_strcmpi(name, "AWeapons"))
        return new AWeapons_Saiyan(ai);
    if (!_strcmpi(name, "ATargetEnemy"))
        return new ATargetEnemy_Saiyan(ai);
    return nullptr;
}
