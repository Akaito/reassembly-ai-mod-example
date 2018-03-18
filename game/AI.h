
#ifndef _AI_H
#define _AI_H

#include "AI_modapi.h"
#include "Nav.h"
#include "Types.h"

DLLFACE extern float kAITimeStep;
DLLFACE extern float kAIBigTimeStep;
DLLFACE extern float kAISuperTimeStep;

DLLFACE extern int   kAIPathMaxQueries;

DLLFACE extern int   kAIEnableNoResReproduce;


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

DLLFACE float2 directionForFixed(
    const BlockCluster *cluster,
    float2 targetPos,
    float2 targetVel,
    const FiringFilter &filter
);

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
    DLLFACE float getDpsAtRange(float range) const;
    
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
    DLLFACE FiringData(const Block *bl);
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
    Mod_CreateAiActions           m_aiModCreateActions = nullptr;
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
    DLLFACE ObstacleQuery&    getQuery();
    DLLFACE bool              isBigUpdate() const;
    bool                      isSuperUpdate() const;
    DLLFACE const BlockList&  getEnemies();
    DLLFACE const BlockList&  getAllies();
    float                     getEnemyDeadliness();
    float                     getAllyDeadliness();
    float                     getEnemyAllyRatio();
    vector<ResourcePocket*>&  getVisibleResources();
    DLLFACE const AttackCapabilities& getAttackCaps();
    const AttackCapabilities& getCachedAttackCaps() const { return m_attackCaps; }
    const AICommandConfig&    getConfig() const { return m_config; }
    DLLFACE int               fireWeaponsAt(FiringData &data);
    DLLFACE Block *           enumWeapons(int * indexInOut);
    bool                      isValidTarget(const Block *bl) const;
    DLLFACE void              setTarget(const Block* bl, AIMood mood);
    DLLFACE float2            estimateTargetPos() const;
    DLLFACE bool              canEstimateTargetPos() const;
    DLLFACE void              addAction(AIAction * action);
    DLLFACE bool              addActionVanilla(VanillaActionType actionType);  // returns true if an action type was recognized.  Action still isn't added if supportsConfig() is false.
    const AICommandConfig &   commandConfig() { return m_config;  }
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
    DLLFACE void appendCommandDest(float2 p, float r);

    void onDamaged(int faction);
    void onClusterInit();
    void onCommandDeath();
    void update(bool force=false);
    void render(DMesh &mesh, float2 screenPos, vector<TextBoxString> &tvec);

    size_t getSizeof() const;
};


struct APositionBase : public AIAction
{
    bool obstructed = false;

    APositionBase(AI* ai): AIAction(ai, LANE_MOVEMENT) { }

    DLLFACE bool isTargetObstructed(const Block *target=NULL);
};


struct Obstacle {
    float2 pos, vel;
    float rad;                  // includes the radius of the ship!!!!
    float damage;               // as a percentage of the ship health!!!!
    bool  canShootDown;

    DLLFACE Obstacle(const BlockCluster &bc, float radius, float dmg) NOEXCEPT;
    DLLFACE Obstacle(const Projectile &pr, float radius, float dmg) NOEXCEPT;
    DLLFACE bool isDangerous(float2 clPos, float2 clVel) const;
};

struct ObstacleQuery
{
private:
    vector<Projectile*> m_projVec;
    vector<Block*>      m_blockVec;
    vector<Obstacle>    m_obstacles;

public:

    DLLFACE const vector<Obstacle> &getLast() const;
    DLLFACE const vector<Obstacle> &queryObstacles(Block* command, bool blocksOnly=false);
    DLLFACE const vector<Block*> &queryBlockObstacles(Block *command);
    int cullForDefenses(const AttackCapabilities &caps);

};

class PathFinder {

    struct Node {
        float2      pos;
        const Node *parent = NULL;
        Node       *next   = NULL;
        int         g      = 0; // number of nodes to start
        float       f      = -1.f; // weight. lower weighted nodes are searched first

        struct Compare {
            bool operator()(const Node *a, const Node *b) { return a->f > b->f; }
        };
    };

