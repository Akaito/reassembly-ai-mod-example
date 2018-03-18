
#ifndef _AI_MODAPI_H
#define _AI_MODAPI_H

// originally from platformincludes.h
typedef unsigned int uint;
/////

// originally from Geometry.h
#define GLM_FORCE_RADIANS 1
#define GLM_FORCE_XYZW 1
#include "../glm/vec2.hpp"
#include "../glm/vec3.hpp"
#include "../glm/vec4.hpp"
#include "../glm/mat3x3.hpp"
#include "../glm/mat4x4.hpp"
#include "../glm/trigonometric.hpp"
#include "../glm/exponential.hpp"
#include "../glm/common.hpp"
//#include "../glm/packing.hpp"
#include "../glm/geometric.hpp"
//#include "../glm/matrix.hpp"
//#include "../glm/vec_relational.hpp"
//#include "../glm/integer.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtx/color_space.hpp"
#include "../glm/gtc/random.hpp"
//#include "../glm/gtc/quaternion.hpp"

typedef unsigned char uchar;
typedef unsigned long long uint64; // not from Geometry.h

typedef glm::vec2 float2;
/////

#ifndef TYPE_NAME_S
// this is a backup to make compilation work in mods.  You shouldn't use this in mods!
#define TYPE_NAME_S(x) (#x)
#endif


#include <string>
using std::string;

#ifndef vector
#include <vector>
using std::vector;
#endif

struct AI;
struct AICommandConfig;
struct Block;
struct BlockCluster;

#ifdef _WIN32
typedef HMODULE DllHandle;
typedef void (WINAPI *Mod_GetApiVersion)(int * major, int * minor);
typedef bool (WINAPI *Mod_CreateAiActions)(AI* ai);
#else
typedef std::nullptr_t DllHandle;
typedef void (*Mod_GetApiVersion)(int * major, int * minor);
typedef bool (*Mod_CreateAiActions)(AI* ai);
#endif

// Use this with ai->addActionVanilla().
enum VanillaActionType {
    VANILLA_ACTION_TYPE_NONE                   = 0,
    VANILLA_ACTION_TYPE_ATTACK                 = 1,
    VANILLA_ACTION_TYPE_AVOID_CLUSTER          =  2,
    VANILLA_ACTION_TYPE_AVOID_WEAPON           =  3,
    VANILLA_ACTION_TYPE_BUD_REPRODUCE          =  4,
    //VANILLA_ACTION_TYPE_CLEAR_DEBRIS           =  5,
    //VANILLA_ACTION_TYPE_COLLECT                =  6,
    //VANILLA_ACTION_TYPE_COLLECT_WANDER         =  7,
    VANILLA_ACTION_TYPE_FALLBACK_TARGET        =  8,
    //VANILLA_ACTION_TYPE_FOLLOW_ALLIES          =  9,
    //VANILLA_ACTION_TYPE_FOLLOW_PARENT          = 10,
    VANILLA_ACTION_TYPE_HEAL                   = 35,
    VANILLA_ACTION_TYPE_HEALERS                = 11,
    VANILLA_ACTION_TYPE_INVESTIGATE            = 12,
    //VANILLA_ACTION_TYPE_INVESTIGATE_HEREDITARY = 13,
    //VANILLA_ACTION_TYPE_JUST_GO                = 14,
    //VANILLA_ACTION_TYPE_KILL_PLANTS            = 15,
    VANILLA_ACTION_TYPE_METAMORPHOSIS          = 17,
    //VANILLA_ACTION_TYPE_PLANT                  = 18,
    VANILLA_ACTION_TYPE_PLANT_SELF             = 36,
    //VANILLA_ACTION_TYPE_RAM_TARGET             = 19,
    //VANILLA_ACTION_TYPE_RESPOND_TO_SIGNAL      = 20,
    //VANILLA_ACTION_TYPE_RES_TO_PARENT          = 21,
    //VANILLA_ACTION_TYPE_RETURN_PARENT          = 22,
    //VANILLA_ACTION_TYPE_ROTATE                 = 23,
    //VANILLA_ACTION_TYPE_RUN_AWAY               = 24,
    VANILLA_ACTION_TYPE_SCAVENGE_WEAPON        = 25,
    //VANILLA_ACTION_TYPE_SIGNAL_BACKUP          = 26,
    //VANILLA_ACTION_TYPE_TARGET_DEFENSE         = 27,
    VANILLA_ACTION_TYPE_TARGET_ENEMY           = 28,
    //VANILLA_ACTION_TYPE_TARGET_HELP_ALLIES     = 29,
    //VANILLA_ACTION_TYPE_TARGET_HEREDITARY      = 30,
    //VANILLA_ACTION_TYPE_TARGET_OPPORTUNITY     = 31,
    //VANILLA_ACTION_TYPE_UNTARGET               = 32,
    VANILLA_ACTION_TYPE_WANDER                 = 33,
    VANILLA_ACTION_TYPE_WEAPONS                = 34,

