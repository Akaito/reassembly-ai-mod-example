
#ifndef RESOURCES_H
#define RESOURCES_H

#include "Spacer.h"

struct Block;

extern float kMinResPocketQuant;
extern float kMaxResPocketQuant;
static const float kResourcePocketZ   = -0.5f;

struct ResourcePocket : public Watchable {

    GameZone         *zone = NULL;
    float2            pos;
    RenderFloat2      renderPos;
    int               level;
    float             quantity;
    float             endTime;
    watch_ptr<Block>  owner;
    
    ResourcePocket(float quant);

    float getRadius(float time) const;
    float getBRadius() const;
    friend bool exist(const ResourcePocket *rp);

    float2 getPos() const { return pos; }

    void setPos(float2 p)
    {
        renderPos.set(p);
        pos = p;
    }

    void updateSetVel(float2 v)
    {
        pos += globals.simTimeStep * v;
    }
};

#endif
