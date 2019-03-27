
#ifndef GALAXY_H
#define GALAXY_H

#include "Levels.h"

struct Agent;
struct StreamerSpec;
struct OneMod;

DECLARE_ENUM(uint, EObjective);

typedef vector< pair<Faction_t, vector<float2> > > RegionFleets;

#define GALAXY_REGION_FIELDS(F)                 \
    F(int, ident, 0)                            \
    F(uint, color, 0)                           \
    F(Faction_t, faction, 0)                    \
    F(uint, count, 1)                           \
    F(float2, position, float2())               \
    F(float2, radius, float2(1.f))              \
    F(float2, totalSize, float2())              \
    F(int, type, 0)                             \
    F(RegionFleets, fleets, RegionFleets())     \
    F(vector<int2>, fleetCount, vector<int2>(1, int2(8, 15))) \
    F(float, fleetFraction, 0.75f)              \
    F(vector<lstring>, fortress, vector<lstring>()) \
    F(int2, fortressCount, int2(3, 6))              \
    F(float2, fortressRadius, float2(500.f))    \
    F(vector<int>, ambient, vector<int>())      \
    F(vector< vector<lstring> >, unique, vector< vector<lstring> >()) \
    F(float, uniqueFraction, 0.25f)             \
    F(float2, asteroidDensity, float2(0.f, 0.4f)) \
    F(int2, asteroidSize, int2())               \
    F(EAsteroidFlags, asteroidFlags, EAsteroidFlags()) \
    F(vector< copy_ptr<GalaxyRegion> >, subregions, vector< copy_ptr<GalaxyRegion> >()) \


// hierarchical structure definition
struct GalaxyRegion {

    DECLARE_SERIAL_STRUCT_CONTENTS(GalaxyRegion, GALAXY_REGION_FIELDS, SERIAL_PLACEHOLDER);

    string source;
    const OneMod *mod = NULL;
    GalaxyRegion *parent = NULL;
    
    int size() const
    {
        int size = 1;
        foreach (const copy_ptr<GalaxyRegion> &reg, subregions) {
            size += reg->size();
        }
        return size;
    }

    const GalaxyRegion *getById(int nm) const;
    const GalaxyRegion* getByFaction(Faction_t faction) const;

    void expandOnLoad();
};

// instance definition
struct GalaxyMap {

    struct Cell {
        int           region = 0;
        float         centrality = 0.f; // 0-1, 0 at center of region
        uchar         visited  = 0;
        mutable uchar valid    = 0;

        uchar         is_center = 0;

        Cell() {}
        Cell(int r, float c) : region(r), centrality(c) {}

        typedef int VisitIndexedEnabled;
        
        template <typename V>
        bool accept(V& vis)
        {
            return (vis.VISIT(region) &&
                    vis.VISIT(centrality) &&
                    vis.VISIT(visited) &&
                    vis.VISIT(valid));
        }
    };

    float2       radius;
    int          width = 0;
    vector<Cell> cells;

    int          version = 0;
    mutable int  savedVersion = -1;

    template <typename V>
    bool accept(V& vis)
    {
        // !!!!!!!!!!! WARNING !!!!!!!!!!!!!! only used for parsing
        return (vis.VISIT(radius) &&
                vis.VISIT(width) &&
                vis.VISIT(cells));
    }

    // unwrapped
    int2 getCell(float2 pos) const
    {
        int2 cell = floor_int((float) width * pos / (2.f * radius.x));
        return cell;
    }

    float2 getPos(int2 cell) const 
    {
        float2 pos = (2.f * radius.x * (float2(cell) + float2(0.5f))) / (float) width;
        return pos;
    }
        
    Cell &at(int2 cell) 
    {
        return cells[getIndex(cell)];
    }

    const Cell& at(int2 cell) const { return const_cast<GalaxyMap*>(this)->at(cell); }
    const Cell& atPos(float2 cell) const { return const_cast<GalaxyMap*>(this)->atPos(cell); }

