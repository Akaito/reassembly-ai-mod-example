
#pragma once

#include "Blocks.h"

inline float boost_val(float v, const float2 &b, float t)
{
    if (b.x >= 0)
        return v + min(b.y, b.x * t);
    else
        return v + max(b.y, b.x * t);
}

#define BOOST_VAL(T, S, X) ((T && T->sb.bt->boost) ? boost_val((S).X, T->sb.bt->boost->X, T->chargeTime) : (S).X)


extern bool kProjectileQueryOptimization;
struct SoundEvent;

bool spawn_fragment(Projectile *base, const SerialCannon &sc, float2 pos, float angle,
                    float2 vel, float boost, uint faction, GameZone *zone);

struct Projectile final : public Body {

    union {
        cpShape        shape;
        cpCircleShape  circle_shape;
        cpSegmentShape seg_shape;
    };
    
    watch_ptr<Block> origin;
    EExplosive       explosive;
    float            explodeRadius;
    float            size;
    float            endTime;
    float            health;
    uint             color;
    float            chargeBoost = 1.f;
    const SerialCannon *fragment = NULL;

    Projectile();
    void init(const SerialCannon& sc, float charge);

    void setPosAngle(float2 pos, float angle);
    
    cpBody* getBody() { return &body; }
    Faction_t getFaction() const { return shape.group2; }

    // return true if collision physics should be simulated
    bool collide(float2 p, Block *bl);
    bool collideShield(float2 p, Block *bl);

    bool updateIsDead(GameZone *zone);

    static float getSize(const SerialCannon& sc, float charge);
    static Projectile* fromShape(cpShape* shape);
    float getLength(float v) const;

    void removeFromGameZone();

    ~Projectile();

    string toString() const;

    // static void pool_purge();
    // static Projectile *pool_alloc();
    static void pool_free(Projectile *pr) { delete pr; }
};

struct CannonBlock {

    float turretRadius      = 0.f;
    uchar turretBarrelCount = 1;

private:

    uchar turretShotIndex = 0;

    uchar shotInBurst = 0;
    float lastShotTime = 0.f;
    float nextShotTime = 0.f;

    uint totalDeadliness = 0;

    Block*                 thisBlock = NULL;
    watch_ptr<const Block> lastBlock; // last block in the boost chain

    void addBooster(Block* bl);
    void resetMuzzleOffset();

public:

    float2       muzzleOffset;
    SerialCannon boostedStats;

    CannonBlock(Block* b);

    void onAttach()
    {
        shotInBurst = 0;
    }

    // place where bullets come out, in cluster space
    float2 getBarrelOffset() const;
    float  getProjectileSize() const;
    int    getDeadliness() const { return totalDeadliness; }

    void onClusterInit();

    void update(float cannonAngle, float chargeBoost);
};

struct Laser {
    float  firing  = 0.f;
    float2 start, end;      // sim (relative)
    RenderFloat2 renderStart;
    RenderFloat2 renderEnd;
    bool   lastEnabled = false;
    bool   hitting     = false;
    bool   shooting    = false;

    float lastPulseTime = 0.f;
    float nextPulseTime = 0.f;
    uchar pulseInBurst  = 0;
    bool  pulseEnabled = true;
    
    mutable watch_ptr<SoundEvent> sound;
};

struct Explosion {
    GameZone*        zone = NULL;
    watch_ptr<Block> origin;
    float2           expPos;
    float2           expVel;
    float            expRad = 0.f;
    float            expDmg = 0.f;
    bool             dead = false;
    Faction_t        faction = -1;

    void update();
};

