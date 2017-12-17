
#ifndef PLAYER_H
#define PLAYER_H

#include "AI.h"
#include "GUI.h"
#include "MapObjective.h"

#define PLAYER_PROGRESS(F)                      \
    F(USED_EDITOR, uint64(1)<<0)                \
    F(DIED, uint64(1)<<1)                       \
    F(MAKE_STATION, uint64(1)<<2)               \
    F(DESTROYED_TARGET, uint64(1)<<3)           \
    F(ACTIVATED_STATION, uint64(1)<<5)          \
    F(OPEN_WORMHOLE, uint64(1)<<6)              \
    F(COMMAND_MODE, uint64(1)<<7)               \
    F(RECRUIT_ALLY, uint64(1)<<8)               \
    F(BIGGER_SHIP, uint64(1)<<9)                \
    F(FACTORY_ON_SHIP, uint64(1)<<10)           \
    F(SHIP_IN_CHILD_SLOT, uint64(1)<<11)        \
    F(SPAWN_CHILD, uint64(1)<<12)               \
    F(KILL_A_SHIP, uint64(1)<<13)               \
    F(FIRE_CANNON, uint64(1)<<14)               \
    F(FIRE_MISSILE, uint64(1)<<15)              \
    F(PICKUP_RES_SMALL, uint64(1)<<16)          \
    F(PICKUP_RES_LARGE, uint64(1)<<17)          \
    F(NO_POWER, uint64(1)<<18)                  \
    F(GENERATOR_ON_SHIP, uint64(1)<<19)         \
    F(SPAWN_INTRO, uint64(1)<<20)               \
    F(FIRST_RESPAWN, uint64(1)<<21)             \
    F(HAS_FACTORY, uint64(1)<<22)               \
    F(TRANSFER_RU, uint64(1)<<23)               \
    F(USED_UPGRADES, uint64(1)<<24)             \
    F(USED_BINDINGS, uint64(1)<<25)             \
    F(USED_MAP, uint64(1)<<26)                  \
    F(EXPLORED, uint64(1)<<27)                  \
    F(BASIC_TUT, uint64(1)<<28)                 \
    F(UPGRADE_SOMETHING, uint64(1)<<29)         \
    F(RES_TUT, uint64(1)<<30)                   \
    F(CONQUERED_SECTOR, uint64(1)<<31)          \
    F(SPAWN_FACTORY, uint64(1)<<32)             \
    F(MULTIPLE_WEAPONS, uint64(1)<<33)          \
    F(LOTS_OF_LC, uint64(1)<<34)                \
    F(RELEASE_ALLY, uint64(1)<<35)              \
    F(INTRO_1, uint64(1)<<36)                   \
    F(INTRO_2, uint64(1)<<37)                   \
    F(UNLOCK_FACTORY, uint64(1)<<38)            \
    F(MAYBE_FACTORY, uint64(1)<<39)             \
    F(ENTER_WORMHOLE, uint64(1)<<40)            \
    F(WORMHOLE_UPLOAD, uint64(1)<<41)           \
    F(UNLOCKED_FACTION, uint64(1)<<42)          \
    F(EDIT_COMMAND_TARGET, uint64(1)<<43)       \
    F(LIBERATED_STATION, uint64(1)<<44)         \
    F(TRANSFER_RU_QUANT, uint64(1)<<45)         \
    F(RES_FULL, uint64(1)<<45)                  \
    F(RES_FULL_TUT, uint64(1)<<46)              \
    

DEFINE_ENUM(uint64, EProgress, PLAYER_PROGRESS);

#define AUTO_FLAGS(F)                                   \
    F(ZOOM,    1<<0)                                 \
    F(AIM,     1<<1)                                 \
    F(TARGET,  1<<2)                                 \

DEFINE_ENUM(uchar, EAutoFlags, AUTO_FLAGS);


#define FLEET_MODE(F)                          \
    F(ENGAGE_TARGET, 1)                        \
    F(FIRE_AT_WILL, 2)                         \
    F(DISENGAGE, 3)                            \
    
DEFINE_ENUM(uchar, EFleetMode, FLEET_MODE);


#define HUD_MODE(F)                             \
    F(ON, 1)                                    \
    F(MIN, 2)                                   \
    F(OFF, 3)                                   \

DEFINE_ENUM(uchar, EHUDMode, HUD_MODE);


#define CONTROL_SCHEME(F)                       \
    F(MOUSE_ROT, 1)                             \
    F(KEY_ROT, 2)                               \
    F(CARDINAL, 3)                              \