    Cell& atPos(float2 pos)
    {
        return at(getCell(pos));
    }

    uint getIndex(int2 cell) const
    {
        const int2 wcell = modulo(cell, width);
        const uint indx  = wcell.y * width + wcell.x;
        ASSERT(indx < cells.size());
        return indx;
    }

    float2 indexPos(int idx) const
    {
        return getPos(indexCell(idx));
    }

    int2 indexCell(int idx) const
    {
        ASSERT(idx < cells.size());
        return int2(idx % width, idx / width);
    }
};

// generates levels
struct MetaZone final {

    friend struct MapGenerator;
    
private:
    MapObjective         *m_firstStation = NULL; // only used during generation
    copy_ptr<GalaxyRegion> m_regions;
    GalaxyMap             m_map;
    SaveGame             *m_save = NULL;
    GLTexture             m_mapTex;
    bool                  m_wraps = true;
    LoadProgress          m_progress;
    mutable vector<MapObjective*> m_needsFill;

    void generateRegion(const GalaxyRegion* region, SaveGame& save,
                        spatial_hash<bool>& stuff, vector<float2> &points,
                        float2 parentPos, float2 parentRad, int depth);
    bool addAgent(Agent *agent);
    int addAgentRegion(Agent *agent);
    string getMapFile() const;
    void setGenStep(int step);
public:

    MetaZone();

    SaveGame *getSave() const { return m_save; }
    
    const GalaxyRegion *getRegion(float2 pos) const { return m_regions->getById(m_map.atPos(pos).region); }
    const GalaxyRegion &getRootRegion() const { return *m_regions; }

    void getFactionList(vector<Faction_t> &facs, const GalaxyRegion *region=NULL) const
    {
        if (!region)
            region = m_regions.get();
        vec_add(facs, region->faction);
        for_ (reg, region->subregions)
            getFactionList(facs, reg.get());
    }
    
    int getRegionFleetDeadliness(float2 pos) const;
    int getRegionRandomizedFaction(float2 pos) const;
    vector<Faction_t> getRegionNearbyFactions(float2 pos) const;
    int getControllingFaction(float2 pos) const;

    float getCentrality(float2 pos) const;
    float2 getRadius() const { return m_map.radius; }

    GLTexture getMapTexture() const;
    uint getMapColor(float2 pos) const;
    const GalaxyMap &getMap() const { return m_map; }
    GalaxyMap &getMap() { return m_map; }
    int       markVisited(float2 pos, float rad); // return true if new sectors were marked
    bool      isVisited(float2 pos) const;
    
    void fillLevel(GameZone* zone, const Level* level) const;
    void fillCell(GameZone* zone, const Level* level, const GalaxyMap::Cell &cell, EObjective oflags) const;
    bool onWarpUpdate(Level * level) const;
    bool isSaveValid(int2 pos) const { return m_map.at(pos).valid; }
    MapObjective* generate(SaveGame &save); // return first station
    MapObjective* generate_foredit(SaveGame &save);
    float getLoadPercent() const { return m_progress.getPercent(); }
    bool loadRegions(SaveGame *save);
    void setRegions(const GalaxyRegion &reg);
    bool reloadRegions();
    bool loadMap(SaveGame *save);
    bool saveMap() const;

    bool hasMap() const { return !m_map.cells.empty(); }
    float2 getWrappedPos(float2 pos) const { return m_wraps ? modulo(pos, getWrapSize()) : pos; }

    void setMapSize(float2 ws);
    float2 getWrapSize() const;

    void onFactionsUpdate(int2 cell, const unordered_map<int, int> &factions);
    void onGalaxyHeartbeat(float timestep, const StreamerSpec &spec);
    void onClusterInit(BlockCluster *cl);
    void onPlayerColorChanged() const;

    GalaxyRegion *createAgentRegion();
    Agent* getAgent(int index) const;
};

void globalReloadRegions();
GameState *CreateGSGalaxyOptions(int slot, int seq, GameState *mm);

#endif
