
#ifndef BLOCKS_H
#define BLOCKS_H

#include "Render.h"
#include "SerialBlock.h"
#include "Spacer.h"
#include "Nav.h"

struct CannonBlock;
struct Laser;
struct Turret;
struct Autofire;
struct FiringData;
struct WeaponGroup;
struct SaveData;
DECLARE_ENUM(uchar, UIMode);

typedef vector<Port> PortList;
typedef spatial_hash<Block*> BlockHash;

static const float kComponentWidth = 10.f;

extern float kAssemblerSpeed;
extern float kPhysicsDamping;
extern int kCommandHaloDeadliness;

struct BlockMover final : public snMover {
    float force         = 0.f;
    float forceSqrt     = 0.f;
    float enabledSmooth = 0.f;
};

struct BlockShield final {
    float    regenTime   = 0.f;
    float    health      = 0.f;
    cpShape* shape       = NULL;
    float    sradius     = 0.f;
    bool     deployed    = false;
    float    damagedTime = -1000.f;

    ~BlockShield();
    BlockShield(Block *bl);
    void update(Block* bl);
    void updateShape(Block *bl);
};

#define CLUSTER_CLASS_FIELDS(F)                  \
    F(DEBRIS, 0)                                 \
    F(SPACESHIP, 1)                              \
    F(STATION, 2)                                \
    F(ROOTED, 3)                                 \
    
DEFINE_ENUM(uchar, ClusterClass, CLUSTER_CLASS_FIELDS);

struct LiveCluster final {
    bool                 momentumDirty = true;
    bool                 damaged = false;
    bool                 m_maybeVisible = false;
    bool                 m_isStatic     = false;
    bool                 m_enableShape  = true; // false if being mouse dragged or similar

    float                lastCollisionTime   = -1000.f;
    float                lastDamageTime      = -1000.f;
    float                lastMouseTime       = -1000.f;
    float                lastInitTime        = -1000.f;

    float                m_shieldBRadius = 0.f;
    float                m_power = 0.f;
    SpacerShape          m_spacer;
    BlockHash*           m_blockHash         = NULL;
    float2               momentum;

    LiveCluster(BlockCluster *cl);
    ~LiveCluster();

    size_t getSizeof() const
    {
        return sizeof(LiveCluster) + SIZEOF_IREC(m_spacer) + SIZEOF_REC(m_blockHash);
    }
};

struct BlockCluster final : public Body {

    friend struct SpacerShape;

    BlockList            blocks;
    Block*               command = NULL;     // a pointer to the command module, if any
    BlockCluster*        parent = NULL;
    LiveCluster*         live = NULL;
    bool                 dirty;       // blocks were attached or detached - recompute the center of gravity/mass
    uint                 version;     // incremented on changes
    AABBox               bbox;        // unrotated bounding rectangle
    lstring              name;        // ship name
    lstring              path;        // ship load path
    uint                 hash;        // blueprint hash: equal clusters have equal hashes
    void*                explicitOwner; // true if we should not delete as blueprint
    mutable uint         query = 0;
    ShipData             data;
    Faction_t            m_faction         = 0;
    Faction_t            m_factionActual   = 0;

    size_t                size()            const { return blocks.size(); }
    float                 getBRadius()      const { return m_brad; }
    float                 getMinRadius()    const { return m_minrad; }
    AABBox                getRotatedBBox(float angle, Feature_t mask=~0) const;
    Feature_t             getFeatureUnion() const { return m_featureUnion; }
    const SpacerShape    &getSpacer()       const { return live->m_spacer; }
    bool                  isSpacerEnabled() const { return live && live->m_spacer.isEnabled(); }
    rmutex&               getMutex()        const { return m_mutex; }

    typedef std::unique_lock<rmutex> unique_lock;
    typedef std::lock_guard<rmutex> lock_guard;
    typedef ::reverse_lock< unique_lock > reverse_lock;