    typedef std::priority_queue<Node*, std::vector<Node*>, Node::Compare> PQueue;
    Node *                                  m_nodes = NULL;
    PQueue                                  m_q;
    std::unordered_set<const BlockCluster*> m_traced;
    spatial_hash<bool>                      m_explored;
    vector<float2>                          m_path;
    int                                     m_queries = 0;
    int                                     m_maxQueries = kAIPathMaxQueries;

    const BlockCluster* intersectSegmentClusters(const BlockCluster* cl, float2 start, float2 end) const;
    bool trace1(const BlockCluster* ocl, const BlockCluster *cl, float2 start, float2 end,
                float g, const Node *parent);
    bool trace(const BlockCluster *cl, float2 start, float2 end, float g, const Node *parent);
    void calcPath(const Node *node);
    void clear();

public:

    DLLFACE PathFinder();

    ~PathFinder()
    {
        m_nodes = slist_clear(m_nodes);
    }

    const vector<float2>& getPath() const { return m_path; }
    const vector<float2>& findPath(const BlockCluster *cl, float2 end, float endRad, int maxc);
    void render(VertexPusherLine& line) const;
};

struct AMove final : public AIAction {

    snConfigDims target;
    snPrecision  prec;

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.isMobile; }

    DLLFACE AMove(AI* ai);
    AMove(AI* ai, float2 pos, float r);

    void setMoveDest(float2 pos, float r);
    void setMoveAngle(float angle, float rad);
    void setMoveRot(float2 vec, float rad);
    void setMoveDestAngle(float2 pos, float r, float angle, float rad);
    void setMoveConfig(const snConfigDims& cfg, float radius);
    virtual uint update(uint blockedLanes);
};

// FIXME should provide a way to set velocity at destination
struct APath final : public AIAction {

    PathFinder path;
    AMove move;

    float  time;
    float2 pos;
    float  rad;

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.isMobile; }

    APath(AI* ai) : AIAction(ai, LANE_MOVEMENT), move(ai)
    {
        setPathDest(float2(0), 0);
        IsFinished = true;
    }

    APath(AI* ai, float2 pos_, float r) : AIAction(ai, LANE_MOVEMENT), move(ai, pos_, r)
    {
        setPathDest(pos_, r);
    }


    DLLFACE void setPathDest(float2 p, float r);

    bool isAtDest() const;

    DLLFACE virtual uint update(uint blockedLanes);

    DLLFACE virtual void render(void* lineVoid) const;
};

// stock AAvoidWeapon action
DLLFACE struct AAvoidWeapon final : public AIAction {

    snConfig      config;

    int           culled = 0;
    int           count = 0;

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.isMobile; }
    virtual const char* toPrettyString() const { return _("Dodging"); }
    virtual string toStringEx() const { return str_format("Culled %d/%d", culled, count); }

    AAvoidWeapon(AI* ai) : AIAction(ai, LANE_MOVEMENT, PRI_ALWAYS) { }

    DLLFACE virtual uint update(uint blockedLanes);
};

DLLFACE struct AHealers final : public AIAction {
    
    bool fixedHealer = false;

    AHealers(AI* ai) : AIAction(ai, LANE_HEALER) { }

    static bool supportsConfig(const AICommandConfig& cfg)
    {
        return cfg.hasHealers;
    }

    DLLFACE virtual uint update(uint blockedLanes);

    virtual const char* toPrettyString() const { return status; }

};

struct AHeal final : public AIAction {

    AHeal(AI* ai) : AIAction(ai, LANE_ASSEMBLE) {}

    DLLFACE static bool supportsConfig(const AICommandConfig& cfg);

    bool valid   = true;
    bool matches = true;

    DLLFACE virtual uint update(uint blockedLanes);
};

DLLFACE struct AScavengeWeapon final : public AIAction {

    std::set<uint> fitFailed;

    DLLFACE static bool supportsConfig(const AICommandConfig& cfg);

    AScavengeWeapon(AI* ai) : AIAction(ai, LANE_ASSEMBLE) {}

    virtual void onReset()
    {
        fitFailed.clear();
    }

    DLLFACE virtual uint update(uint blockedLanes);
};

