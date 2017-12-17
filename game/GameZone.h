
#ifndef GAMEZONE_H
#define GAMEZONE_H

#include "Types.h"
#include "Render.h"
#include "Sound.h"

#define ZONE_FEATURES(F)                         \
    F(NONE,       0)                             \
    F(RESOURCES,  1<<0)                          \
    F(SPACERS,    1<<1)                          \
    F(BACKGROUND, 1<<2)                          \
    F(PARTICLES,  1<<3)                          \
    F(UNDO,       1<<6)                          \
    F(SELECTION,  1<<7)                          \
    F(OFFLINE,    1<<8)                          \
    F(UNDO_ALL,   1<<9)                          \
    F(DEAD,       1<<10)                         \
    F(NODEBRIS,   1<<11)                         \

DEFINE_ENUM(uint64, EZoneFeatures, ZONE_FEATURES);


#define UIMODES(F)                              \
    F(FLY, 0)                                   \
    F(COMMAND, 1)                               \
    F(CONSTRUCT, 2)                             \
    F(VIEW, 5)                                  \
    F(FLYCOMMAND, 6)                            \

DEFINE_ENUM(uchar, UIMode, UIMODES);

inline bool isUIModeInShip(UIMode mode)
{
    return mode == UIMode::FLY || mode == UIMode::FLYCOMMAND;
}

struct UndoRegion {
    float2                              offset;
    float2                              radius;
    bool                                hasRedo = false;
    vector< unique_ptr<SerialCluster> > uclusters; // undo clusters
    vector< unique_ptr<SerialCluster> > rclusters; // redo clusters
};
    
struct UndoSnapshot {
    vector<float2>                   uselection;
    vector<float2>                   rselection;
    bool                             hasRedo = false;
    vector< unique_ptr<UndoRegion> > regions;
};

struct ZoneCacheStats {
    int segCacheHits       = 0;
    int segCacheMisses     = 0;
    int clusterCacheHits   = 0;
    int clusterCacheMisses = 0;
};

// pattern for circular queries
struct BlockPattern {

    float2    position;
    float     radius       = 0.f;
    Feature_t features     = 0; // matches blocks with any of features
    Feature_t antifeatures = 0;
    int       faction      = 0; // negative means ignore
    int       ident        = 0;
    trit      hasCommand;
    
    BlockPattern();

    // matches if any subblock may match
    bool matches(const BlockCluster* cl) const;
    bool matches(const Block* b) const;

    bool allowCached(const BlockPattern &o) const;
};

struct GameZone final {

    EffectsParticleSystem*       m_particles = NULL;
    std::set< watch_ptr<Block> > m_effectBlocks;
    std::mutex                   m_effectBlocksMutex;

    // state
    UIMode                     m_uimode = UIMode::VIEW;
    float2                     m_cursorPos;

    Block*                     m_debugCommand = NULL;
    float                      m_sfxVolume    = 1.f;
    float                      m_blockOverlap;

protected:
    void initCollisionHandlers();

    vector<Explosion*>         m_explosions;
    Selection*                 m_selection = NULL;

    vector< pair<const BlockCluster *, const Block*> > m_renderEffectBlocks;

public:

    cpSpace*                   m_space = NULL;
    float                      m_planeZ; // origin of the main game plane, 0 if in foreground
    uint                       simStep = 0;
    float                      simTime = 0.f;
    float                      renderSimTime = 0.f;
    float                      updateTime = 0.f; // real time when object positions were advanced
    int                        m_blockLimit = -1;
    SaveGame                   *save = NULL;
    float                      timeFactor = 1.f;
    bool                       debugPaused = false;
    bool                       debugSingleStep = false;

    bool                       m_spaceIsHash = false;
    float                      m_spaceHashDim = 0.f;
    int                        m_spaceHashSize = 0;

    mutable cpPolyShape        m_cachedShape;
    
    bool isUpdateStep(const void *ptr, float step) const;
    bool isUpdateStep(size_t hash, float step) const;
    bool isUpdateSubStep(const void *ptr, float step, float subStep) const;
    bool isUpdateSubStep(size_t hash, float step, float subStep) const;
    
    const vector<BlockCluster*>& getClusters() const
    {
        ASSERT_NOT_MAIN_THREAD();
        return m_clusters;
    }
    
    const vector<BlockCluster*>& getRenderClusters() const
    {
        ASSERT_MAIN_THREAD();
        return m_renderClusters;
    }
    