    BlockCluster       *getBodyCluster()       { return parent ? parent : this; }
    const BlockCluster *getBodyCluster() const { return parent ? parent : this; }
    cpBody*             getBody()              { return &getBodyCluster()->body; }
    const cpBody*       getBody()        const { return &getBodyCluster()->body; }

    float  getResources()        const;
    float  getResourceCapacity() const { return m_resourceCapacity; }
    int    getGrowResource()     const { return m_deadliness; }       // resources used to grow this cluster
    int    getDeadliness()       const { return m_deadliness; }
    bool   hasCommand()          const;
    ClusterList&       getSubclusters()       { return m_subclusters; }
    const ClusterList& getSubclusters() const { return m_subclusters; }
    DLLFACE int isMobile() const;
    ClusterClass getClass() const;
    void overwriteFromId();

    void setColors(int3 colors) ;
    void applyColors();         // apply data.colors

    // return true if weapon with given faction collides with this cluster
    bool weaponCollide(Faction_t weaponFaction) const
    {
        return exist(this) &&
            (weaponFaction == 0 || m_faction == 0 ||
             ((weaponFaction > 0) ? m_faction != weaponFaction : m_faction == -weaponFaction));
    }

    void setResources(float v);

    void setPos(float2 p);
    void setAngle(float p);
    void setPosAngle(const BlockCluster& other);

    // if cluster is a tower/plant blueprint
    Block* getRoot() const;
    Block* getAssembler();
    const Block* getAssembler() const { return const_cast<BlockCluster*>(this)->getAssembler(); }
    Block* getRegrower();

    bool hasFreeRes() const;

    // called from collision handlers
    void   notifyPhysicsSolve(float collidingRadius);
    void   notifySpacerInvalidate() { live->m_spacer.invalidate(); }
    bool   needsNoSpacer(const Block* bl) const;
    int    updateSpacerVertices();
    uint   updateHash();
    bool   getEnableShape() const { return live && live->m_enableShape; }
    void   setEnableShape(bool has);
    
    static BlockCluster* fromShape(cpShape* shape);
    static bool lessByDeadliness(const BlockCluster *a, const BlockCluster *b) { return a->getDeadliness() < b->getDeadliness(); }
    static bool lessByHash(const BlockCluster *a, const BlockCluster *b) { return a->hash < b->hash; }
    static bool equalByHash(const BlockCluster *a, const BlockCluster *b) { return a->hash == b->hash; }
    
    bool   isMaybeVisible() const { return !live || live->m_maybeVisible; }
    void   setMaybeVisible(bool isVis) { if (live) live->m_maybeVisible = isVis; }
    EffectsParticleSystem* getParticles() const;
    bool   isBodyStatic() const { const BlockCluster *cl = getBodyCluster(); return cl->live && cl->live->m_isStatic; }
    BlockHash *getBlockHash();

    // NO - in zone
    // DIRTY - was instantiated for rendering
    // CLEAN - called setBlueprint(true)
    // PRISTINE - called SaveDate::cleanupBlueprint
    enum BPState : uchar { BP_NO=0, BP_DIRTY, BP_CLEAN, BP_PRISTINE };

private:
    int                   m_deadliness = 0;
    float                 m_brad = 0.f; // bounding radius
    float                 m_minrad = 0.f; // minimum radius
    vector<BlockCluster*> m_subclusters;
    Feature_t             m_featureUnion   = 0; // union of all attached block features
    float                 m_tractorBRadius = 0.f;
    float                 m_coreRadius = 0.f;
    float                 m_generatorCapacityPerSec = 0.f;
    float                 m_powerCapacity = 0.f;
    float                 m_resourceCapacity  = 0;
    float                 m_totalHealth = 0.f;
    BPState               m_bpstate = BP_NO;
    float                 m_mass              = 0.f; // cpmass may be infinite!
    mutable int           m_refcount = 1;
    mutable rmutex        m_mutex;

