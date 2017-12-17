
#ifndef BLOCKTOOL_H
#define BLOCKTOOL_H

#include "GameZone.h"
#include "Blocks.h"

struct BlockTool {

    GameZone     *zone;
    BlockCluster *cluster;

    static float2 findEmptySpace(GameZone* zone, float radius, float2 center, float rmin, float rmax)
    {
        //ASSERT(radius < rmax);
        for (;;)
        {
            for (uint i=0; i<10; i++)
            {
                float2 p = center + randpolar(rmin, rmax);
                if (!zone->intersectCircleClustersNearest(p, radius))
                    return p;
            }
            rmax *= 2;
        }
    }
    
    static float2 findEmptySpaceSafely(GameZone* zone, float radius, float2 center, float rmin, float rmax)
    {
        //ASSERT(radius < rmax);
        for (;;)
        {
            for (uint i=0; i<10; i++)
            {
                float2 p = center + randpolar(rmin, rmax);
                if (!zone->intersectCircleClusterCirclesNearest(p, radius))
                    return p;
            }
            rmax *= 2;
        }
    }

    static float2 findEmptySpace(GameZone* zone, float radius, float2 center, float2 rmax)
    {
        ASSERT(radius < min_dim(rmax));
        for (;;)
        {
            for (uint i=0; i<10; i++)
            {
                float2 p = center + randrange(-rmax+float2(radius), rmax-float2(radius));
                if (!zone->intersectCircleClustersNearest(p, radius))
                    return p;
            }
            rmax *= 2;
        }
    }



    BlockTool(GameZone* zone_, BlockCluster *c)
    {
        zone    = zone_;
        cluster = c;
    }

    BlockTool(GameZone* zone_, Block* b)
    {
        if (!b->cluster) {
            b->createCluster();
        }
        zone    = zone_;
        cluster = b->cluster;
    }

    BlockTool& place(float2 p, bool station=true)
    {
        cluster->addToGameZone(zone, p);
        return *this;
    }

    BlockTool& setFaction(Faction_t faction)
    {
        ASSERT(cluster->command);
        cluster->command->sb.command->faction = faction;
        return *this;
    }

    BlockTool& placeRandomlySafely(float2 center, float rmin, float rmax)
    {
        cluster->init();
        float2 p = findEmptySpaceSafely(zone, cluster->getBRadius(), center, rmin, rmax);
        ASSERT(p != float2(0));
        cluster->setAngle(randangle());
        cluster->addToGameZone(zone, p);
        return *this;
    }
    
    BlockTool& placeRandomly(float2 center, float rmin, float rmax)
    {
        cluster->init();
        float2 p = findEmptySpace(zone, cluster->getBRadius(), center, rmin, rmax);
        ASSERT(p != float2(0));
        cluster->setAngle(randangle());
        cluster->addToGameZone(zone, p);
        return *this;
    }

    BlockTool& placeRandomly(float2 center, float2 rmax)
    {
        cluster->init();
        float2 p = findEmptySpace(zone, cluster->getBRadius(), center, rmax);
        ASSERT(p != float2(0));
        cluster->setAngle(randangle());
        cluster->addToGameZone(zone, p);
        return *this;
    }


};


#endif // BLOCKTOOL_H