    const vector<ResourcePocket*>& getResources() const { return m_resources; }
    const vector<Projectile*>&     getProjectiles() const { return m_projectiles; }
    const Selection*               getSelection() const { return m_selection; }
    Selection      *               getSelection() { return m_selection; }
    bool                           isSelected(const Block* bl) const;
    bool                           isDragged(const Block* bl) const;

protected:
    vector<BlockCluster*>        m_clusters;
    vector<BlockCluster*>        m_renderClusters;
    std::mutex                   m_projectileMutex;
    vector<Projectile*>          m_projectiles;
    std::mutex                   m_resourceMutex;
    vector<ResourcePocket*>      m_resources;
    EZoneFeatures                m_zoneFeatures;
    StreamerBase*                m_streamer     = NULL;
    MetaZone*                    m_metaZone     = NULL;

    // derived/dynamic zone state
    typedef spatial_hash< watch_ptr<Block> > BlockHash;
    typedef spatial_hash< BlockCluster* >    ClusterHash;

    float getMaxQueryRad() const;

    AABBox                        m_bbox;
    ClusterHash                   m_clusterHash;
    struct ClusterQuery {
        BlockPattern pattern;
        int          simStep = -1;
        ClusterList  clusters;
    };
    mutable ClusterQuery          m_clusterQuery;
    mutable vector<Block*>        m_tempBlockList;

    mutable vector< pair<SegmentPattern, BlockCluster*> > m_cachedSegs;
    mutable int m_cachedSegsSimStep = -1;
    bool intersectCircleClustersPopulateList(const BlockPattern& pattern) const;

    struct VisibleProjectile {
        float2 pos;
        float  angle;
        uint   color;
        float  luma;
        float  size;
        float  len;
    };
    vector<VisibleProjectile>  m_visibleProjectiles;

    spatial_hash<Projectile*>     m_projectileHash; // fast access for projectiles
    spatial_hash<ResourcePocket*> m_resourceHash;
    bool                          m_locked = false; // in the middle of running the simulation - don't delete anything!

    int                               m_undoIndex = -1;
    std::mutex                        m_undoMutex;
    deque< unique_ptr<UndoSnapshot> > m_undoBuffer;

public:
    mutable std::recursive_mutex        m_clustersMutex;
    mutable std::mutex                  m_advanceMutex;
    std::mutex                          m_debugMutex;
    mutable ZoneCacheStats              m_cacheStats;
    FrameLogger                        *m_logger = NULL;
    
    vector<std::pair<float2, uint> > m_debugPoints;
    vector<std::pair< std::pair<float2, float2>, uint> > m_debugLines;

    size_t estimateUndoBufferBytes() const;
    void snapshotSelectionForUndo(bool newSnapshot=true);
    void snapshotForUndo(float2 center=float2(), float2 radius=float2());
    void appendToUndoSnapshot(float2 center=float2(), float2 radius=float2());
    bool doUndo();
    bool doRedo();
    int clearPersistentIdent(uint pid);
    int clearRect(float2 offset, float2 radius);
    int clearOutOfBounds();
    
    bool isLocked() const { return m_locked; }

    EZoneFeatures getZoneFeatures() const { return m_zoneFeatures; }
    void setZoneFeatures(EZoneFeatures features);

    int intersectCircleProjectiles(vector<Projectile*> *outv, float2 pos, float rad) const
    {
        ASSERT(m_locked);
        return m_projectileHash.intersectCircle(outv, pos, rad);
    }

    int countShapes() const;

    const MetaZone*      getMetaZone() const { return m_metaZone; }
    MetaZone*            getMetaZone()       { return m_metaZone; }
    void                 setMetaZone(MetaZone* mz) { m_metaZone = mz; }

    StreamerBase*        getStreamer()       { return m_streamer; }
    const StreamerBase*  getStreamer() const { return m_streamer; }
    bool                 saveLoaded();
    void                 setStreamer(StreamerBase* lb);
    int                  getSeedsLimitRemaining(float2 pos) const;
    int                  getCommandsLimitRemaining(float2 pos) const;

protected:

    // ui
    ClusterMesh                  m_clusterMesh;
    LumaMesh                     m_effects;
    PointMesh<VertexPos2ColorTime> m_resMesh;
    std::mutex                   m_renderableMutex;
    vector<IRenderable*>         m_renderable;
    unique_ptr<BackgroundRenderer> m_background;

public:

