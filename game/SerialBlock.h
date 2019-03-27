#ifndef SERIAL_BLOCK_H
#define SERIAL_BLOCK_H

#include "Shapes.h"

typedef vector< watch_ptr<Block> > WatchBlockList;

// Serial representations are used for in memory blueprints
// They also represent the information that persists when saving the game

// WARNING bits are saved directly in player save files!
#define BLOCK_FEATURES(F)                       \
    F(COMMAND,        uint64(1)<<0)             \
    F(THRUSTER,       uint64(1)<<1)             \
    F(GENERATOR,      uint64(1)<<2)             \
    F(PERISHABLE,     uint64(1)<<3)             \
    F(TURRET,         uint64(1)<<4)              \
    F(LAUNCH,         uint64(1)<<5)             \
    F(CANNON,         uint64(1)<<6)             \
    F(LASER,          uint64(1)<<7)             \
    F(AUTOFIRE,       uint64(1)<<8)            \
    F(SHIELD,         uint64(1)<<9)             \
    F(TORQUER,        uint64(1)<<10)            \
    F(LAUNCHER,       uint64(1)<<11)            \
    F(EXPLODE,        uint64(1)<<12)            \
    F(ASSEMBLER,      uint64(1)<<13)            \
    F(REGROWER,       uint64(1)<<14)            \
    F(CANNON_BOOST,   uint64(1)<<15)            \
    F(INVULNERABLE,   uint64(1)<<16)            \
    F(NOREGEN,        uint64(1)<<17)            \
    F(PERSISTENT,     uint64(1)<<18)            \
    F(ENVIRONMENTAL,  uint64(1)<<19)            \
    F(TRACTOR,        uint64(1)<<20)            \
    F(ROOT,           uint64(1)<<21)            \
    F(GROW,           uint64(1)<<22)            \
    F(PHOTOSYNTH,     uint64(1)<<23)            \
    F(AUTOLAUNCH,     uint64(1)<<24)            \
    F(FREERES,        uint64(1)<<25)            \
    F(FACTORY,        uint64(1)<<26)            \
    F(SEED,           uint64(1)<<27)            \
    F(MELEE,          uint64(1)<<28)            \
    F(UNGROW,         uint64(1)<<29)            \
    F(UNIQUE,         uint64(1)<<30)            \
    F(CHARGING,       uint64(1)<<31)            \
    F(TRANSIENT,      uint64(1)<<38)            \
    F(SELFFACTORY,    uint64(1)<<39)            \
    F(NOCLIP,         uint64(1)<<40)            \
    F(INVISIBLE,      uint64(1)<<41)            \
    F(BUMPER,         uint64(1)<<42)            \
    F(TELEPORTER,     uint64(1)<<44)            \
    F(DEACTIVATES,    uint64(1)<<45)            \
    F(TELESPAWN,      uint64(1)<<46)            \
    F(NOCLIP_ALLY,    uint64(1)<<47)            \
    F(INTLINES,       uint64(1)<<48)            \
    F(ONEUSE,         uint64(1)<<49)            \
    F(NORECOLOR,      uint64(1)<<50)            \
    F(NOPALETTE,      uint64(1)<<51)            \
    F(LAUNCHER_BARRAGE, uint64(1)<<52)          \
    F(ALWAYSFIRE,     uint64(1)<<53)            \
    F(PALETTE,        uint64(1)<<54)            \
    F(NEVERFIRE,      uint64(1)<<55)            \
    F(STICKY,         uint64(1)<<56)            \
    F(ACTIVATE,       uint64(1)<<57)            \
    F(FIN,            uint64(1)<<58)            \


#define OBSOLETE_BLOCK_FEATURES(F)              \
    F(REPLICATE,      uint64(1)<<35)            \
    F(SENSOR,         uint64(1)<<47)            \
    F(SUBCOMMAND,     uint64(1)<<33)            \
    F(ENERGIZE,       uint64(1)<<32)            \
    F(LIGHTNING,      uint64(1)<<34)            \
    F(PROTOTYPE,      uint64(1)<<36)            \
    F(VISIBLURB,      uint64(1)<<37)            \
    F(JAMMER,         uint64(1)<<42)            \
    F(SPECTER,        uint64(1)<<43)            \