    // geometry for the whole cluster - regenerated when blocks are added or removed
    mutable bool          m_meshDirty = true;
    mutable ClusterMesh   m_mesh;

    void renderPortGeometry() const;
    void construct();
    bool spacerQueryValid(cpVect* cpVerts, uint vertCount);

public:

    void updateShapes();
    
    bool usePower(float v);
    float getPower() const { return live ? live->m_power : 0.f; }
    float getPowerCapacity() const { return m_powerCapacity; }
    float getPowerPerSec() const { return m_generatorCapacityPerSec; }

    BlockCluster(const BlockCluster& o);
    BlockCluster& operator=(const BlockCluster& o);
    bool operator==(const BlockCluster& o) const
    {
        // only equal if same object
        return &o == this;
    }

    BlockCluster();
    ~BlockCluster();
    void kill(int subidx);
    void killRecursive(int subidx);
    bool validateConnections(const Block *start=NULL, uint queryVal=0) const;

    void init();
    void makeLive() { if (!live) live = new LiveCluster(this); }
    void setAngleNoRotation(float sangle);
    void detachDisconnected(GameZone* zone, ClusterList *splitList);

    void repatchPorts();
    void patchPorts(PortList* freePorts);

    // called in sequence
    void updateBegin();
    void updateCommand();
    void updateBlocks();
    void updateEnd();

    void advanceUpdate();
    void advanceRender(float interpolant, std::unique_lock<std::recursive_mutex> &l);

    void render(const ShaderState &s, ClusterMesh* mesh) const;
    void renderInZone(const ShaderState &ss, const View& view) const;

    void getNavConfig(snConfig* c) const;

    // return a bounding circle outside of which no sensors can detect
    DLLFACE float getSensorRadius() const;

    // return a bounding circle inside of which at least one tractor can collect
    float getTractorBRadius() const { return m_tractorBRadius; }
    float getShieldBRadius()  const { return live ? max(live->m_shieldBRadius, m_brad) : m_brad; }
    float getCoreRadius() const { return m_coreRadius; }
    int   countTransients() const;
    float getHealthFraction() const;
    bool  isFullHealth() const;
    int attachTransient(Block *bl);

    friend bool exist(const BlockCluster *cl) { return cl && cl->live && cl->blocks.size() && cl->zone; }
    
    void dirtyMesh() { m_meshDirty = true; }

    Faction_t getFaction() const       { return m_faction; }
    Faction_t getFactionActual() const { return m_factionActual; }  // The faction assigned in the first call to init().  Effectively stores "real" faction so tournaments can find it.
    int getGroup() const;
    string summarize() const;
    lstring toPrettyShipName() const;
    string toString() const;
    lstring getName() const;     // cluster/blueprint name
    string getUniqueName() const;
    
    float getMass() const { return m_mass; }
    
    Block* getCenterBlock() const
    {
        if (command)
            return command;
        else if (blocks.size())
            return blocks[0];
        else
            return NULL;
    }
    
    void removeTentacles();
    int removeFromGameZone(int index);
    void removeFromGameZone() = delete;
    void addToGameZone(GameZone* zone, float2 pos);

    // add a free block to a cluster
    // assumes caller sets up position and rotation of block relative to cluster
    void attachBlock(Block* bl);
    bool attachBlockPatch(Block *bl);
    void repatchBlock(Block *bl);

    void removeBlock(Block* bl, int index);
    void killDeleteBlockSilently(int index);

    void attachSubcluster(BlockCluster *cl);
    void removeSubcluster(BlockCluster *cl, int idx);
    bool stitchSubcluster(BlockCluster *sub);
    void stichDangling(const ClusterList &dangling);
    void detachFromParent(int index);
    int absorbSubclusters();

    // like the above but use block connections to attach recursively
    void attachConnectedBlocks(Block* b, uint queryVal);

    // Remove all the blocks from a cluster. Blocks will not be in any cluster
    int removeAllBlocks();

