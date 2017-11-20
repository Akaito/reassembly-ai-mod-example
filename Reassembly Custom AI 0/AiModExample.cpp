#include "AiModExample.h"

struct AWeaponsExample final : AIAction
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

    AWeaponsExample(AI* ai) : AIAction(ai, LANE_SHOOT) {}

    uint update(uint blockedLanes) override
    {
        blockedLanes;
        enabled = 0;
        /*
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
        //*/
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

    string toStringName() const override { return "AWeaponsExample_saiyan"; }
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
        return new AWeaponsExample(ai);
    return nullptr;
}
