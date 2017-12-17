
#ifndef MYTYPES_H
#define MYTYPES_H

// pre-declerations
struct AI;
struct AIAction;
struct Agent;
struct Block;
struct Block;
struct BlockCluster;
struct BlockIndex;
struct BlockPattern;
struct BlockPosition;
struct ClusterWindow;
struct ControlPanel;
struct EffectsParticleSystem;
struct Explosion;
struct Fleetspec;
struct FrameLogger;
struct GameState;
struct GSList;
struct GameZone;
struct IBackground;
struct ISoundMgr;
struct Level;
struct MetaZone;
struct ObstacleQuery;
struct Player;
struct Port;
struct PortPair;
struct Projectile;
struct ResourcePocket;
struct SaveGame;
struct SaveParser;
struct SaveSerializer;
struct SectorLimits;
struct SegmentPattern;
struct Selection;
struct SerialBlock;
struct SerialCluster;
struct SerialCommand;
struct ShaderState;
struct ShipLoader;
struct SpacerShape;
struct StreamerBase;
struct StreamerSpec;
struct View;
struct WatchPortPair;
struct sNav;
typedef vector<Block*> BlockList;
typedef vector<BlockCluster*> ClusterList;
typedef vector<const BlockCluster*> BlueprintList;

// types
typedef uint64 Feature_t;
typedef int    Faction_t; // negative faction used in patterns, etc.
typedef uint   AI_t;      // AI mod id
typedef uint   BlockId_t;


enum LoadStatus {
    LS_ERROR = -1,
    LS_MISSING = 0,
    LS_OK = 1,
};

// struct fixed_invert { constexpr float operator()(int S) { return 1.f / S; } };
// struct fixed_identity { constexpr float operator()(int S) { return S; } };

#if _MSC_VER <= 1800
#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif

inline CONSTEXPR float fixed_invert(int S) { return 1.f / S; }
inline CONSTEXPR float fixed_identity(int S) { return S; }

typedef float (*fixed_scalar_t)(int);

template <typename T, int S, fixed_scalar_t I= fixed_invert>
struct FixedPt {
    T val;

    template <typename F>
    static T round_clamp(F f)
    {
        DASSERT(lowest() <= f && f <= max());
        DASSERT(f == 0 || abs(f) >= epsilon());
        return clamp(round_int(f / I(S)),
                     std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max());
    }

    static CONSTEXPR float max() { return std::numeric_limits<T>::max() * I(S); }
    static CONSTEXPR float lowest() { return std::numeric_limits<T>::lowest() * I(S); }
    static CONSTEXPR float epsilon() { return I(S); }

    FixedPt() {}
    FixedPt(int f) : val(round_clamp(f)) {}
    FixedPt(float f) : val(round_clamp(f)) {}
    FixedPt(double f) : val(round_clamp(f)) {}
    FixedPt(FixedPt &x) : val(x.val) {}

    FixedPt &operator=(float f) { val = round_clamp(f); return *this; }
    operator float() const { return val * I(S); }

    FixedPt &operator+=(float f) { *this = *this + f; return *this; }
    FixedPt &operator/=(float f) { *this = *this / f; return *this; }
    FixedPt &operator*=(float f) { *this = *this * f; return *this; }
    
    friend float max(float a, FixedPt b) { return std::max(a, (float) b); }
    friend float max(FixedPt a, float b) { return std::max((float)a, b); }
    friend float min(float a, FixedPt b) { return std::min(a, (float) b); }
    friend float min(FixedPt a, float b) { return std::min((float)a, b); }
    friend float abs(FixedPt a) { return std::abs(a.val) * I(S); }
};

typedef signed char schar;

#if USE_FIXED_POINT

typedef FixedPt<schar, 10> fixed8_10;
typedef FixedPt<uchar, 10> fixed8_u10;
typedef FixedPt<uchar, 20> fixed8_u20;
typedef FixedPt<uchar, 4> fixed8_u4;
typedef FixedPt<schar, 4> fixed8_4;
typedef FixedPt<uchar, 2> fixed8_u2;
typedef FixedPt<schar, 2> fixed8_2;
typedef FixedPt<schar, 1, fixed_identity> fixed8_1;
typedef FixedPt<uchar, 1, fixed_identity> fixed8_u1;
typedef FixedPt<uchar, 255> fixed8_unorm;
typedef FixedPt<schar, 127> fixed8_snorm;

typedef FixedPt<ushort, 200> fixed16_u200;
typedef FixedPt<ushort, 100>  fixed16_u100;
typedef FixedPt<ushort, 10>  fixed16_u10;
typedef FixedPt<short, 10>  fixed16_10;
typedef FixedPt<ushort, 4>  fixed16_u4;
typedef FixedPt<short, 4>  fixed16_4;
typedef FixedPt<short, 2>  fixed16_2;
typedef FixedPt<ushort, 1, fixed_identity>  fixed16_u1;
typedef FixedPt<short, 1, fixed_identity>  fixed16_1;
typedef FixedPt<ushort, 65535> fixed16_unorm;
typedef FixedPt<short, 32767> fixed16_norm;
typedef FixedPt<short, 8192> fixed16_pi;

typedef FixedPt<ushort, 100, fixed_identity> scaled16_u100;
typedef FixedPt<short, 100, fixed_identity> scaled16_100;
typedef FixedPt<ushort, 10, fixed_identity> scaled16_u10;
typedef FixedPt<uchar, 50, fixed_identity> scaled8_u50;
typedef FixedPt<uchar, 10, fixed_identity> scaled8_u10;
typedef FixedPt<uchar, 50, fixed_identity> scaled8_u50;
typedef FixedPt<uchar, 5, fixed_identity> scaled8_u5;

#else

typedef float fixed8_10;
typedef float fixed8_u10;
typedef float fixed8_u20;
typedef float fixed8_u4;
typedef float fixed8_4;
typedef float fixed8_u2;
typedef float fixed8_2;
typedef float fixed8_1;
typedef float fixed8_u1;
typedef float fixed8_unorm;
typedef float fixed8_snorm;

typedef float fixed16_u200;
typedef float  fixed16_u100;
typedef float  fixed16_u10;
typedef float  fixed16_10;
typedef float  fixed16_u4;
typedef float  fixed16_4;
typedef float  fixed16_2;
typedef float  fixed16_u1;
typedef float  fixed16_1;
typedef float fixed16_unorm;
typedef float fixed16_norm;
typedef float fixed16_pi;

typedef float scaled16_u100;
typedef float scaled16_100;
typedef float scaled16_u10;
typedef float scaled8_u50;
typedef float scaled8_u10;
typedef float scaled8_u50;
typedef float scaled8_u5;

#endif


#endif // MYTYPES_H