    BlockCluster* clone() const;
    BlockCluster* instantiate() const;
    void onInstantiate(bool force=false);
    void makeVegetable(bool vegetal);
    void addSpawnRes();

    Block* getFirstBlock(Feature_t feature);

    size_t getSizeof() const;

    float2 getCenterPos() const { return parent ? getPos() : float2(); }
    float2 getAbsolutePos() const { return parent ? parent->getPos() + rotate(getCenterPos(), parent->getRot()) : getPos(); }
    float2 getAbsolutePos(float2 offset) const { return getAbsolutePos() + rotate(offset, getAbsoluteRot()); }
    float2 getOffset(float2 absPos) const { return rotate(absPos - getAbsolutePos(), -getAbsoluteAngle()); }
    float getAbsoluteAngle() const { return parent ? getAngle() + parent->getAngle() : getAngle(); }
    float2 getAbsoluteRot() const { return parent ? rotate(getRot(), parent->getRot()) : getRot(); }
    
    float2 getAbsoluteVel() const
    {
        if (parent)
            return parent->getVel() + rotate90(getPos()) * parent->getAngVel();
        else
            return getVel();
    }

    /////// Blueprint functions ////////////////////////////////////////////////////////
    // These operate on the in-zone cluster, with blueprint as argument
    // optional BlockCluster arguments default to blueprint from SerialCommand

    const BlockCluster* getCreateBlueprint();
    const BlockCluster* getBlueprint() const;

    BPState getBPState() const { return m_bpstate; }
    void setBPState(BPState state) { m_bpstate = state; }
    void setIsBlueprint(bool isBlueprint);    // remove non-blueprint data.
    bool isBlueprint() const { return m_bpstate != BP_NO; }
    
    float getBudResource() const;

    bool assembleToBlueprint(); // return true if assembly is complete
    void applyMutation(const SaveData *save=NULL);

    // return the child block, or NULL
    Block* budChild(const BlockCluster *bp);

    bool applyBlueprintBindings(const BlockCluster* bp=NULL);
    bool matchesBlueprint(const BlockCluster* bp=NULL) const;
    bool isBlueprintValid(const BlockCluster *bp=NULL) const;
    void repositionBlocks();

    bool teleportToDest(float2 pos, float angle);

    friend bool copy_explicit_owner(const BlockCluster* cl)
    {
        return cl->explicitOwner != NULL;
    }

    // refcounts are used with copy_ptr
    // refcounted clusters should never be in a zone!
    friend bool copy_refcount(const BlockCluster *cl, int delta)
    {
        std::lock_guard<rmutex> l(cl->m_mutex);
        ASSERT(!cl->zone);
        cl->m_refcount += delta;
        return cl->m_refcount > 0;
    }
    const BlockCluster *incref(int delta) const { m_refcount += delta; return this; }
    BlockCluster *incref(int delta) { m_refcount += delta; return this; }
    int getRefCount() const { return m_refcount; }
    bool isShared() const { return m_refcount > 1 || explicitOwner; }

    virtual void onQueueForDelete();
};

struct BlockPosition final {
    float2 clusterPos;
    float  clusterAngle = 0.f;
    float2 offset;
    float  angle = 0.f;

    BlockPosition() {}
    BlockPosition(const Block* b);
    BlockPosition(const BlockCluster *cl, float2 bo, float ba);
    BlockPosition(float2 cp, float ca, float2 bo, float ba) : 
        clusterPos(cp), clusterAngle(ca), offset(bo), angle(ba) {}
    BlockPosition(float2 cp, float ca) : 
        clusterPos(cp), clusterAngle(ca), offset(0), angle(0) {}

    float2 getAbsolutePos() const
    {
        return clusterPos + rotate(offset, clusterAngle);
    }

    float getAbsoluteAngle() const
    {
        return angle + clusterAngle;
    }
};


struct Port final {
    Block* block = NULL;
    int    index = -1;