#define ALL_BLOCK_FEATURES(F) BLOCK_FEATURES(F) OBSOLETE_BLOCK_FEATURES(F)

#define FEATURE_UPDATE_MASK \
    (COMMAND|THRUSTER|TURRET|LAUNCH|AUTOLAUNCH|CANNON|LASER| \
     SHIELD|TORQUER|LAUNCHER|EXPLODE|ASSEMBLER|TRACTOR|       \
     PHOTOSYNTH|GROW|SEED|UNGROW|CHARGING|FIN)

#define ISOBLOCK_FEATURE_MASK (Block::ENVIRONMENTAL|Block::TRANSIENT|Block::MELEE|Block::ROOT| \
                               Block::INTLINES|Block::NORECOLOR|Block::NOPALETTE|Block::PALETTE| \
                               Block::NOCLIP|Block::NOCLIP_ALLY)
#define WEAPON_FEATURES (Block::CANNON|Block::LASER|Block::EXPLODE|Block::MELEE)

#define GROW_FEATURES (Block::MELEE|Block::ENVIRONMENTAL|Block::INVULNERABLE|\
                       Block::NOCLIP|Block::NOCLIP_ALLY|Block::INVISIBLE|Block::FREERES) \

#define ALL_FEATURES_MASK (~uint64(0))

DEFINE_ENUM(uint64, FeatureE, ALL_BLOCK_FEATURES);

#define EXPLOSIVE_FLAGS(F)                      \
    F(ENABLED, 1<<0)                             \
    F(PROXIMITY, 1<<1)                           \
    F(FINAL, 1<<2)                               \
    F(FRAG_PROXIMITY, 1<<3)                      \
    F(FRAG_FINAL, 1<<4)                          \
    F(FRAG_IMPACT, 1<<5)                         \
    F(FRAG_NOFLASH, 1<<6)                        \

DEFINE_ENUM(uchar, EExplosive, EXPLOSIVE_FLAGS);


#define SPREAD_PATTERN(F)                       \
    F(RANDOM,   1<<0)                              \
    F(SPIRAL,   1<<1)                               \
    F(WAVE,     1<<2)                               \
    F(CONSTANT, 1<<3)                              \
    F(ABSOLUTE, 1<<4)                            \

DEFINE_ENUM(uchar, ESpreadPattern, SPREAD_PATTERN);

// F(TYPE, NAME, DEFAULT)
#define SERIAL_CANNON_FIELDS(F)                                         \
    F(float,         roundsPerSec,   0) /* this many rounds per second, on average, regardless of burst size */ \
    F(uchar,         roundsPerBurst, 1) /* each burst shoots this many bullets, evenly spread out over the time slice */ \
    F(EExplosive,    explosive,      0)            \
    F(ESpreadPattern, pattern,       ESpreadPattern::RANDOM)            \
    F(float,         burstyness,     0) /* 0-1: percentage of time spent shooting (0 means evenly spaced shots, 1 means shotgun) */ \
    F(float,         muzzleVel,      0)                                \
    F(float,         spread,         0) /* max radians to perturbe the projectile trajectory by */ \
    F(float,         rangeStdDev,    0)                                 \
    F(float,         power,          0) /* power usage per shot */         \
    F(float,         damage,         0) /* damage per shell */             \
    F(float,         range,          0) /* distance shell travels */       \
    F(float,         explodeRadius,  0)                                   \
    F(float,         recoil,         1.f)                               \
    F(uint,          color,          0x00ffffff)                        \
    F(float,         projectileSize, 0)                                 \
                                                                        \
    F(copy_ptr<SerialCannon>, fragment,  copy_ptr<SerialCannon>())

