#include "game/StdAfx.h"
#include "AiModExample.h"

#include <game/AI.h>
#include <game/Blocks.h>


struct AAttack_ModStarter final : public APositionBase {

    snConfigDims targetCfg;

    AAttack_ModStarter(AI* ai): APositionBase(ai) { }

    virtual uint update(uint blockedLanes)
    {
        m_ai->rushDir = float2();
        const Block *target = m_ai->target.get();
        if (isTargetObstructed(target))
            return LANE_NONE;

        target->getNavConfig(&targetCfg.cfg);
        snPrecision precision;
        precision.pos = getWaypointRadius();

        Block*        command = m_ai->command;
        BlockCluster* cluster = command->cluster;

        const AttackCapabilities &caps = m_ai->getAttackCaps();

        // can't attack without weapons...
        if (!caps.weapons)
            return noAction("No Weapons");

        const AttackCapabilities &tcaps = target->commandAI->getAttackCaps();

        const float2 pos        = cluster->getAbsolutePos();
        const float2 targetPos  = target->getAbsolutePos();
        const float2 targetVel  = target->cluster->getVel();
        const float  targetDist = distance(targetPos, pos) - 0.5f * target->cluster->getCoreRadius();

        // FIXME targetDist is a hack here... works fine for the common case

        targetCfg.dims = 0;

        const float mydps = caps.rushDps / tcaps.totalHealth;
        const float tdps = tcaps.totalDps / caps.totalHealth;
        const uint64 flags = m_ai->getConfig().flags;

        const bool rushing = (mydps > 1.1f * tdps || (flags&SerialCommand::ALWAYS_RUSH)) && !(flags&SerialCommand::ALWAYS_MANEUVER);
        const float snipeRange = 1.1f * tcaps.maxRange;
        const bool canStayOutOfRange = (caps.maxRange > snipeRange) &&
            target->cluster->isMobile() &&
            caps.getDpsAtRange(snipeRange) > 2.f * tcaps.healthRegen &&
            !(flags&SerialCommand::ALWAYS_MANEUVER);
        const bool sniping = (!rushing && canStayOutOfRange) || (flags&SerialCommand::ALWAYS_KITE);
        status = rushing ? _("Rushing") :
            canStayOutOfRange ? gettext_("Kiting", "Sniping") : _("Maneuvering");

        // FIXME too many magic numbers here! at least make them cvars
        const float wantRange = rushing ? 0.f :
            canStayOutOfRange ? snipeRange :
            (0.9f * caps.bestRange);

        const float2 targetLeadPos = targetPos + kAIBigTimeStep * targetVel;
        const float2 targetDir = normalize(targetLeadPos - pos);

        if (!canStayOutOfRange && wantRange < targetDist)
            m_ai->rushDir = targetDir;

        const float2 dir = (caps.hasFixed ? directionForFixed(cluster, targetPos, targetVel, FiringFilter()) :
            targetLeadPos - pos);

        // move to the optimal attack range
        targetCfg.cfg.position = targetLeadPos - targetDir * wantRange;
        targetCfg.cfg.velocity = 0.95f * targetVel; // damp velocity - otherwise they drift together
        targetCfg.cfg.angle = vectorToAngle(dir);
        targetCfg.dims = SN_POSITION | SN_ANGLE | (rushing ? SN_TARGET_VEL : SN_VELOCITY);
        precision.pos = max(precision.pos, 0.1f * caps.bestRange);

        // escape super fast if we are sniping
        if (sniping) {
            if (targetDist < wantRange) {
                targetCfg.cfg.velocity += 10.f * (targetCfg.cfg.position - pos);
                targetCfg.dims = SN_ANGLE | SN_VELOCITY | SN_VEL_ALLOW_ROTATION;
            }
            else if (targetDist < 1.1f * caps.maxRange) {
                // don't worry about position, just match velocity
                if (caps.hasFixed)
                    targetCfg.dims = SN_ANGLE | SN_VELOCITY;
                else
                    targetCfg.dims = SN_ANGLE | SN_VELOCITY | SN_VEL_ALLOW_ROTATION;
            }
        }
        else if (caps.hasFixed && targetDist <= caps.maxRange) {
            targetCfg.dims |= SN_POS_ANGLE;
        }

        if (!targetCfg.dims)
            return noAction("No direction");

        m_ai->nav->setDest(targetCfg.cfg, targetCfg.dims, precision);
        return LANE_MOVEMENT;
    }

    virtual const char* toPrettyString() const { return status; }

    string toStringName() const override { return "AAttack_ModStarter"; }
};


struct AAvoidCluster_ModStarter final : public AIAction
{