    // changed by ChrisB, 2018-03-18 (update this line to force merge conflicts)
    // next available: 37
};

enum AIMood { OFFENSIVE = 1,           // initiating combat
              NEUTRAL   = 0,           // no combat
              DEFENSIVE = -1 };        // only fighting because being attacked

enum AIPriority {
    PRI_OVERRIDE = 0,
    PRI_ALWAYS   = 1,
    PRI_COMMAND  = 3,
    PRI_DEFAULT  = 4,
};


struct AIAction {

    enum ActionLane
    {
        LANE_NONE        = 0,
        LANE_MOVEMENT    = 1<<0,
        LANE_SHOOT       = 1<<1,
        LANE_TARGET      = 1<<2,
        LANE_ASSEMBLE    = 1<<3,
        LANE_SIGNAL      = 1<<4,
        LANE_HEALER      = 1<<5,
        // ...
    };

    enum ActionTag
    {
        TAG_COMMAND  = 1<<1,    // normal command, for RTS style completeable actions
        TAG_BEHAVIOR = 1<<2,
    };

    AIAction(AI* a, uint lane, AIPriority pri=PRI_DEFAULT)
        : m_ai(a), Lanes(lane), Priority(pri), Tag(TAG_BEHAVIOR)
    {
        IsFinished = false;
        Blocking = LANE_NONE;
    }

    virtual ~AIAction() {}

    AI*         m_ai;
    bool        IsFinished;
    uchar       Blocking;   // bitmask of blocked lanes
    const uchar Lanes;      // bitmask of lanes blocked by
    uchar       Priority;
    uchar       Tag;
    const char* status = "";

    // return new lanes to block
    virtual uint update(uint blockedLanes)=0;

    virtual void onReset() {}

    virtual string toStringName() const { return TYPE_NAME_S(*this); }
    virtual string toStringEx() const { return status; }
    virtual const char* toPrettyString() const { return ""; }

    static bool supportsConfig(const AICommandConfig& cfg) { return true; }

    DLLFACE uint noAction(const char* reason);

    string toString() const;

    virtual void render(void* line) const { line; }

    // helper
    DLLFACE const BlockCluster* getCluster() const;
    DLLFACE float2 getClusterPos() const;
    float  getClusterBRadius() const;

    bool isDestObstructed(float2 dest, uint ignoreFaction, float avoidDebrisRatio) const;

    // repath if we hit something (bigger than kAvoidDebrisRadiusRatio)
    bool needsRepath() const;

    DLLFACE AIMood isActivelyHostile(const Block* target) const;

    DLLFACE float getWaypointRadius() const;
    
};


struct AICommandConfig {

    uint64 flags;
    uint64 features;
    int    isMobile;
    bool   isDoomed;
    bool   isAttached;
    bool   hasFreeRes;
    bool   hasParent;
    bool   hasWeapons;
    bool   hasHealers;

    AICommandConfig(const Block* cmd);
    
    bool operator==(const AICommandConfig& o) const;
    bool operator!=(const AICommandConfig& o) const { return !(*this == o); }
    bool usesResources() const;
    DLLFACE bool isRoot() const;
};


// base class for targeting other blocks
struct ATargetBase : public AIAction {

    typedef std::pair<const Block*, AIMood> Target;

    vector<Target> targets;

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.hasWeapons; }

    ATargetBase(AI* ai) : AIAction(ai, LANE_TARGET, PRI_ALWAYS) {}

    DLLFACE virtual AIMood acceptTarget(const Block* target) const;

    DLLFACE Target testAcceptTarget(const Block *tgt) const;

    DLLFACE float targetDistanceMetric(float2 defPos, const Block *tgt) const;

    DLLFACE uint findSetTarget();
};


// block


#define FIREABLE_WEAPONS (Block::CANNON|Block::LASER|Block::LAUNCH|Block::LAUNCHER)


#endif // _AI_MODAPI_H
