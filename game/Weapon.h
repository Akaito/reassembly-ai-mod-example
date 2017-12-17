
#ifndef CANNON_H
#define CANNON_H

#include "Blocks.h"

inline float boost_val(float v, const float2 &b, float t)
{
    if (b.x >= 0)
        return v + min(b.y, b.x * t);
    else
        return v + max(b.y, b.x * t);
}

#define BOOST_VAL(T, S, X) (T->sb.boost ? boost_val((S).X, T->sb.boost->X, T->chargeTime) : (S).X)

struct SoundEvent;

struct Projectile final : public Body {

    cpShape*         shape;
    watch_ptr<Block> origin;
    Faction_t        faction;
    EExplosive       explosive;
    fixed16_u4       explodeRadius;
    fixed8_u4        size;
    float            endTime;
    float            health;
    uint             color;

    Projectile(Block* origin, const SerialCannon& sc, float charge);

    void setPosAngle(float2 pos, float angle);
    
    cpBody* getBody() { return &body; }

    // return true if collision physics should be simulated
    bool collide(float2 p, Block *bl);
    bool collideShield(float2 p, Block *bl);

    bool updateIsDead(GameZone *zone) const;

    static float getSize(const SerialCannon& sc, float charge);
    static Projectile* fromShape(cpShape* shape);
    float getLength(float v) const;

    void removeFromGameZone();

    ~Projectile()
    {
        removeFromGameZone();
    }

    string toString() const;
};

struct CannonBlock {

    fixed8_u2 turretRadius = 0.f;
    uchar  turretBarrelCount = 1;

private:

    uchar  turretShotIndex = 0;

    uchar shotInBurst = 0;
    float lastShotTime = 0.f;
    float nextShotTime = 0.f;

    ushort totalDeadliness = 0;

    Block*                 thisBlock = NULL;
    watch_ptr<const Block> lastBlock; // last block in the boost chain

    void addBooster(Block* b, uint queryVal);
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
    bool   isBoosted() const { return thisBlock != lastBlock.get(); }

    void onClusterInit();

    void update(float cannonAngle, float chargeBoost);
    void render(VertexPusherTri *tri, VertexPusherLine *line);
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

struct Turret {
    float                  angle       = 0.f;
    RenderAngle            renderAngle;
    float                  targetAngle = 0.f;
    float                  spreadAngle = 0.f;
};


#endif