    Port() NOEXCEPT {}
    Port(Block* b, uint i_) : block(b), index(i_) {}

    float2      getAbsolutePos() const;
    float2      getRenderPos() const;
    float2      getAbsolutePos(const BlockPosition& pos) const;
    float2      getOffset() const;
    float2      getAbsoluteNormal() const;

    Port&       connected();
    const Port& connected() const;
    bool        free() const;
    EPort       type() const;

    friend bool operator==(const Port& a, const Port& b) { return a.block == b.block && a.index == b.index; }
};

namespace std {
    template <>
    struct hash<Port> {
        std::size_t operator()(const Port& pt) const
        {
            return std::hash<Block*>()(pt.block) ^ (std::hash<int>()(pt.index) << 1);
        }
    };
}

struct Block final : public Watchable {

    friend bool exist(const Block *bl) { return bl && !bl->m_dead && exist(bl->cluster); }
    friend bool live(const Block *bl) { return exist(bl) && (bl->sb.health > 0); }
    friend bool kill_delete(Block *bl, int index);
    friend void kill_delete_silently(Block *bl, int index);
    
    bool isDead()     const { return m_dead; }
    bool isCommand()  const { return (sb.features&COMMAND); }
    bool hasCommand() const { return live(this) && getCommand(); }
    bool isIncomplete() const { return sb.features&GROW; }
    bool isRoot() const;
    bool isThruster() const;

    bool hasFreeRes() const;
    bool isInvulnerable() const;
    float getSensorRadius() const { return (sb.command ? sb.command->sensorRadius :
                                            cluster->getSensorRadius()); }
    bool canSkipShape() const;

    size_t getSizeof() const;

    // return true if weapon with given faction collides with this block
    bool weaponCollide(Faction_t weaponFaction) const
    {
        Faction_t faction = getFaction();
        return weaponFaction == 0 || faction == 0 ||
            ((weaponFaction > 0) ? faction != weaponFaction : faction == -weaponFaction);
    }
    
    void onClusterInit();
    float2 getClusterPos() const { return cluster->getAbsolutePos(); }
    float getClusterBRadius() const { return cluster->getBRadius(); }
    DLLFACE int getBlueprintDeadliness() const;

    bool isSensorVisible(const BlockCluster* cl) const;
    
    Block* getCommand() { return cluster->command; }
    const Block* getCommand() const { return cluster->command; }
    
    AI* getCommandAI()
    {
        Block *cmd = getCommand();
        return cmd ? cmd->commandAI.get() : NULL;
    }

    const AI* getCommandAI() const
    {
        const Block *cmd = getCommand();
        return cmd ? cmd->commandAI.get() : NULL;
    }
    
    float  getResources() const;              // return resources available to block
    float  addResource(float v, float2 pos); // return how much we added
    float  removeResource(float v);          // return how much we removed

    int getTractorPriority() const;

    const BlockCluster* getBlueprint() const;
    lstring toPrettyShipName() const;
    lstring getShipAuthor() const;

    // return amount removed
    float removeHealth(float v, Block* origin);
    float removeShieldHealth(float v, float2 pos);
    float applyHealing(float val, uint color);
    float getMaxHealth() const { return m_maxHealth; }
    float getMass() const { return round(spec->area * sb.density); }
    float getHealRate() const { return m_healRate; }
    float calcHealRate() const;
    float getZGrowRate() const;

    // return true if block is a weapon and has enough power to fire
    bool hasPowerToFire() const;

    // return true if firing weapon now will hit target, assuming linear motion
    bool isWeaponAimed(const FiringData &target) const;

    DLLFACE float2 weaponDirForTarget(float2 tpos, float2 tvel) const;
    float weaponAngleForTarget(float2 tpos, float2 tvel) const { return v2a(weaponDirForTarget(tpos, tvel)); }

    // actually enable weapon if it would hit target
    bool fireWeapon(FiringData &data);

