
#ifndef PERSISTENT_BLOCK_H
#define PERSISTENT_BLOCK_H

#include "Blocks.h"

struct Block;
struct MetaZone;
struct Level;
struct Agent;

#define ENUM_OBJECTIVE_FLAGS(F)                 \
    F(STATION, 1<<0)                            \
    F(TARGET, 1<<1)                             \
    F(PROTOTYPE, 1<<2)                          \
    F(UNIQUE, 1<<3)                             \
    F(ACTIVATED, 1<<4)                          \
    F(DESTROYED, 1<<5)                          \
    F(WORMHOLE, 1<<6)                           \
    F(HIDDEN, 1<<7)                             \
    F(WAS_TARGET, 1<<8)                         \
    F(VISIBLE, 1<<9)                            \
    F(OBJECTIVE, 1<<10)                         \
    F(MESSAGE, 1<<11)                           \
    F(TELESTATION, 1<<12)                       \
    F(CONTESTED, 1<<13)                         \

    
DEFINE_ENUM(uint, EObjective, ENUM_OBJECTIVE_FLAGS);

struct MapObjective final : public Watchable {

    typedef int VisitEnabled;

private:
    // serialized fields
    float2        position;     // wrapped position
    float2        destination;
    EObjective    flags;        // type of objective
    uint          ident = 0;    // used to associate with a particular block
    int           faction = 0;
    lstring       message;

    const MetaZone*   mz = NULL;
    watch_ptr<Block>  block;

public:
    MapObjective() {}
    MapObjective(const MetaZone* m, float2 pos, EObjective flgs) : position(pos), flags(flgs), mz(m){ }
    MapObjective(const MetaZone* m, Block *bl, EObjective flgs) : flags(flgs), mz(m) { onAssociate(bl); }
    ~MapObjective();

    void reset(const MetaZone* m, float2 pos, EObjective flgs)
    {
        ASSERT((!flags || isDestroyed()) && !block);
        mz = m;
        position = pos;
        flags = flgs;
    }

    void setPos(float2 pos) { position = pos; }
    void setDest(float2 dest) { destination = dest; }
    float2 getDest() const { return destination; }

    void   setMetaZone(const MetaZone* m);
    const MetaZone *getMetaZone() const { return mz; }

    Block* getBlock()     const { return block.get(); }
    friend bool exist(const MapObjective *mp);
    float2 getPos()       const;
    float2 getWrappedPos() const;
    float2 getRenderPos(const View &view) const;
    float  getRenderBRadius() const; // bradius of cluster, or zero
    uint   getIdent() const { return ident; }
    
    bool isActivated() const { return flags&EObjective::ACTIVATED; }
    bool isDestroyed() const { return flags&EObjective::DESTROYED; }
    bool isStation() const { return flags&EObjective::STATION; }
    bool isContested() const { return flags&EObjective::CONTESTED; }
    bool isObjective() const { return flags&EObjective::OBJECTIVE; }
    bool isTarget() const { return flags&EObjective::TARGET; }
    bool isHidden() const;
    bool isWormhole() const { return flags&EObjective::WORMHOLE; }
    EObjective getFlags() const { return flags; }

    void markVisible() { flags |= EObjective::VISIBLE; }
    void updateContested();
    
    void onBlockKilled();
    void onBlockDeactivated();
    void onBlockRemoved();
    void markDestroyed();

    void   setFaction(uint fac) { faction = fac; }
    int    getFaction() const { return faction; }
    void   setMessage(lstring str) { message = str; }
    string  toString(int width=-1) const;
    lstring getMessage() const { return message; }
    Agent* getAgent();
    const Agent* getAgent() const { return const_cast<MapObjective*>(this)->getAgent(); }

    bool operator==(const MapObjective& o) const
    {
        return position == o.position && flags == o.flags && ident == o.ident;
    }

    // main transitions:

    // associate with a block generated elsewhere, but of same type as our block
    void onAssociate(Block* bl);  // MapObjective copies to block

    // generate new live block, and any attachements
    bool needsLevelFill(const Level *level);
    void onLevelFill(GameZone* zone, const Level* level);

    bool canActivate() const;

    // player manually activated: transform or disappear 
    void onActivate();

    // player did something to make visible
    void onReveal(GameZone *zone);

    // agent captured territory
    bool onLost();
    
    void updateForSerialization();

    template <typename V>
    bool accept(V& vis)
    {
        return (vis.VISIT(position) &&
                vis.VISIT(flags) &&
                vis.VISIT(ident) &&
                vis.VISIT(faction) &&
                vis.VISIT(message) &&
                vis.VISIT_SKIP(int, "blockReward") &&
                vis.VISIT_SKIP(bool, "instantiated") &&
                vis.VISIT_SKIP(bool, "destroyed") &&
                vis.VISIT_SKIP(SerialBlock, "block"));
    }
};


#endif // PERSISTENT_BLOCK_H

