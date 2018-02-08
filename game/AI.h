
#ifndef _AI_H
#define _AI_H

// #define CHRIS_DLL_TEST (IS_DEVEL)
#define CHRIS_DLL_TEST (1)

#include "AI_modapi.h"
#include "Types.h"

template <typename Vtx> struct LineMesh;
template <typename TriV, typename LineV> struct MeshPair;
struct AIModData;
struct VertexPosColor;
typedef LineMesh<VertexPosColor> VertexPusherLine;
typedef MeshPair<VertexPosColor, VertexPosColor> DMesh;
struct TextBoxString;
struct FiringFilter;
struct OneMod;
struct WeaponGroup;

float2 directionForFixed(const BlockCluster *cluster, float2 targetPos, float2 targetVel,
                         const FiringFilter &filter);

#ifdef _WIN32
typedef HMODULE DllHandle;
typedef AIAction* (WINAPI *ModCreateAiAction)(const char* actionType, AI* ai);
#else
typedef nullptr_t DllHandle;
typedef AIAction* (*ModCreateAiAction)(const char* actionType, AI* ai);
#endif

struct AIActionList {

    vector<AIAction*> lastActions;
    vector<AIAction*> actions;
    uint              actionWasRun = 0;
    
    const char*       prettyState = "";

    void       insert(AIAction* a);            // insert before first action of greater priority
    void       clear();
    int        clearTag(uint tag);             // tag is a bitmask, return cleared count

    void       update();

    string     toString() const;
    void       render(VertexPusherLine &vp) const;
    size_t     getSizeof() const;
    
    ~AIActionList() { clear(); }
};

static const float kAvoidDebrisRadiusRatio = 0.5f;
static const float kAvoidDebrisRadiusMin   = 40.f;

struct AISignal {
    
    enum NeedType { NONE=0, DEADLINESS=1, CAPACITY=4 };

    float2               pos;
    float                radius    = 0.f;
    uint                 priority  = 5;
    NeedType             type      = NONE;
    float                needTotal = 0.f;
    vector< watch_ptr<const Block> > responders;

    void reset(NeedType tp, float need, uint pri, float2 p, float rad)
    {
        if (tp == NONE)
            responders.clear();
        // otherwise let responders remove themselves if they want
        type      = tp;
        needTotal = need;
        priority  = pri;
        pos       = p;
        radius    = rad;
    }

    void reset() { reset(NONE, 0, 0, float2(0), 0); }

    bool satisfiesNeed(const BlockCluster *bl) const;
    float satisfiedNeed() const;
    bool hasNeed() const { return satisfiedNeed() < needTotal; }

    string toString() const
    {
        return str_format("%s/%d %.f/%.f(%d)", ((type == DEADLINESS) ? "Deadliness" :
                                                (type == CAPACITY) ? "Capacity" : "None"),
                          priority, needTotal, satisfiedNeed(), int(responders.size()));
    }
};

struct AttackCapabilities {
    float  bestRange   = 0;
    float  maxRange    = 0;
    float  totalDps    = 0;
    float  rushDps     = 0;
    float  autofireDps = 0;
    float  weaponVel   = 0.f;
    uint64 weapons     = 0;
    bool   hasFixed    = false;
    float  totalHealth = 0.f;
    float  maxAccel    = 0.f;
    float  healthRegen = 0.f;
    
    vector< pair<float, float> > rangeDmg;
    float getDpsAtRange(float range) const;
    
    bool   initialized = false;
};

struct FiringFilter {
    Feature_t allowMask     = ~0;
    float     minEfficiency = 0.f;
    bool      healer        = false;

    FiringFilter() {}
    FiringFilter(Feature_t mask) : allowMask(mask) {}
    bool allow(const Block *bl) const;
};

// shared info for targeting/firing weapons
struct FiringData {

    const Block  *command = NULL;
    const float2  pos;
    const float2  clusterPos;
    const float2  clusterVel;
    const float   clusterRad = 0.f;
    const int     faction    = -1;

    int          index = 0;
    int          turretIndex = 0;
    float        totalSpread = 0.f;
    float        aimError = 0.f;
    FiringFilter filter;
    const WeaponGroup* wg = NULL;
    bool         enableSpacialQuery = false;
    bool         useTurretTargetAngle = false;
    FiringData() {}
    FiringData(const watch_ptr<const Block> &bl) : FiringData(bl.get()) {}
    DLLFUNC FiringData(const Block *bl);
};

struct WeaponGroup {

    vector< Block* > blocks;
    int spreadCount = 0;

private:
    float         rippleDelta          = 0.f;
    mutable int   rippleIndex          = 0;
    mutable float rippleIndexStartTime = 0.f;
public:

    bool isRippleEnabled() const { return rippleDelta > epsilon; }

    void init(bool isRipple, bool isSpread);
    void setEnable(bool enable, float sangle) const;
    bool isReady(int index) const;
    float getSpreadAngle(int tidx, float sangle) const;
    int fireAtTarget(FiringData &data) const;
};

struct AutofireData {
    std::unordered_map<const Block*, int> spread;
    int lastSimStep = -1;
};

// encapsulates the intelligence of a command block
struct AI final {

    using AIMood = ::AIMood;
    static const auto OFFENSIVE = AIMood::OFFENSIVE;
    static const auto NEUTRAL = AIMood::NEUTRAL;
    static const auto DEFENSIVE = AIMood::DEFENSIVE;

private:

    static const char* moodToString(AIMood m)
    {
        return m == OFFENSIVE ? "offensive" : m == DEFENSIVE ? "defensive" : "neutral";
    }
    