DLLFACE struct AMetamorphosis final : public AIAction {

    const char *which = "None";

    AMetamorphosis(AI* ai) : AIAction(ai, LANE_ASSEMBLE) {}

    DLLFACE static bool supportsConfig(const AICommandConfig& cfg);
    DLLFACE virtual void onReset();
    DLLFACE virtual uint update(uint blockedLanes);

    virtual string toStringEx() const
    {
        return str_format("picked %s: %s", which, status);
    }
};

DLLFACE struct ABudReproduce final : public AIAction {

    float          childResTarget;
    float          needRes;
    vector<Block*> followers;

    ABudReproduce(AI* ai) : AIAction(ai, LANE_ASSEMBLE),
                                 childResTarget(0), needRes(0) {}

    DLLFACE static bool supportsConfig(const AICommandConfig& cfg);
    DLLFACE virtual void onReset();
    DLLFACE virtual uint update(uint blockedLanes);
};

DLLFACE struct AWeapons final : public AIAction {

    int  enabled    = 0;        // number of weapons enabled
    bool isFallback = false;

    AWeapons(AI* ai) : AIAction(ai, LANE_SHOOT) {}

    DLLFACE static bool supportsConfig(const AICommandConfig& cfg);
    DLLFACE virtual uint update(uint blockedLanes);

    virtual string toStringEx() const
    {
        return str_format("enabled %d at %s target", enabled, isFallback ? "fallback" : "main");
    }
};

// set fallback target - i.e. incoming missile
DLLFACE struct AFallbackTarget final : public AIAction {

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.hasWeapons; }
    AFallbackTarget(AI* ai) : AIAction(ai, LANE_TARGET, PRI_ALWAYS) {}

    DLLFACE virtual uint update(uint blockedLanes);
};

// target based on faction: attack enemy ships
DLLFACE struct ATargetEnemy final : public ATargetBase {

    ATargetEnemy(AI* ai) : ATargetBase(ai) { }

    virtual uint update(uint blockedLanes)
    {
        return findSetTarget();
    }
};

DLLFACE struct AAvoidCluster final : public AIAction {

    vector<Obstacle> obstacles;
    snConfig         config;

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.isMobile; }

    AAvoidCluster(AI* ai) : AIAction(ai, LANE_MOVEMENT, PRI_ALWAYS) {}

    DLLFACE void generateClusterObstacleList(Block* command);
    DLLFACE virtual uint update(uint blockedLanes);
};

DLLFACE struct AAttack final : public APositionBase {

    snConfigDims targetCfg;

    AAttack(AI* ai): APositionBase(ai) { }

    DLLFACE static bool supportsConfig(const AICommandConfig& cfg);
    DLLFACE virtual uint update(uint blockedLanes);

    virtual const char* toPrettyString() const { return status; }

};

DLLFACE struct AInvestigate final : public AIAction {

    APath       path;

    static bool supportsConfig(const AICommandConfig& cfg) { return APath::supportsConfig(cfg); }
    virtual void render(void* line) const { path.render(line); }

    AInvestigate(AI* ai) : AIAction(ai, LANE_MOVEMENT), path(ai) { }

    DLLFACE virtual uint update(uint blockedLanes);
};

DLLFACE struct AWander final : public AIAction {

    AMove move;

    DLLFACE static bool supportsConfig(const AICommandConfig& cfg);
    DLLFACE float getMoveRad() const;

    AWander(AI* ai) : AIAction(ai, LANE_MOVEMENT), move(ai) { }

    DLLFACE virtual uint update(uint blockedLanes);
};

// estimate best overall direction for avoiding obstacles
DLLFACE float2 getTargetDirection(const AI* ai, const vector<Obstacle> &obs);

//AAvoidWeapon does not always know what the best direction is, and will only make small adjustments anyway
DLLFACE bool velocityObstacles(
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