DEFINE_ENUM(uchar, EControlScheme, CONTROL_SCHEME);

const char* getFleetModeDescription(EFleetMode mode);

struct StationInterface;
struct FactionData;

struct TutorialData {
    struct Field {
        string       descr;
        EProgress gate;
        bool         complete = false;
        float        blinktime = 0.f;

        typedef int VisitIndexedEnabled;
        template <typename V> bool accept(V& vis) { return vis.VISIT(descr) && vis.VISIT(gate); }
    };

    // serialized fields
    string        m_header;
    int           m_minP  = 0;
    EProgress     m_gate  = 0;
    EProgress     m_after = 0;
    vector<Field> m_fields;

    template <typename V>
    bool accept(V& vis)
    {
        return vis.visit("header", m_header) &&
            vis.visit("minP", m_minP) &&
            vis.visit("gate", m_gate) &&
            vis.visit("after", m_after) &&
            vis.visit("fields", m_fields);
    }

    typedef int VisitIndexedEnabled;
};



extern int       kPointMax;
extern int       kPlayerFaction;
extern int       kStartingP;
extern const int kSaveCurrentVersion;

void setupDefaultWeaponBindings(BlockCluster *cl, bool force=false);
void processSteamCallbacks();
void setupSteamStats();

#define DEBUG_SPAWN(X) DPRINT(SPAWN, X)

#define ColorPlayer() (globals.save->getColor())
#define ColorPlayerHi() (globals.save->getColorHi())

struct ShipPosition {
    float2 pos;
    float2 bbox;
    float  radius;
    float  angle;
};

struct PlayerTarget {
    watch_ptr<Block> command;
    bool             isSet = false;
    bool             isLocked = false;
    float            time  = 0.f;

    // last position of target marker, for animation
    float2           pos;
    float            radius = 0.f;
    Faction_t        faction = 0;

    void setTarget(Block *bl, bool animate=false);
};

// runtime player info
struct PlayerData {
    PlayerTarget               target;

    watch_ptr<Block>           estimatedTarget;
    float                      lastThrustTime = -1000.f;
    float                      lastZoomTime   = -1000.f;

    bool                       isActivating = false;
    float                      viewAngle     = 0.f;
    float2                     direction     = float2(1.f, 0.f);
    float2                     desiredVelocity;
    float                      smoothThrustVol = 0.f;

    float                      transferFullTime = -1000.f;
    float                      transferPossibleTime = -1000.f;
    bool                       transferEnabled = false;

    int                        deferredExploredSectors = 0;

    watch_ptr<const MapObjective> waypoint;

    watch_ptr<Block>           command; // command module on the player's ship
    watch_ptr<Block>           nextCommand;
    
    // cached player cluster stats
    float                      resources = 0;
    float                      resourceCapacity = 0;
    float                      power = 0.f;
    float                      powerCapacity = 0.f;
    float                      pangle = 0.f;
    float                      bradius = 0.f;
    AABBox                     bbox;
    int                        deadliness = -1;
    Faction_t                  faction = 0;
    GameZone *                 pzone = NULL;
    float2                     velocity;
    float                      lastDamageTime = -1000.f;
    float                      lastWeaponUseTime = -1000.f;
    float                      disruptTime = 1.f; // disruption length
    float                      healthFrac = 1.f;
    bool                       isDead = false;

    float                      spacerStuckTimeout = 0.f;

    // weapon group info
    WeaponGroup                wgroups[5];
    float                      wspread = 0.f;

    int                        blueprintEnergy = 0.f;

    int                        wormholeResources = -1;
    BlockList                  wormholeFleet;

    StationInterface*          statint = NULL;
    float                      transferredR = 0.f;
};

struct Player : public PlayerData {

    Player();
    void reset();
    
    OverlayMessage            message;
    std::mutex                mutex;

    void toggleAutoFlag(uint flag, int set=-1);
    void cycleControlScheme(int scheme=-1);
    void cycleHUDMode();
    void toggleCommandMode(GameZone *zone);
    void cycleFleetMode();
    
    Block *respawnFromStation(const MapObjective* station, GameZone *zone); // create new player ship
    Block *spawnChild();

    void onContinue(GameZone* zone, float2 playerPos);

