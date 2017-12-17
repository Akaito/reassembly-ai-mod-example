
#ifndef BLOCKPATTERN_H
#define BLOCKPATTERN_H

#include "Blocks.h"

// pattern for circular queries

inline BlockPattern patternForAll(float2 rpos, float rad)
{
    BlockPattern pat;
    pat.position = rpos;
    pat.radius = rad;
    return pat;
}

inline BlockPattern patternForTractor(float2 rpPos, float range)
{
    BlockPattern pat;
    pat.position   = rpPos;
    pat.radius     = range;
    pat.features   = Block::TRACTOR;
    pat.hasCommand = true;
    return pat;
}

inline BlockPattern patternForAllies(const BlockCluster* sc, float rad=0)
{
    BlockPattern pat;
    pat.position      = sc->getAbsolutePos();
    pat.radius        = rad > epsilon ? rad : sc->getSensorRadius();
    pat.faction       = sc->getFaction();
    pat.features      = Block::COMMAND;
    pat.hasCommand    = 1;
    return pat;
}

inline BlockPattern patternForPlanting(const BlockCluster *sc)
{
    BlockPattern pat;
    pat.position   = sc->getAbsolutePos();
    pat.radius     = sc->getSensorRadius();
    pat.features   = Block::ENVIRONMENTAL;
    return pat;
}

inline BlockPattern patternForKillPlants(const Block *cmd)
{
    BlockPattern pat;
    pat.position = cmd->getClusterPos();
    pat.radius   = cmd->getSensorRadius();
    pat.features = Block::SEED;
    pat.faction  = -cmd->getFaction();
    return pat;
}

inline BlockPattern patternForTarget(const Block *cmd, Feature_t features, float radius=0.f)
{
    BlockPattern pat;
    pat.position      = cmd->getClusterPos();
    pat.radius        = nearZero(radius) ? cmd->getSensorRadius() : radius;
    pat.features      = features;
    pat.antifeatures  = Block::ENVIRONMENTAL;
    pat.faction       = -cmd->getFaction();
    pat.hasCommand    = 1;
    return pat;
}

inline BlockPattern patternForDTurret(const Block *turret, float radius)
{
    ASSERT(exist(turret));
    BlockPattern pat;
    pat.position      = turret->getAbsolutePos();
    pat.radius        = radius;
    if (turret->sb.isWeapon())
    {
        pat.features      = Block::COMMAND|Block::EXPLODE;
        pat.antifeatures  = Block::ENVIRONMENTAL; // don't shoot at explosive asteroids
        pat.faction       = -turret->getFaction();
    }
    else
    {
        pat.features = Block::COMMAND;
        pat.faction = turret->getFaction();
    }
    return pat;
}

inline BlockPattern patternForAssemble(const BlockCluster *sc, uint ident)
{
    BlockPattern pat;
    pat.position      = sc->getAbsolutePos();
    pat.radius        = sc->getSensorRadius(); // TODO maybe separate tractor radius?
    // want to assemble with UNGROW blocks, for metamorphosis, etc.
    pat.antifeatures  = Block::COMMAND|Block::GROW|Block::AUTOLAUNCH|Block::PERSISTENT;
    pat.ident         = ident;
    pat.hasCommand    = -1;
    return pat;
}

struct SegmentPattern {
    float2              start;
    float2              end;
    float               width         = 0.f;
    Faction_t           weaponFaction = 0; // faction of firer - collides with enemies
    Faction_t           ignoreFaction = 0; // ignore collisions with this faction
    Feature_t           features      = 0;
    float               minRadius     = 0;
    const BlockCluster *ignoreCl      = NULL;
    cpLayers            layers        = LR_BLOCK|LR_SPACER;

    SegmentPattern() {}
    
    SegmentPattern(float2 s, float2 e, float minrad) : start(s), end(e), minRadius(minrad) {}
    
    SegmentPattern(float2 s, float2 e, float minrad, uint weaponfac, uint targetfac) :
        start(s), end(e), weaponFaction(weaponfac), ignoreFaction(targetfac), minRadius(minrad) {}

    SegmentPattern(float2 s, float2 e, float minrad, const BlockCluster *igncl, uint tfac, Feature_t ftrs=0) :
        start(s), end(e),ignoreFaction(tfac), features(ftrs), minRadius(minrad), ignoreCl(igncl) {}

    bool allowCached(const SegmentPattern &o) const
    {
        return (intersectPointCircle(start, o.start, minRadius) &&
                intersectPointCircle(end, o.end, minRadius) &&
                weaponFaction == o.weaponFaction &&
                ignoreFaction == o.ignoreFaction &&
                features == o.features &&
                isBetween(minRadius, 0.5f * o.minRadius, 2.f * o.minRadius) &&
                ignoreCl == o.ignoreCl &&
                layers == o.layers);
    }
};

#endif