#define SERIAL_CANNON_IGNORE_FIELDS(F)          \
    F(float,         barrelTaper, 1)              \

struct SerialCannon {
    DECLARE_SERIAL_STRUCT_CONTENTS(SerialCannon, SERIAL_CANNON_FIELDS, SERIAL_CANNON_IGNORE_FIELDS);
    float getDamage() const;
    float getRange() const;
};

#define SERIAL_BOOST_FIELDS(F)                                   \
    F(float2,   roundsPerSec,   float2(1, 0))                           \
    F(float2,   muzzleVel,      float2(1, 0))                           \
    F(float2,   power,          float2(1, 0))                           \
    F(float2,   damage,         float2(1, 0))                           \
    F(float2,   range,          float2(1, 0))                           \
    F(float2,   explodeRadius,  float2(1, 0))                           \
    F(float2,   spread,         float2(1, 0))                           \

DECLARE_SERIAL_STRUCT(SerialCannonBoost, SERIAL_BOOST_FIELDS);

// F(TYPE, NAME, DEFAULT)
#define SERIAL_LASER_FIELDS(F)                                          \
    F(float,       pulsesPerSec,   0)  /* 0 means continuous */             \
    F(uchar,       pulsesPerBurst, 1)                                       \
    F(EExplosive,  explosive, 0)                                      \
    F(float,       burstyness,     0)                                       \
    F(float,       pulseAvailability, 0) /* 0-1 what fraction of the time is it on*/ \
    F(float,       decay,         0.35f) /* seconds */                      \
    F(float,       power,          0)                                       \
    F(float,       width,           0)                                      \
    F(float,       damage,         0)                                       \
    F(uint,        color,          0x00ffffff)                              \
    F(float,       range,          0)                                       \
    F(float,       explodeRadius,  0)                                       \
    F(float,       immobilizeForce, 0)                                       \
    F(float,       linearForce, 0)                                          \


struct SerialLaser {
    DECLARE_SERIAL_STRUCT_CONTENTS(SerialLaser, SERIAL_LASER_FIELDS, SERIAL_CANNON_IGNORE_FIELDS);
};

#define SERIAL_SHIELD_FIELDS(F)                                \
    F(float,             strength,          100)               \
    F(float,             armor,             0.f)             \
    F(float,             regen,             20)                \
    F(float,             delay,             3)                 \
    F(float,             radius,            40)                \
    F(uint,              color,             0)                 \
    F(uint,              lineColor,         0)                 \
    F(uint,              damagedColor,      0)                 \
    F(float,             power,             1.f)               \

struct SerialShield {
    DECLARE_SERIAL_STRUCT_CONTENTS(SerialShield, SERIAL_SHIELD_FIELDS, SERIAL_PLACEHOLDER);
    float getRegenPower() const { return regen * power; }
};


#define COMMAND_FLAGS(F)                        \
    F(NONE,          uint64(1)<<0)              \
    F(METAMORPHOSIS, uint64(1)<<1)              \
    F(FOLLOWER,      uint64(1)<<2)              \
    F(ATTACK,        uint64(1)<<3)             \
    F(FLOCKING,      uint64(1)<<4)             \
    F(RECKLESS,      uint64(1)<<5)             \
    F(AGGRESSIVE,    uint64(1)<<6)             \
    F(CAUTIOUS,      uint64(1)<<7)             \
    F(SOCIAL,        uint64(1)<<8)             \
    F(PEACEFUL,      uint64(1)<<9)             \
    F(WANDER,        uint64(1)<<10)             \
    F(HATES_PLANTS,  uint64(1)<<11)             \
    F(FORGIVING,     uint64(1)<<12)             \
    F(TRACTOR_TRANSIENT, uint64(1)<<13)         \
    F(DODGES,        uint64(1)<<14)             \
    F(RIPPLE_FIRE,   uint64(1)<<15)             \
    F(SPREAD_FIRE,   uint64(1)<<16)             \
    F(BAD_AIM,       uint64(1)<<17)             \
    F(POINT_DEFENSE, uint64(1)<<18)             \
    F(INACTIVE,      uint64(1)<<19)             \
    F(SMART_FIRE,    uint64(1)<<20)             \
    F(NO_PARENT,     uint64(1)<<21)             \
    F(CHILDREN_SET,  uint64(1)<<22)             \
    F(BLUEPRINT_SET, uint64(1)<<23)             \
    F(HANGOUT,       uint64(1)<<24)             \
    F(FIRE_AT_WILL,  uint64(1)<<25)             \
    F(PACIFIST,      uint64(1)<<26)             \
    F(ALWAYS_KITE,   uint64(1)<<27)             \
    F(ALWAYS_RUSH,   uint64(1)<<28)             \
    F(ALWAYS_MANEUVER,uint64(1)<<29)             \
    F(MUTATES,       uint64(1)<<30)             \