    void setPlayer(Block* cmd);
    void setTarget(Block* bl, bool animate=true) { target.setTarget(bl, animate); }
    const Block *getTarget() const { return target.command.get(); }
    void updateTarget(GameZone *zone);
    Block *findPlayerTarget(GameZone* zone, float radius) const;
    void onToggleTarget();

    void setMessage(const string &msg);
    void setErrorMessage(const string &msg);

    bool gamepadToMouse(const Event *evt);
    void gamepadToMouseStep();

    const BlockCluster* getBlueprint(); // return currently applied player blueprint
    void applyUpdateSaveData();
    
    int getFaction() const;

    void renderPointCursor(const View& view) const;
    void renderFlyCursor(const View& view) const;
    float2 renderResStats(const ShaderState &ss, float2 pos, float2 next, float tsize,
                          float3 hilite, int count, float alpha) const;


    void getPlayerFleetForMarkers(GameZone *zone, vector<ShipPosition> &vec) const;

    bool HandleFlyEvent(const Event* event, GameState* caller);
    bool HandleMenuEvent(const Event* event, GameState *caller);
    void doShipFlying();
    void doWeaponsTargetting(bool *hasFixed);

    void wormholeSave();
    void wormholeRestore();
};


struct GlobalPlayerLock {
    std::lock_guard<std::mutex>   player_lock;
    BlockCluster::unique_lock     cluster_lock;
    bool                          valid;
    GlobalPlayerLock();
};

struct ShipExperience {
    bool    seen      = false;
    int     killed    = 0;
    int     killed_by = 0;
 
    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(seen) &&
            vis.VISIT(killed) &&
            vis.VISIT(killed_by);
    }

    typedef int VisitEnabled;

    bool operator==(const ShipExperience &o) const
    {
        return seen == o.seen && killed == o.killed && killed_by == o.killed_by;
    }
};

struct ModData {
    vector<const SerialBlock*> blocks;

    typedef int VisitEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(blocks);
    }

    bool operator==(const ModData &o) const { return blocks == o.blocks; }
};

template <typename T> struct ParseValidate;
template <typename T> struct ParsePrepare;

template <> struct ParseValidate<ModData> { bool validate(const SaveParser &sp, ModData &ss); };
template <> struct ParsePrepare<ModData> { void prepare(const SaveParser &sp, ModData &ss); };


typedef pair<BlockCluster*, int> PrintCount;
typedef std::map<lstring, ShipExperience> EncounterMap;

// parsed via visitor
// for serialization see SaveData::toUploadString, SaveData::toStringBlueprints, SaveGame::writeToFile
#define SERIAL_SAVE_DATA(F)                                       \
    F(vector<BlockCluster*>,  blueprints)                         \
    F(copy_ptr<BlockCluster>, blueprint)                          \
    F(int,                    currentChild)                       \
    F(vector<BlockCluster*>,  children)                           \
    F(vector<PrintCount>,     fleetprints)                        \
    F(copy_ptr<BlockCluster>, playerprint)                        \
    F(Faction_t,              faction)                            \
    F(uint,                   color0)                             \
    F(uint,                   color1)                             \
    F(uint,                   color2)                             \
    F(lstring,                title)                              \
    F(lstring,                name)                               \
    F(uint,                   uniqueId)                           \
    F(vector<BlockId_t>,      knownBlockIds)                      \
    F(vector<BlockId_t>,      paletteIds)                         \
    F(int,                    points)                             \
    F(int,                    credits)                            \
    F(float2,                 position)                           \
    F(uint,                   playerIdent)                        \
    F(EProgress,              progress)                           \
    F(vector<MapObjective*>,  metamap)                            \
    F(int,                    version)                            \
    F(int,                    time)                               \
    F(ModData,                mods)                               \
    F(EHUDMode,               hudMode)                            \
    F(EAutoFlags,             autoFlags)                          \
    F(EFleetMode,             fleetMode)                          \
    F(EControlScheme,         controlScheme)                      \


#define SAVE_DATA_FIELDS(F)                     \
    F(bool, ownsShips, true)                    \
    F(string, loadPath, string())               \


#define SAVE_DATA_IGNORE(F)                                       \
    F(int, experience)                                            \
    F(int, energy)                                                \
    F(int, sequence)                                              \
    F(int, level)                                                 \
    F(vector<int>, missions)                                      \
    F(vector<int>, visited)                                       \
    F(vector<BlockCluster*>, userships)                           \
    F(EncounterMap, encounters)                                   \


struct SaveData {

    SERIAL_SAVE_DATA(SERIAL_TO_STRUCT_FIELD2);
    SAVE_DATA_FIELDS(SERIAL_TO_STRUCT_FIELD);
    