    vector<Obstacle> obstacles;
    snConfig         config;

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.isMobile; }

    void generateClusterObstacleList(Block* command)
    {
        // FIXME this should include asteroids! - nearby asteroid blocks?
        const BlockCluster *cl       = command->cluster;
        const float         clRad    = cl->getBRadius();
        const float2        clPos    = cl->getAbsolutePos();
        const float2        clVel    = cl->getVel();
        const BlockList    &commands = m_ai->getAllies();

        obstacles.clear();
        foreach (const Block *bl, commands)
        {
            const BlockCluster *bcl = bl->cluster;
            if (bl != command && cl != bcl && bcl->getMass() > 0.5f * cl->getMass())
            {
                Obstacle obs(*bcl, clRad, bcl->getMass() / cl->getMass());
                if (intersectRayCircle(clPos, clVel - obs.vel, obs.pos, 5 * obs.rad))
                    obstacles.push_back(obs);
            }
        }
    }

    AAvoidCluster_ModStarter(AI* ai) : AIAction(ai, LANE_MOVEMENT, PRI_ALWAYS) {}

    virtual uint update(uint blockedLanes)
    {
        // explosive blocks / missiles don't collide with each other anyway
        if (m_ai->command->sb.features&Block::EXPLODE)
            return LANE_NONE;

        // don't bother for single seeds
        if ((m_ai->command->sb.features&Block::SEED) && getCluster()->size() == 1)
            return LANE_NONE;

        generateClusterObstacleList(m_ai->command);

        if (velocityObstacles(&config.velocity, NULL, getClusterPos(), getCluster()->getVel(),
            m_ai->nav->maxAccel, getTargetDirection(m_ai, obstacles), float2(), obstacles))
        {
            m_ai->nav->setDest(config, SN_VELOCITY, 0);
            return LANE_MOVEMENT;
        }
        return LANE_NONE;
    }

    string toStringName() const override { return "AAvoidCluster_ModStarter"; }
};


// set fallback target - i.e. incoming missile
struct AFallbackTarget_ModStarter final : public AIAction
{

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.hasWeapons; }
    AFallbackTarget_ModStarter(AI* ai) : AIAction(ai, LANE_TARGET, PRI_ALWAYS) {}

    virtual uint update(uint blockedLanes)
    {
        float2 blah;
        if (m_ai->isBigUpdate())
        {
            float         minDistSqr = FLT_MAX;
            const Block  *target     = NULL;
            /*
            const float2  pos        = m_ai->command->getClusterPos();

            foreach (const Block* tgt, m_ai->getEnemies())
            {
                const float dist = distanceSqr(pos, tgt->getAbsolutePos());
                if (dist < minDistSqr && (isActivelyHostile(tgt) != AI::NEUTRAL)) {
                    minDistSqr = dist;
                    target     = tgt;
                }
            }
            foreach (const Block *tgt, m_ai->getQuery().queryBlockObstacles(m_ai->command))
            {
                const float dist = distanceSqr(pos, tgt->getAbsolutePos());
                if (dist < minDistSqr && (isActivelyHostile(tgt) != AI::NEUTRAL)) {
                    minDistSqr = dist;
                    target     = tgt;
                }
            }

            status = target ? "Set fallback target" : "no fallback target";
            m_ai->fallbackTarget = target;
            */
        }
        return LANE_NONE;       // never blocks other targetingp
    }

    string toStringName() const override { return "AFallbackTarget_ModStarter"; }
};


struct AWeapons_ModStarter final : AIAction
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

    AWeapons_ModStarter(AI* ai) : AIAction(ai, LANE_SHOOT) {}

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

    string toStringName() const override { return "AWeapons_ModStarter"; }
};


// target based on faction: attack enemy ships
struct ATargetEnemy_ModStarter final : public ATargetBase {

    ATargetEnemy_ModStarter(AI* ai) : ATargetBase(ai) { }

    virtual uint update(uint blockedLanes)
    {
        return findSetTarget();
    }

    // copy-paste from Reassembly ATargetBase.
    // If you don't implement it, you get the built-in version instead.
    // If you use the built-in version, the two copy-paste "CVars" below
    // are respected (instead of being hard-coded as below).
    #if 1
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

    string toStringName() const override { return "ATargetEnemy_ModStarter"; }
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
    if (!_strcmpi(name, "AAttack"))
        return new AAttack_ModStarter(ai);
    if (!_strcmpi(name, "AAvoidCluster"))
        return new AAvoidCluster_ModStarter(ai);
    if (!_strcmpi(name, "AFallbackTarget"))
        return new AFallbackTarget_ModStarter(ai);
    if (!_strcmpi(name, "ATargetEnemy"))
        return new ATargetEnemy_ModStarter(ai);
    if (!_strcmpi(name, "AWeapons"))
        return new AWeapons_ModStarter(ai);
    return nullptr;
}