#define OBSOLETE_COMMAND_FLAGS(F)           \
    F(ISPLAYER,      uint64(1)<<32)              \
    F(FARMER,        uint64(1)<<33)              \
    F(GARRISON,      uint64(1)<<34)              \
    F(CREATURE,      uint64(1)<<35)              \
    F(PIRATE,        uint64(1)<<36)              \
    F(TRANSPORT,     uint64(1)<<37)              \
    F(STATION,       uint64(1)<<38)              \
    F(PLANT,         uint64(1)<<39)             \
    F(HARVESTER,     uint64(1)<<40)             \
    F(CURIOUS,       uint64(1)<<41)             \
    F(GROW_ON_OK,    uint64(1)<<42)             \
    F(RAMMING_SPEED, uint64(1)<<43)             \
    F(FROMPLAYER,    uint64(1)<<44)             \
    F(DEFENDER,      uint64(1)<<45)             \


#define ALL_COMMAND_FLAGS(F)                    \
    COMMAND_FLAGS(F)                            \
    OBSOLETE_COMMAND_FLAGS(F)                   \


DEFINE_ENUM(uint64, ECommandFlags, ALL_COMMAND_FLAGS);

// set in blueprint, persistent across saves
#define AIBEHAVIOR_COMMAND_FLAGS (ECommandFlags::ALWAYS_KITE|    \
                                  ECommandFlags::ALWAYS_RUSH|    \
                                  ECommandFlags::ALWAYS_MANEUVER)

// set in ship, persistent for that ship
#define PERSISTENT_COMMAND_FLAGS (ECommandFlags::CHILDREN_SET|   \
                                  ECommandFlags::BLUEPRINT_SET|  \
                                  ECommandFlags::FOLLOWER|       \
                                  AIBEHAVIOR_COMMAND_FLAGS)

#define FOLLOWER_NOFLAGS (ECommandFlags::NO_PARENT| \
                          ECommandFlags::BAD_AIM|   \
                          ECommandFlags::SOCIAL)
                          

// persistent data for command blocks (ships really).
// F(TYPE, NAME, DEFAULT)
typedef copy_ptr<const BlockCluster> BlueprintPtr;
typedef vector<BlueprintPtr> CopyBlueprintList;

#define SERIAL_COMMAND_FIELDS(F)                                        \
    F(ECommandFlags,       flags,            ECommandFlags())           \
    F(uint,                ident,            0)     /* command GUID */  \
    F(uint,                parentIdent,      0)                         \
    F(Faction_t,           faction,          0)                         \
    F(float,               resources,        0)                         \
    F(float,               resourceCapacity, 0)                         \
    F(int,                 energy,           0)                         \
    F(float,               sensorRadius,     2200)                      \
    F(float,               disruptTime,      1.f)                       \
    F(schar,               currentChild,     -1)                        \
    F(float2,              destination,      float2(0.f))               \
    F(uint,                objective,        0)                         \
    F(BlueprintPtr,        blueprint,        BlueprintPtr())            \
    F(BlueprintPtr,        nextprint,        BlueprintPtr())            \
    F(CopyBlueprintList,   children,         CopyBlueprintList())       \
    