    // !!!!!!!! only used for parsing !!!!!!!!!
    template <typename V>
    bool accept(V& vis)
    {
        return SERIAL_SAVE_DATA(SERIAL_VISIT_FIELD_AND2)
            SAVE_DATA_IGNORE(SERIAL_SKIP_FIELD_AND2) true;
    }
    
    typedef int VisitEnabled;

    DECLARE_SERIAL_STRUCT_OPS(SaveData);
        
    SaveData();
    ~SaveData();
    void reset();
    bool isLoaded() const { return version != 0 && (blueprint || blueprints.size() || playerprint); }
    uint getColor() const;
    uint getColorHi() const;
    int3 getColors() const;
    void setColors(int3 v);
    
    BlockCluster* getAppliedBlueprint() const;
    BlockCluster* getConstructorBlueprint() const { return blueprint.get(); }
    void cleanupAllBlueprints() const;
    void cleanupBlueprint(BlockCluster *bp) const;

    enum SaveStatus {
        LOADING     = -4,
        FLEET_ONLY  = -3,
        OLD_VERSION = -2,
        PARSE_ERROR = -1,
        NOT_FOUND   = 0,
        SAVE_OK     = 1
    };

    enum ExportFlags {
        EXPORT_DEFAULT = 0,
        EXPORT_MODS = 1
    };
    
    SaveStatus loadSaveData(const char* path);

    bool isLocked(const BlockCluster *bp) const;
    BlueprintList getUniqueBlueprints() const;
    BlueprintList getAllBlueprints() const;
    string toUploadString() const;
    string toStringBlueprints(ExportFlags flags=EXPORT_DEFAULT) const;

    void clone() const;
};

// persistent player info
struct SaveGame : public SaveData, public INotifyHandler {

    int                          slot = -1;

    // not saved
    float2                       wrapSize = float2(FLT_MAX); // saved in map
    MetaZone                    *metazone = NULL;
    vector<Agent*>               agents;
    std::unordered_map<lstring, const BlockCluster*> agent_blueprints;
    bool                         error = false;
    mutable bool                 dirty = false;
    mutable std::recursive_mutex mutex; // between sector streamer writing save and update changing it
    mutable FactionData         *fdata = NULL;

    // update/streamer thread mutex
    mutable std::mutex           streamer_mutex;
    vector<MapObjective*>        metamap_new;
    vector<MapObjective*>        metamap_render;
    mutable BlueprintList        faction_blueprints;

    void foreach_pbl(const std::function<void(MapObjective *pbl)> &fun) const;

    void reset();

    ~SaveGame() { reset(); }
    
    MapObjective* intersectCircleMapObjectiveNearest(float2 pos, float rad, uint64 mask=~0);
    const MapObjective* findNearestStation(float2 pos, bool visited);
    MapObjective *addObjective(MapObjective *mo);
    void onMetazoneHeartbeat();

    void onLevelFill(GameZone *zone, const Level* level);
    bool onClusterUnload(BlockCluster* cl, const StreamerSpec &spec);
    void onClusterLoad(BlockCluster *cl);
    void onLoadClusters(GameZone *zone, const StreamerSpec &spec);
    void onZoneUpdate();
    void onZoneRender();
    
    void OnNotify(const Notification& notif);

    bool hasProgress(EProgress::Fields flag) { return isLoaded() ? (progress&flag) : true; }
    bool doProgress(EProgress::Fields flag);

    float2 getWrapSize() const { return wrapSize; }
    bool isLoaded() const { return slot >= 0 && SaveData::isLoaded(); }

    string getPath(const char* ext) const;
    bool writeToFile() const;
    bool writeBlueprintsToFile() const;

    SaveStatus loadSaveGame(int slot, MetaZone* mz);
    bool regenerateSaveGame();
    bool createInitial(int slot, const string &name, Faction_t faction);
    void recomputePlayerParams();
    void updateAgentShipIndex();

    const FactionData *getFactionData(Faction_t faction) const;
    const BlockCluster *getBlueprint(lstring name, bool allowCreate=false) const;
    BlueprintList getFactionBlueprints(Faction_t faction) const;
    bool isFactionUnlocked(Faction_t faction) const;

    const BlockCluster *advanceCurrentChild(int diff);
    
private:
    void setupKnownBlocks();
    void updateFactionBlueprints() const;
};

#endif // PLAYER_H

