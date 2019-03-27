
#ifndef LEVELS_H
#define LEVELS_H

#include "SerialCore.h"
#include "BlockTool.h"

#define ASTEROID_FLAGS(F)                       \
    F(EXPLOSIVE, 1<<0)                          \
    F(PENROSE, 1<<1)                            \
    F(SQUARE, 1<<2)                             \
    F(HEXAGON, 1<<3)                            \
    F(TRIANGLE, 1<<4)                           \
    F(OCTAGON, 1<<5)                            \
    F(UNIFORM_SIZE, 1<<6)                       \
    F(UNIFORM_TYPE, 1<<7)                       \
    F(SIZE_IMPORTANT, 1<<8)                     \
    F(NO_RANDOMIZE_FLAGS, 1<<9)                 \
    
DEFINE_ENUM(uint64, EAsteroidFlags, ASTEROID_FLAGS);

struct OneMod;

bool parseNameAuthor(string sname, ShipData& data);

struct SerialCluster {

    // serialized fields
    float2               position;
    float2               velocity;
    float                angle  = 0;
    lstring              name;
    uint                 hash = 0;
    ShipData             data;
    vector< copy_ptr<SerialBlock> > blocks;
    vector< copy_ptr<SerialCluster> > subclusters;

    // runtime fields
    lstring         path;
    const OneMod   *mod = NULL;

    BlockCluster* deserialize(BlockCluster *bc=NULL, float2 offset=float2()) const;
    BlockCluster* deserialize_move(BlockCluster *bc=NULL, float2 offset=float2());
    SerialCluster* clone() const { return new SerialCluster(*this); }

    // helper fuctions: return this on success
    SerialCluster *loadCluster(SaveParser &sp, lstring name_=lstring());
    SerialCluster *serialize(const BlockCluster *c);
    SerialCluster *overwriteFromId();

    void         setFaction(int faction);
    SerialBlock *command() const;
    int          deadliness() const;
    size_t       size() const { return blocks.size(); }
    size_t       getSizeof() const;

    void clear();

    typedef int VisitEnabled;

    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(position) &&
            vis.VISIT(velocity) &&
            vis.VISIT(angle) &&
            vis.VISIT(name) &&
            vis.VISIT(hash) &&
            vis.VISIT(data) &&
            vis.VISIT(blocks) &&
            vis.VISIT(subclusters);
    }

    bool operator==(const SerialCluster &o) const
    {
        return position == o.position &&
            velocity == o.velocity &&
            angle == o.angle &&
            name == o.name &&
            data == o.data &&
            (hash ? (hash == o.hash) : (blocks == o.blocks)) &&
            subclusters == o.subclusters;
    }
};

struct Level {
    
    // stored attributes
    int                               number;
    vector< copy_ptr<SerialCluster> > clusters;
    vector< BlockCluster* >           cachedClusters;
    float2                            offset; // offset of center from global origin
    float2                            radius; // bounding radius if contained clusters
    float3                            viewpos; // camera position (for editor)
    int                               time;

    Level() { reset(); }
    ~Level() { clear(); }

    void reset()
    {
        number = -1;
        radius = float2(4000.f);
        clear();
    }

    void clear();
    
    int expandToCache();
    int injectCachedToZone(GameZone *zone);

    int extractFromZone(GameZone* zone);
    int writeToFile(const string& file) const;
    string serialize(int *saved=NULL) const;

    // don't save a copy in memory
    int saveDirectFromZone(const GameZone* zone, SaveSerializer &sr) const;

    void findPortsForRoot(GameZone *zone, vector<Port> &freePorts) const;
    Block *addBlueprint(GameZone* zone, const BlockCluster *bp, vector<Port> *freePorts) const;
    Block *addBlueprint(GameZone* zone, lstring name, vector<Port> *freePorts) const;
    Block *addCluster(GameZone* zone, BlockCluster *cl, vector<Port> *freePorts) const;
    
    int fillWithAsteroids(GameZone* zone, uint64 flags, float fillPerc, int2 size) const;

    void serialize(SaveSerializer& sr, const BlockCluster *bc) const;
    void serializeHeader(SaveSerializer &sr, int t=0) const;
    
    bool placeRandomly(GameZone *zone, BlockCluster *cl, float angle=-1.f) const;
};


#define FLEETSPEC_FLAGS(F)                           \
    F(MOBILE,      1<<0)                             \
    F(PYRAMID,     1<<2)                             \
    F(SELECTION,   1<<3)                             \

DEFINE_ENUM(uchar, EFleetspecFlags, FLEETSPEC_FLAGS);

#define FLEETSPEC_FIELDS(F)                                             \
    F(int,                deadliness, 0)                                \
    F(int,                faction, 0)                                   \
    F(int,                count, 0)                                     \
    F(EFleetspecFlags, flags, EFleetspecFlags::PYRAMID|EFleetspecFlags::MOBILE) \
    F(Feature_t,          features, 0)                                  \
    F(BlueprintList,      allprints, BlueprintList())                  \
    

struct Fleetspec {
    DECLARE_SERIAL_STRUCT_CONTENTS(Fleetspec, FLEETSPEC_FIELDS, SERIAL_PLACEHOLDER);
    enum Flags : EFleetspecFlags::value_type { FLEETSPEC_FLAGS(SERIAL_TO_ENUM) };

    Fleetspec(int fac, int dead, int cnt) :
        deadliness(dead), faction(fac), count(cnt) { }

    BlockList     create(SaveGame *save) const;
    BlueprintList compose(SaveGame *save) const;
};

int deployFleet(BlockList &fleet, GameZone *zone, float2 pos, float angle, float2 rad, Block *leader=NULL);
inline int deployFleet(BlockList &fleet, GameZone *zone, float2 pos, float2 rad, Block *leader=NULL)
{
    return deployFleet(fleet, zone, pos, randangle(), rad, leader);
}
const BlockCluster* selectShip(SaveGame *save, int faction, int deadly_low, int deadly_high);

void addFactionDebris(GameZone* zone, Faction_t faction, Faction_t faction1, uint totalBlocks, float2 pos, float radius);
void damageCluster(BlockCluster* cl, Block* preserve, float ratio, float maxdeadly);

#endif