#define DEPRECATED_SERIAL_COMMAND_FIELDS(F)                          \
    F(int,                 level,            0)                      \
    F(int,                 experience,       0)                      \


struct DLLFACE SerialCommand {

    enum CommandFlag : ECommandFlags::value_type { COMMAND_FLAGS(SERIAL_TO_ENUM) };

    DECLARE_SERIAL_STRUCT_CONTENTS(SerialCommand, SERIAL_COMMAND_FIELDS,
                                   DEPRECATED_SERIAL_COMMAND_FIELDS);
    
    ECommandFlags getInitialFlags() const;
    void   setNonDef(const SerialCommand&sc);
    void   toBlueprint();
    bool   isBlueprint() const;
    string toString() const;
    string toPrettyString() const;
    void   finalize();
    bool   debugCheckConsistency(const BlockCluster *bp) const;
    const BlockCluster* setBlueprint(const BlockCluster *bp);
    void setNextprint(const BlockCluster *bp);

    size_t getSizeof() const;
};

// Almighty list of block attributes!
// F(TYPE, NAME, DEFAULT)
// mutable/dynamic fields
// !!WARNING!! don't forget to update the custom serializer in Save.cpp
#define SERIAL_BLOCK_BASE_FIELDS(F)                                          \
    F(BlockId_t,         ident,                   0)                    \
    F(float2,            offset,                  float2(0.f))          \
    F(float,             angle,                   0.f)                  \
    F(copy_ptr<SerialCommand>, command,           copy_ptr<SerialCommand>())                 \
    F(uint,              persistentIdent,         0)                    \
    F(uchar,             bindingId,               0)                    \


// pseudo-immutible block type fields that are OVERWRITTEN based on ident on de-blueprint-ization
#define SERIAL_BLOCK_MUTABLE_FIELDS(F)                                     \
    F(FeatureE,          features,                0)     /* bitmask */ \
    F(float,             lifetime,                -1) /* how many seconds before we self destruct? For missiles, etc */ \
    F(float,             growFrac,                1.f)                 \
    F(float,             health,                  100.f)                 \
    F(FeatureE,          growFeatures,            0)  /* adult features */ \
    F(FeatureE,          launchFeatures,          0) /* features we will have after we launch */ \
    F(Faction_t,         explodeFaction,          0)                \
    F(float,             launchLifetime,         -1.f)                 \
    F(float,             launchResources,         0)                 \
    F(float,             capacity,                0)  /* resource capacity */ \
    F(float,             growRate,                3.25f)  /* radius per seconds per to adulthood */ \
    F(uint,              fillColor,               0x2d0833) /* 0x00rrggbb */ \
    F(uint,              fillColor1,              0) /* 0x00rrggbb */ \
    F(uint,              lineColor,               0xffffff) \
    F(uint,              thrusterColor,           0xece1c6)          \
    F(uint,              thrusterColor1,          0)       \
    F(copy_ptr<SerialBlock>, replicateBlock,      copy_ptr<SerialBlock>()) \
    F(copy_ptr<SerialCannon>, cannon,             copy_ptr<SerialCannon>())   \
    F(copy_ptr<SerialLaser>, laser,               copy_ptr<SerialLaser>())              \
    F(copy_ptr<SerialShield>, shield,             copy_ptr<SerialShield>()) \