    GLRenderTexture              m_bloomRt; // for bloom
    GLRenderTexture              m_blurRt;  // for blurring bloom

    GLRenderTexture              m_aux0;
    GLRenderTexture              m_aux1;

    enum ZoneFeature : EZoneFeatures::value_type { ZERO=0, ZONE_FEATURES(SERIAL_TO_ENUM) };

    GameZone(EZoneFeatures features);
    
    void onQueueForDelete();
    ~GameZone();

    void Update();

    void registerEffect(IRenderable *ptr)
    {
        std::lock_guard<std::mutex> l(m_renderableMutex);
        vec_add(m_renderable, ptr);
    }
    void updateRenderPositions(bool active);
    void renderProjectilesPass0(const View& view, TriMesh<VertexPosColorLuma> &mesh);
    void renderProjectilesPass1(const View& view, TriMesh<VertexPosColorLuma> &mesh) const;
    void renderResources(const View& view);
    void DrawDebugHud(const View& view);
    void DrawGame(const View& view);

    void Clear(bool animate);
    void patchAIParents();

    void indexCommand(Block* bl);
    void insert(BlockCluster* cl);
    int erase(BlockCluster* cl, int index);
    
    void insert(Projectile* pr) 
    {
        ASSERT_UPDATE_THREAD();
        std::lock_guard<std::mutex> l(m_projectileMutex);
        m_projectiles.push_back(pr); 
    }

    void transferFrom(GameZone *zone);

    Block*        intersectMouseBlock();
    BlockCluster* intersectMouseCluster();

    // return closest block that would intersect BLOCK if placed at POSITION
    Block* intersectBlock(const Block* block, const BlockPosition& position,
                          const BlockCluster *checkCluster, 
                          const BlockCluster *ignoreCluster,
                          const Block *ignoreBlock) const;

    // return closest block that would intersect SPACER if placed at POSITION
    Block* intersectSpacer(const SpacerShape* spacer, const BlockPosition& position) const;

    // return cluster nearest to pos within radius, or NULL
    // LIMITATION: works based on shapes, so inside of asteroids not tracked if spacer disabled
    BlockCluster *intersectCircleClustersNearest(float2 pos, float radius) const;
    BlockCluster *intersectCircleClusterCirclesNearest(float2 pos, float radius) const;

    // return cluster closest to START intersecting a segment from START to END with width SHIPRAD
    // ignore clusters smaller than OBSRAD, and ignore IGNORECL
    BlockCluster *intersectSegmentClusters(const SegmentPattern &pattern) const;

    // return block closest to START intersecting a segment from START to END with width SHIPRAD
    // ignore blocks from clusters smaller than OBSRAD, and ignore IGNORECL
    // only consider blocks with FEATURES (if non-zero)
    struct IntersectSegmentResult {
        Block *block;
        float2 pos, normal;
    };
    IntersectSegmentResult intersectSegmentBlocks(const SegmentPattern &pattern) const;

    // return true if both points are valid positions (use streamer)
    bool intersectSegmentBounds(float2 start, float2 end) const;
    bool intersectPointBounds(float2 center) const;

    bool useClusterHash() const;
    void intersectCircleBlocks(BlockList* blocks, const BlockPattern& pattern) const;
    const BlockList &intersectCircleBlocks(const BlockPattern &pattern) const;
    Block* intersectCircleBlocksNearest(const BlockPattern& pattern) const;
    const ClusterList &intersectCircleClusters(const BlockPattern& pattern) const;
    const BlockList &intersectCircleShields(const BlockPattern &pattern) const;

    int intersectCircleResources(vector<ResourcePocket*> &resources, float2 pos, float rad);
    ResourcePocket *intersectCircleResourcesNearest(float2 pos, float rad);

    void makeExplosion(float2 pos, float2 vel, float radius, float damage, Block* origin, int faction);
    ResourcePocket* spawnResources(float2 pos, float quantity, bool fadein);

    void dirtyClusters();
    void SwitchToMode(UIMode mode);

    bool HandleEvent(const Event* event, bool mouseWasDown);
    bool HandleSharedEvent(const Event* event, GameState* caller);

    void maintainFleet(float2 offset, float radius, const Fleetspec &fs);
    void onPlayerColorChanged();
};

void magicCoverCluster(const BlockCluster* cl);

#endif