    bool                          m_enemiesQueried = false;
    bool                          m_alliesQueried  = false;
    AIActionList                  m_actions;
    ModCreateAiAction             m_aiModCreateAction = nullptr;
    BlockList                     m_enemies;
    BlockList                     m_allies;
    vector<ResourcePocket*>       m_visibleResources;
    AttackCapabilities            m_attackCaps;
    watch_ptr<Block>              m_parent;
    vector< watch_ptr<Block> >    m_children;
    AICommandConfig               m_config; // will rebuild action list if this changes
    WeaponGroup                   m_weapons;
    int                           m_weaponsVersion = -1;
    copy_ptr<ObstacleQuery>       m_obsQuery;
    
    // render read-only state
    float                         m_renderEnemyDeadliness = 0;
    float                         m_renderAllyDeadliness  = 1;

    void resetForActions();
    void recreateActions();
    bool recreateActionsModded();
    bool playerUpdate(); // return true to run AI actions
    void onPlayerAdopt(Block *child);

    template <typename T>
    void addRecycleAction();
    AIAction *getRecycle(const std::type_info &tp);
 
public:
    // AIAction interface
    watch_ptr<const Block>          target;
    watch_ptr<const Block>          fallbackTarget; // shoot at this if target is not yet in range
    watch_ptr<const Block>          priorityTarget; // use this target if possible
    float                           targetPosTime;
    float2                          targetPos;
    float2                          targetVel;
    watch_ptr<const Block>          healTarget;
    float                           healRange;
    float2                          rushDir;
    AISignal                        signal;
    float2                          defendPos; // prioritize enemies near this position
    float                           defendRadius; // don't path this far from defendPos
    int                             damagedFaction = -1;
    float                           lastDamagedTime = -1.f;
    GameZone *                      zone = NULL;
    AutofireData                    autofire;

    void             setMod(const AIModData* modApi, bool shouldResetActions);
    DLLFUNC ObstacleQuery&    getQuery();
    DLLFUNC bool              isBigUpdate() const;
    bool                      isSuperUpdate() const;
    DLLFUNC const BlockList&  getEnemies();
    DLLFUNC const BlockList&  getAllies();
    float                     getEnemyDeadliness();
    float                     getAllyDeadliness();
    float                     getEnemyAllyRatio();
    vector<ResourcePocket*>&  getVisibleResources();
    DLLFUNC const AttackCapabilities& getAttackCaps();
    const AttackCapabilities& getCachedAttackCaps() const { return m_attackCaps; }
    const AICommandConfig&    getConfig() const { return m_config; }
    DLLFUNC int               fireWeaponsAt(FiringData &data);
    bool                      isValidTarget(const Block *bl) const;
    DLLFUNC void              setTarget(const Block* bl, AIMood mood);
    float2                    estimateTargetPos() const;
    bool                      canEstimateTargetPos() const;
    float2                    getTargetPos() const;
    const Block*              getTarget() const;

    Block*                    getParent() const { return m_parent.get(); }
    const AI*                 getParentAI() const;
    vector< watch_ptr<Block> >& getChildren() { return m_children; }
    const vector< watch_ptr<Block> >& getChildren() const { return m_children; }
    void                      adoptChild(Block* child);
    bool                      removeChild(Block* child);
    void                      removeParent();
    Faction_t                 getFaction() const;

    int                       isMobile() const;
    const char*               toPrettyState() const { return m_actions.prettyState; }

    // Block/Game/UI interface
    SerialCommand       *sc;    // = command->sb.command
    Block*               command;
    copy_ptr<sNav>       nav;

    AIMood               mood = NEUTRAL;
    Faction_t            moodFaction = -1;

    Agent*               agent = NULL;


    AI(Block* c);
    ~AI();
    
    void clearCommands();
    void appendCommandDest(float2 p, float r);

    void onDamaged(int faction);
    void onClusterInit();
    void onCommandDeath();
    void update(bool force=false);
    void render(DMesh &mesh, float2 screenPos, vector<TextBoxString> &tvec);

    size_t getSizeof() const;
};


struct Obstacle {
    float2 pos, vel;
    float rad;                  // includes the radius of the ship!!!!
    float damage;               // as a percentage of the ship health!!!!
    bool  canShootDown;

    DLLFUNC Obstacle(const BlockCluster &bc, float radius, float dmg) NOEXCEPT;
    DLLFUNC Obstacle(const Projectile &pr, float radius, float dmg) NOEXCEPT;
    DLLFUNC bool isDangerous(float2 clPos, float2 clVel) const;
};

struct ObstacleQuery
{
private:
    vector<Projectile*> m_projVec;
    vector<Block*>      m_blockVec;
    vector<Obstacle>    m_obstacles;

public:

    DLLFUNC const vector<Obstacle> &getLast() const;
    DLLFUNC const vector<Obstacle> &queryObstacles(Block* command, bool blocksOnly=false);
    DLLFUNC const vector<Block*> &queryBlockObstacles(Block *command);
    int cullForDefenses(const AttackCapabilities &caps);

};

// estimate best overall direction for avoiding obstacles
DLLFUNC float2 getTargetDirection(const AI* ai, const vector<Obstacle> &obs);

//AAvoidWeapon does not always know what the best direction is, and will only make small adjustments anyway
DLLFUNC bool velocityObstacles(
    float2 *velOut,
    float* pBestDamage,
    float2 position,
    float2 velocity,
    float2 maxAccel,
    float2 targetDir,
    float2 rushDir,
    const vector<Obstacle> &obstacles
);

#endif // _AI_H
