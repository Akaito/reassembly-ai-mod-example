
#ifndef SPACER_H
#define SPACER_H

#include "SerialBlock.h"

enum CollisionLayer {
    LR_BLOCK      = 1<<0,
    LR_PROJECTILE = 1<<1,
    LR_SHIELD     = 1<<2,
    LR_SPACER     = 1<<3,       // invisible cluster hull
    LR_EDGE       = 1<<4,      // invisible edge of loaded sector
};

enum CollisionGroup {
    GROUP_SHIELD     = 1,
    GROUP_PROJECTILE = 2,
};

enum CollisionWhichLayers {
    SHIELD_LAYERS     = LR_SHIELD|(1<<5),
    BLOCK_LAYERS      = LR_BLOCK|(1<<6)|LR_EDGE,
    EXPLODE_LAYERS    = LR_BLOCK|(1<<5)|(1<<6)|LR_EDGE,
    SPACER_LAYERS     = LR_SPACER|(1<<6)|LR_EDGE,
    PROJECTILE_LAYERS = LR_PROJECTILE|(1<<5)|(1<<6),
};

enum CollisionType {
    COLLISION_BLOCK        = 1,
    COLLISION_PROJECTILE,
    COLLISION_SHIELD,
    COLLISION_SPACER,
    COLLISION_EDGE,
    COLLISION_GHOST,
};

struct Body : public IDeletable {
    
    GameZone*      zone = NULL;

protected:
    cpBody        body;

    RenderFloat2   renderPos;
    RenderAngle    renderAngle;

public:

    virtual ~Body();

    // call this just AFTER cpSpaceStep
    void advanceUpdate()
    {
        ASSERT_UPDATE_THREAD();
        renderAngle.advanceUpdate(getAngle());
        renderPos.advanceUpdate(getPos());
    }

    // call before drawing anything
    void advanceRender(float interpolant)
    {
        //ASSERT_MAIN_THREAD();
        renderPos.advanceRender(interpolant);
        renderAngle.advanceRender(interpolant);
    }

    void setRenderPosAngle(float2 pos, float angle)
    {
        renderPos.set(pos);
        renderAngle.set(angle);
    }

    float2 getRenderPos()   const { return renderPos.render; }
    float  getRenderAngle() const { return renderAngle.render; }

    float2 getPos() const    { return f2v(body.p); }
    float2 getVel() const    { return f2v(body.v); }
    float  getAngle() const  { return body.a; }
    float2 getRot() const    { return f2v(body.rot); }
    float  getAngVel() const { return body.w; }

    void setVel(float2 p) { body.v = cv(p); }
    void setAngVel(float p) { body.w = p; }

    float getMass() const    { return body.m; }
    float getMoment() const  { return body.i; }

    // force is per second
    // offset is relative to cluster (not rotated)
    void applyForce(float2 force, float2 offset=f2()) { cpBodyApplyForce(&body, cv(force), cv(offset)); }
    void applyImpulse(float2 force, float2 offset=f2()) { cpBodyApplyImpulse(&body, cv(force), cv(offset)); }
    void resetForces() { cpBodyResetForces(&body); }
};



// encapsulate an invalidate-able convex hull shape
struct SpacerShape {

    float           lastInvalidateTime = -1000.f;
    bool            dirty   = true;
    vector<cpVect>  vertices;
    cpShape        *shape   = NULL;
    BlockCluster   *cluster = NULL;

    ~SpacerShape();
    static SpacerShape* fromShape(cpShape* shape);

    bool isValid() const;
    bool isEnabled() const { return shape && cpShapeGetSpace(shape); }
    void invalidate();
    void invalidateNow();       // expensive!
    bool queryValid();

    size_t getSizeof() const;
    Feature_t getFeatureUnion() const;
};

inline void bodyCountShapesCallback(cpBody *body, cpShape *shape, void *data)
{
    (*(int*)data)++;
}

inline int cpBodyCountShapes(cpBody *body)
{
    int count = 0;
    cpBodyEachShape(body, bodyCountShapesCallback, &count);
    return count;
}

inline void cpBodyRemove(cpBody* body)
{
    ASSERT(cpBodyCountShapes(body) == 0);
    cpSpace *space = cpBodyGetSpace(body);
    if (space)
        cpSpaceRemoveBody(space, body);
}

inline bool cpShapeRemove(cpShape* shape)
{
    cpSpace *space = cpShapeGetSpace(shape);
    if (space)
        cpSpaceRemoveShape(space, shape);
    return space;
}

inline bool cpShapeRemoveFree(cpShape* shape)
{
    if (!shape)
        return false;
    bool ret = cpShapeRemove(shape);
    cpShapeFree(shape);
    return ret;
}

inline bool cpShapeRemoveFreeNull(cpShape* &shape)
{
    bool ret = cpShapeRemoveFree(shape);
    shape = NULL;
    return ret;
}

inline bool cpShapeRemoveIfChangeBody(cpShape* shape, cpBody* body)
{
    if (shape->body != body)
    {
        cpShapeRemove(shape);
        shape->body = body;
        return true;
    }
    return false;
}

inline bool cpShapeRemoveAndUpdateBody(cpShape* shape, cpBody* body)
{
    if (!shape)
        return false;
    bool changed = cpShapeRemove(shape);
    if (shape->body != body) 
    {
        shape->body = body;
        return true;
    }
    return changed;
}

#endif