#define SERIAL_BLOCK_TYPE_FIELDS(F)            \
    F(Faction_t,         group,                   0)    \
    F(EShape,            shape,                   0)                    \
    F(uchar,             scale,                   1)                 \
    F(lstring,           name,                    lstring())                \
    F(int,               points,                  0)                    \
    F(float,             durability,              1.f)                  \
    F(float,             armor,                   0.f)                \
    F(float,             density,                 kComponentDensity)   \
    F(lstring,           blurb,                   lstring())               \
    F(float,             powerCapacity,           0)                 \
    F(float,             thrusterForce,           10000.f)           \
    F(float,             thrusterBoost,           2.f)               \
    F(float,             thrusterBoostTime,               0.2f)             \
    F(float,             torquerTorque,           10000)             \
    F(copy_ptr<SerialCannonBoost>, boost,         copy_ptr<SerialCannonBoost>()) \
    F(float,             replicateTime,           1)  /* seconds */     \
    F(float,             turretSpeed,             6) /* radians per second*/ \
    F(float,             turretLimit,             M_TAUf)                   \
    F(float,             generatorCapacityPerSec, 0)                 \
    F(float,             explodeDamage,           51)                \
    F(float,             explodeRadius,           30)                \
    F(float,             explodeStdDev,           0)                 \
    F(FeatureE,          deathFeatures,           0)                \
    F(float,             launchSpeed,             100)               \
    F(float,             launcherPower,           0)                 \
    F(float,             launcherSpeed,           0)             \
    F(float,             launcherOutSpeed,        0)                 \
    F(float,             launcherAngVel,          0)                 \
    F(float,             tractorRange,            400)                  \
    F(float,             photosynthPerSec,        1)                    \
    F(float,             seedLifetime,            60.f)             \
    F(float,             chargeMaxTime,           1.f)               \
    F(float,             chargeMin,               0.1f)              \
    F(float,             teleporterRadius,        0.f)               \
    F(float,             activatePower,           0.f)               \
    F(float,             meleeDamage,             5.f)              \
    F(int,               sort,                    0)                \
    F(float2,            barrelSize,              f2())             \
    F(int,               barrelCount,             0)                \
    F(float,             barrelTaper,             1.f)              \
    F(float,             elasticity,              0.4f)             \
    F(lstring,           sound,                   lstring())         \
    F(float,             aihint_range,            0.f)               \
    F(float,             aihint_muzzleVel,        0.f)               \
    F(float,             finForce,                0.f)               \

#define SERIAL_BLOCK_FIELDS(F)              \
    SERIAL_BLOCK_BASE_FIELDS(F)                      \
    SERIAL_BLOCK_MUTABLE_FIELDS(F)              \


// ignore these fields when reading in data files instead of crashing
#define DEPRECATED_SERIAL_BLOCK_FIELDS(F)                               \
    F(float,             maxHealth,               100.f)               \
    F(float,             growTime,                5)  /* seconds to adulthood */ \
    F(float,             energyCapacity,          0)                 \
    F(float,             alpha,                   1.f)               \
    F(float,             launchCapacity,          0)               \

    // F(float,             mass,                    0.f)           \

    // F(float,             teleporterPower,         4.f)               \


static const int kModRelocBase = 100000; // base of relocations
static const BlockId_t kMaxBlockId = 0xffff;

static const int kModBlockReloc0Max = 200;
static const int kModBlockReloc1Min = 17000;
static const int kModBlockReloc1Max = 26000;

inline bool isTempBlockId(BlockId_t ident)
{
    return ident == 0 || (kMaxBlockId <= ident && ident < kModRelocBase);
}

enum BindingGroups {
    BG_NONE = 0,
    BG_AUTOFIRE = 5
};

struct BlockType { 
    DECLARE_SERIAL_STRUCT_CONTENTS(BlockType, SERIAL_BLOCK_TYPE_FIELDS, SERIAL_PLACEHOLDER);

    int source = 0;

    void initFeatures(Feature_t ftrs);
};

// Serial block contains all the persistent information for each block
struct DLLFACE SerialBlock final {

    SERIAL_BLOCK_FIELDS(SERIAL_TO_STRUCT_FIELD);
    const BlockType *bt = NULL;
    
    SerialBlock();
    SerialBlock(const BlockType *bt);
    ~SerialBlock();

    DECLARE_SERIAL_STRUCT_OPS(SerialBlock);

