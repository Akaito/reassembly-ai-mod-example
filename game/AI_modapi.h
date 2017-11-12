
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

    uint noAction(const char* reason);

    string toString() const;

    virtual void render(void* line) const { line; }

    // helper
    const BlockCluster* getCluster() const;
    float2 getClusterPos() const;
    float  getClusterBRadius() const;

    bool isDestObstructed(float2 dest, uint ignoreFaction, float avoidDebrisRatio) const;

    // repath if we hit something (bigger than kAvoidDebrisRadiusRatio)
    bool needsRepath() const;

    AIMood isActivelyHostile(const Block* target) const;

    float getWaypointRadius() const;
    
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
    bool isRoot() const;
};


// base class for targeting other blocks
struct ATargetBase : public AIAction {

    typedef std::pair<const Block*, AIMood> Target;

    vector<Target> targets;

    static bool supportsConfig(const AICommandConfig& cfg) { return cfg.hasWeapons; }

    ATargetBase(AI* ai) : AIAction(ai, LANE_TARGET, PRI_ALWAYS) {}

    virtual AIMood acceptTarget(const Block* target) const;

    Target testAcceptTarget(const Block *tgt) const;

    float targetDistanceMetric(float2 defPos, const Block *tgt) const;

    uint findSetTarget();
};


// block


#define FIREABLE_WEAPONS (Block::CANNON|Block::LASER|Block::LAUNCH|Block::LAUNCHER)


#endif // _AI_MODAPI_H
