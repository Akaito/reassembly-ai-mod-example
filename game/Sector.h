
#ifndef SECTOR_H
#define SECTOR_H

#include "Levels.h"

struct ShaderState;
struct RenderContext;

static const float kSectorSize = 6000; // !!!!!!!! impacts disk save format !!!!!!!!!!!

extern int kZoneMaxCommands;
extern int kZoneMaxSeeds;

extern int streamerThreadWork;

int compactSaveSlot(int slot, float* m_progress);

// WRAPS: set if clusters outside of window are unloaded from zone
// STREAM: set if sectors are unloaded to disk in chunks
// ASYNC: set if we use another thread for saves and loads
#define STREAMER_FLAGS(F)                       \
    F(WRAPS, 1<<0)                              \
    F(WINDOW, 1<<1)                             \
    F(STREAM, 1<<2)                             \
    F(ASYNC, 1<<3)                              \
    F(ROUND, 1<<4)                              \

DEFINE_ENUM(uchar, EStreamerFlag, STREAMER_FLAGS);

struct StreamerSpec {

    EStreamerFlag flags;

    // window
    float2 wpos;
    float2 wradius;

    // bounds are 0-bounds
    float2 bounds;

    StreamerSpec();
    StreamerSpec(float2 size);
    bool intersectCircleWindow(float2 pos, float rad) const;
};


struct SectorLimits {
    int commands   = 0;
    int seeds      = 0;
    int deadliness = 0;

    void updateForCluster(const BlockCluster *cl);

    int seedsRemaining() const { return kZoneMaxSeeds - seeds; }
    int commandsRemaining() const { return kZoneMaxCommands - commands; }

    string toString() const { return str_format("%2d/%2d commands, %3d/%3d seeds, %d deadliness", 
                                                commands, kZoneMaxCommands, seeds, kZoneMaxSeeds,
                                                deadliness); }
};

struct Sector final {
    
    const int2           pos;    // wrapped offset
    const int2           abspos; // unwrapped offset

    int                  sequence = -1;
    
    uint                 cachedSimStep = 0;
    uint                 saveSimStep   = 0; // 0 means not saved
    bool                 isLoaded;
    bool                 fromCache = false;
    bool                 fromFile  = false;
    Level                level;
    uint                 query;
    static uint          currentQuery;
    volatile bool        streamerReferenced = false;
    std::mutex           mutex;

    // Stream streamer
    Sector(int2 wp, int2 p, float size) : pos(wp), abspos(p)
    {
        isLoaded         = false;
        level.radius     = float2(size / 2.f);
        level.offset     = getOffset();
    }

    // One sector laoder
    Sector(float2 size) : pos(0, 0), abspos(0, 0)
    {
        isLoaded         = false;
        level.radius     = size / 2.f;
        level.offset     = getOffset();
    }

    ~Sector();

    string toString() const;

    float2 getOffset() const { return float2(abspos.x + 0.5f, abspos.y + 0.5f) * 2.f * level.radius; }
    float2 getRadius() const { return float2(level.radius); }

    string getSaveLevelFile() const;

    // Don't save a copy - just blit to disk
    int saveDirectFromZone(const GameZone* zone, bool verbose);
    int serializeDirectFromZone(const GameZone *zone, SaveSerializer &ss);

    int loadSaveToCache(const GameZone *zone, StreamerBase &streamer);

    bool loadLevelToZone(GameZone* zone, int number);
    bool markLoaded();

    int clearZone(GameZone *zone);
};

struct StreamerBase : public IDeletable {

protected:
    GameZone     *m_zone = NULL;
    SaveGame     *m_save = NULL;
    StreamerSpec  m_spec;       // window of in-zone clusters
    float         m_sectorSize = kSectorSize;
	vector<cpShape*> m_edges;
    vector<float2> m_boundary;
    int           m_loadProgress = 0;
    bool          m_saveEnabled = true;
    SectorLimits  m_limits;

    // segment shapes around the whole loaded section to keep stuff from wandering off
    void createEdges();

    bool loadUnloadClusters(FrameLogger *logger=NULL);

public:
	StreamerBase(const StreamerSpec &spec);
	virtual ~StreamerBase();
    bool isSaveEnabled() const { return m_save && m_saveEnabled; }
    void setSaveEnabled(bool en) { m_saveEnabled = en; }

    // return true if done loading
    virtual bool loadUnloadForPosition(float2 pos) {return true; };
    virtual void clearForUnload()=0;
    void updateLimits(float timestep);
    const SectorLimits& getLimits() { return m_limits; }
    
    virtual bool saveLoaded() = 0;
    virtual void asyncFlush() {}
    virtual Sector* getSectorNearest(float2 pos)=0;
    virtual Sector* getContainingSector(float2 pos)=0;
    virtual Sector* getSectorIndex(uint i)=0;
    virtual uint getSectorCount()=0;
    virtual void renderDebug(RenderContext &ctx);
    virtual string toString() const { return "StreamerBase"; }
    int getLoadPercent() const { return m_loadProgress; }
    virtual void onPlayerColorsChanged() {}

    void enableSave(SaveGame* sg);
    virtual bool loadSectorData(Sector &sec);
    virtual void onEnableSave(SaveGame* sg) { }
    virtual void onSetZone() {}

    void setZone(GameZone* z) { m_zone = z; onSetZone(); }

    const StreamerSpec &getSpec() const { return m_spec; }

    const Sector* getSectorNearest(float2 pos) const { return const_cast<StreamerBase&>(*this).getSectorNearest(pos); }

    // true if point is out of bounds
    bool intersectPointBounds(float2 point) const;
    float getSectorSize() const { return m_sectorSize; }
    float2 getBoundsRadius() const;
    int getBoundsEdges() const;
};

StreamerBase* CreateStreamer(const StreamerSpec &spec);

#endif // SECTOR_H