    template <typename V>
    bool accept(V& vis)
    {
        return SERIAL_BLOCK_FIELDS(SERIAL_VISIT_FIELD_AND)
            DEPRECATED_SERIAL_BLOCK_FIELDS(SERIAL_SKIP_FIELD_AND)
            true;
    }

    BindingType getBindingType() const;

    SerialBlock &setType(const SerialBlock& sb);
    SerialBlock &setNonDef(const SerialBlock &sb);
    SerialBlock &updateType(const SerialBlock& sb);
    SerialBlock &replaceWith(const SerialBlock& sb);
    SerialBlock &setColors(const SerialBlock& sb);

    void setFeatures(Feature_t features_)
    {
        features = features_;
        initFeatures();
    }
    void setFeaturesMaterialize(Feature_t features_)
    {
        features = features_;
        initFeatures(0, NULL, true);
    }
    void setGrowLaunch(Feature_t growFtrs, Feature_t launchFtrs, float resources);

    void setFaction(Faction_t faction);

    void initFeatures(Feature_t features_=0, const SerialBlock *def=NULL, bool materialize=false);
    Feature_t getFutureFeatures() const;
    Feature_t getFeaturesFromId() const;

    // helper functions: return this
    SerialBlock* finalize(BlockIndex *idx=NULL);        // update block properties for consistency
    SerialBlock* overwriteFromId(BlockId_t ident=0); // replace all block properties with canonical versions
    SerialBlock* overwriteColorsFromId();
    SerialBlock* resetFeaturesFromId(); // just replace the features
    SerialBlock* updateById();      // update default block properties to canonical versions
    
    string toString() const;

    string toSimpleStats(int level=0) const;
    string toSimpleStatsHeader() const;
    string toSimpleStatsBody(int level=0) const;
    string toSimpleStatsFamily() const;
    
    static const SerialBlock* fromId(BlockId_t ident);

    BlockCluster *createCluster() const;

    // more helper fuctions, abstract details
    float weaponDamagePerSec() const;
    float weaponRange(Feature_t ftrs=0) const;
    float weaponVel() const;
    bool  weaponSpreads() const;
    float weaponEfficiency() const;
    bool  isFireableFixed() const;
    bool  isFireable() const;
    bool  isWeapon() const;
    bool  isHealer() const;
    bool  isTurret() const;
    int deadliness() const;
    int growResource() const { return max(1, deadliness()); }
    int getPlayerEnergy() const;
    bool isTransient() const;
    bool isTypeDefault() const;

    float getArea() const { return spec().area; }
    float getMaxHealth() const { return round(bt->durability * getArea()); }

    float getMass() const
    {
        float mass = bt->density * getArea();
        return (mass > 1.f) ? round(mass) : max(0.01f, mass);
    }
    
    float getGrowTime() const { return spec().sqrt_area / max(growRate, epsilon); }
    void setGrowTime(float time);
    float getRadius() const { return spec().radius; }
    const ShapeSpec& spec() const;
    const ShapeSpec* check_spec() const;
    const Block *example() const;

    size_t getSizeof() const;
};

enum WeaponFireType { WF_DEFAULT=0, WF_FIRE_ALL=1, WF_RIPPLE_FIRE=2 };

typedef std::array<uchar, 4> uchar4;

// data stored per UGC ship
#define SERIAL_SHIP_DATA(F)                                    \
    F(lstring, name, NULL)                                     \
    F(lstring, author, NULL)                                   \
    F(uint,    color0, 0)                                      \
    F(uint,    color1, 0)                                      \
    F(uint,    color2, 0)                                      \
    F(uchar4,  wgroup, uchar4())                               \


struct ShipData {
    DECLARE_SERIAL_STRUCT_CONTENTS(ShipData, SERIAL_SHIP_DATA, SERIAL_PLACEHOLDER);
    int3 getColors() const { return int3(color0, color1, color2); }
    void setColors(int3 v) { color0 = v.x; color1 = v.y; color2 = v.z; }
};

#endif // SERIAL_BLOCK_H