    // resources used to grow block
    int getGrowResource() const
    {
        return ((sb.features&GROW) ? sb.growFrac : 1.f) * sb.growResource();
    }
    
    void makeVegetable(bool vegetal); // zap the ai
    bool isVegetable() const;
    Block* getAIParent() const;
    void setAIAgent(Agent *agent);
    const Agent* getAIAgent() const;
    AI*  getAI();
    void adoptAIChild(Block* child);

    static Block* fromShape(cpShape* shape);
    void createUpdateBlockShape(bool enabled, bool force);
    bool hasCircleShape() const { return shape && (m_flags&USE_CIRCLE_SHAPE); }

    // for use from the render thread
    struct lock_guard {
        BlockCluster::unique_lock  lock;
        const Block               *block;
        const BlockCluster        *cluster;
        lock_guard(const Block *bl) : block(bl), cluster(bl ? bl->cluster : NULL)
        {
            if (cluster) 
                lock = BlockCluster::unique_lock(cluster->getMutex());
        }

        bool valid() const { return lock.owns_lock() && cluster && block && cluster == block->cluster; }
    };
    
    enum Feature: Feature_t {
        ZERO = 0,
        BLOCK_FEATURES(SERIAL_TO_ENUM)
    };

    enum UpdateFlags {
        UF_NONE          = 0,
        UF_EFFECT_RENDER = 1<<0,
    };

    SerialBlock             sb;
    const ShapeSpec        *spec       = NULL;
    BlockCluster*           cluster    = NULL;
    MapObjective*           persistent = NULL;
    watch_ptr<const Block>  obstruction; // block preventing a plant from growing. also used for modular cannons
    float2                  momentum;

private:
    enum Flags { NEEDS_EFFECT_RENDER = 1<<1,
                 NEEDS_HEAL_STARVE        = 1<<2,
                 USE_CIRCLE_SHAPE         = 1<<3,
                 WEAPON_OBSTRUCTED        = 1<<4 };
    uchar              m_flags = 0;
    uchar              m_stepHash = 0;
    uchar              m_dead = 0;
    uint               m_lines  = 0;

    float              m_maxHealth = 0;
    float              m_healRate = 0.f;
    Feature_t          m_enabled = 0; // per-feature enable bit
    float3             m_shapeLast;

    void setFlag(uchar flag, bool val) { setBits(m_flags, flag, val); }

public:
    Port*             ports = NULL;
    cpShape*          shape = NULL;

    void setEnabled(Feature_t feature, bool val=true) { setBits(m_enabled, feature, val); }
    bool isEnabled(Feature_t feature) { return m_enabled&feature; }

    // query values
    static uint       getNextQuery(); // get next query value
    mutable uint      query = 0;        // query index where this block was last touched
    mutable uint      compareKey = 0;   // general purpose place to store comparison key

    static bool lessByKey(const Block* a, const Block* b) { return a->compareKey < b->compareKey; }

    // drawing / physics
    float2*                    tVertices = NULL; // rotated vertices

    // command
    copy_ptr<AI>      commandAI;

    // charge
    float             chargeTime = 0.f;

    // cannon
    copy_ptr<CannonBlock>  cannon;

    // laaaaser
    copy_ptr<Laser>   laser;

    // turret
    copy_ptr<Turret> turret;

    // autofire (point defense)
    watch_ptr<const Block> autofireTarget;

    // thruster / torquer
    copy_ptr<BlockMover> mover;

    //shield
    copy_ptr<BlockShield>  shield;

    bool teleportToDest(float2 pos, float angle);

    // assembler
    copy_ptr< vector<WatchPortPair> > assemblerConnections;

    // tractor
    copy_ptr< vector< watch_ptr<ResourcePocket> > > tractorResources;

    void construct();
    Block(const Block&) = delete;
    Block(SerialBlock &&sb);
    Block(const SerialBlock &sb_);
    ~Block();
    void reset(); // reset, e.g. to change shape

    string toPrettyString() const;
    string summarize() const;
    string toSimpleStats() const;

    bool update(uint index);    // return true if block was removed
    void onAttach();
    void initFeatures();
    void render(ClusterMesh *mesh) const;
    void renderToCache(ClusterMesh *mesh, const UIMode& uimode) const;
    void renderEffect(LumaMesh &ctx, const View& view) const;
    void renderConstruct(LineMesh<VertexPosColor> &vl) const;
    float getRenderSimTime() const;

private: 

    // called from Block::update
    const Block* autofireAcquire();
    void autofireFire();
    float chargingUpdate();    // return chargeBoost
    uint laserUpdate(float chargeBoost);
    ResourcePocket* tractorLocate() const;
    void tractorUpdateLocate();
    void tractorUpdateGuide();
    uint assemblerUpdateGuide();
    void assemblerClearConnections();
    void moverInit();
    void moverUpdate();
    bool launchUpdate(uint index); // return true if block detached
    void launchDetach(uint indx);
    PortPair seedFindConnection();
    bool seedGrowFrom();
    bool seedBigUpdate(uint cluster_index);
    void explodeParticles(int xp) const;

public:

    bool growUpdate(int cluster_index);
    bool launcherUpdate();      // return true if added block
    
    static MemoryPool &getMemoryPool();
    static void* operator new(std::size_t sz);
    void operator delete(void *ptr, std::size_t sz);

    float chargingGetBoost() const;
    int   getAssemblerConnections() const;
    void explodeOnExplode(Block *other);

    void setUngrow();

    float2 getRenderPos() const
    {
        return cluster->getRenderPos() + rotate(sb.offset, cluster->getRenderAngle());
    }

    BlockCluster *getBodyCluster() { return or_(cluster->parent, cluster); }
    const BlockCluster *getBodyCluster() const { return or_(cluster->parent, cluster); }

    // rotated with cluster (for use with shapes)
    float2 getBodyOffset() const
    {
        return cluster->parent ? (cluster->getPos() + rotate(sb.offset, cluster->getRot())) : sb.offset;
    }
    // in absolute space, relative to cluster (for use with forces, impulses)
    float2 getUnrotatedBodyOffset() const
    {
        return rotate(getBodyOffset(), getBodyCluster()->getRot());
    }

    float2 getAbsolutePos() const { return cluster->getAbsolutePos(sb.offset); }
    float2 getAbsoluteVel() const
    {
        const BlockCluster *body = getBodyCluster();
        return body->getVel() + rotate90(getUnrotatedBodyOffset()) * body->getAngVel();
    }

    float  getAbsoluteAngle() const { return sb.angle + cluster->getAbsoluteAngle(); }
    float2 getAbsoluteRot() const { return angleToVector(getAbsoluteAngle()); }
    float  getRenderAngle() const { return sb.angle + cluster->getRenderAngle(); }
    DLLFACE void   getNavConfig(snConfig* c) const;

    float2 getPortAbsolutePos(uint i) const
    {
        return getAbsolutePos() + rotate(spec->ports[i].offset, getAbsoluteAngle());
    }
    
    float2 getPortRenderPos(uint i) const
    {
        return getRenderPos() + rotate(spec->ports[i].offset, getRenderAngle());
    }

    float2 getPortAbsoluteNormal(uint i) const
    {
        return rotate(spec->ports[i].normal, getAbsoluteAngle());
    }

    // relative to cluster, unrotated (i.e. same as sb.offset)
    float2 getPortOffset(uint i) const 
    {
        return sb.offset + rotate(spec->ports[i].offset, sb.angle);
    }

    bool isPortFree(uint i) const
    {
        return ports[i].block == NULL && spec->ports[i].type != PORT_NONE;
    }

    Port getConnectedPort(Feature_t ftrs) const
    {
        for (uint i=0; i<spec->portCount; i++) {
            if (ports[i].block && (ports[i].block->sb.features&ftrs))
                return ports[i];
        }
        return Port();
    }

    Port getConnectedPort() const
    {
        for (uint i=0; i<spec->portCount; i++) {
            if (ports[i].block)
                return ports[i];
        }
        return Port();
    }

    bool anyPortsConnected() const
    {
        for (uint i=0; i<spec->portCount; i++) {
            if (ports[i].block)
                return true;
        }
        return false;
    }

    bool anyPortsFree() const
    {
        for (uint i=0; i<spec->portCount; i++) {
            if (isPortFree(i))
                return true;
        }
        return false;
    }

    bool allPortsConnected() const
    {
        for (uint i=0; i<spec->portCount; i++) {
            if (isPortFree(i))
                return false;
        }
        return true;
    }
    
    int countConnectedPorts(Feature_t features=0) const
    {
        int c=0;
        for (uint i=0; i<spec->portCount; i++) {
            if (ports[i].block && (!features || (features&ports[i].block->sb.features.get())))
                c++;
        }
        return c;
    }

    int countFreePorts() const
    {
        int c=0;
        for (uint i=0; i<spec->portCount; i++) {
            if (isPortFree(i))
                c++;
        }
        return c;
    }

    Faction_t getFaction() const;
    float2 getWeaponMuzzleSize() const;
    uint   getWeaponMuzzleCount() const;
    float  getTurretRadius() const;
    float  getTurretAngle() const;
    float  getTurretRenderAngle() const;
    float2 getWeaponAbsMuzzlePos() const;
    float2 getWeaponMuzzleOffset() const;
    float  getWeaponRange() const;
    float  getWeaponVel() const;
    float  getWeaponPeriod() const;
    DLLFACE void   setWeaponEnabled(bool enable); // TODO : UN-EXPORT THIS: Super-easy to cheat with it!

    float2 getLaunchVel() const;
    bool   isLaunchEnoughRes() const;

    // how much resources do we have available
    float getTractorResource() const;

    bool isDisrupted() const;
    bool isTractorActive() const;

    int disconnect();             // return number of connections broken
    bool kill(int index);         // remove from game (return true if killed, false if unkillable)
    void killSilently(int index); // remove from game, without making an explosion or anything
    bool doDeathExplode(Block *origin); // return true if dead
    float spawnDieResources();

    // Remove a block from it's cluster. Block will not be in any cluster.
    // return old cluster
    BlockCluster* removeFromCluster(int index);

    // disconnect + remove + add to new cluster, but don't touch other blocks in the old cluster
    // for use during cluster update
    // return true if we detached
    bool detachPartial(int index, float detachVel);

    // just remove all reference to the game zone
    void removeFromGameZone();

    int markConnected(uint queryVal, int* connectedSubclusters);

    Block* clone() const;
    BlockCluster* createCluster();

    static Feature_t getUpdateMask() { return FEATURE_UPDATE_MASK; }

    void doMirror();
    bool doScale(int scalediff, const vector<BlockId_t> &limitIds);

    void applyForce(float2 force) { getBodyCluster()->applyForce(force, getUnrotatedBodyOffset()); }
    void applyImpulse(float2 force) { getBodyCluster()->applyImpulse(force, getUnrotatedBodyOffset()); }
};


// return number of connections broken
int disconnectBlocks(const WatchBlockList &bl);

void cloneBlocks(const WatchBlockList& blocks, ClusterList* out);

PortPair launchConnection(Block* handle, Block *connect, float2 targetDir, float minalign, uint canConnectFlags);
vector<PortPair> launchConnections(Block* handle, Block *connect, float2 targetDir, float minalign, uint canConnectFlags);

int writeClusterImage(const BlockCluster *cl, const char* path);

template <typename T> bool exist(const watch_ptr<T> &ptr) { return exist(ptr.get()); }
template <typename T> bool live(const watch_ptr<T> &ptr) { return live(ptr.get()); }

#endif

