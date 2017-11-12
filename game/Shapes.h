
#ifndef SHAPES_H
#define SHAPES_H

#include "Types.h"

static const float kComponentDensity = 0.1f;
static const float kAsteroidDensity  = 0.5f;

#define BLOCK_SHAPES(F)                         \
    F(SQUARE,   0)                              \
    F(OCTAGON,  1)                              \
    F(THRUSTER, 2)                              \
    F(CANNON,   3)                              \
    F(MISSILE,  4)                              \
    F(RECT,     5)                              \
    F(HEXAGON,  6)                              \
    F(TRI,      7)                              \
    F(COMMAND,  8)                              \
    F(SENSOR,   9)                              \
    F(WING,    10)                              \
    F(ADAPTER, 11)                              \
    F(CUSTOM,  12)                              \
    F(PENTAGON, 13)                             \
    F(RHOMBUS_72_108, 14)                       \
    F(RHOMBUS_36_144, 15)                       \
    F(THRUSTER_PENT,  16)                       \
    F(DISH_WEAPON,    17)                       \
    F(DISH_THRUSTER,  18)                       \
    F(RIGHT_TRI, 19)                            \
    F(RECT_LAUNCHER, 20)                        \
    F(RECT_CANNON, 21)                          \
    F(RECT_CANNON_BOOST, 22)                    \
    F(RECT_LONG, 24)                            \
    F(ISOTRI_72, 25)                            \
    F(ISOTRI_36, 26)                            \
    F(RIGHT_TRI2L, 27)                          \
    F(RIGHT_TRI2R, 28)                          \
    F(SEED_1, 29)                               \
    F(SEED_2, 30)                               \
    F(SEED_3, 31)                               \
    F(SEED_4, 32)                               \
    F(RECT_LONG_NARROW, 33)                     \
    F(RECT_LAUNCHER1, 34)                       \
    F(RIGHT_TRI_22_5L, 35)                      \
    F(RIGHT_TRI_22_5R, 36)                      \
    F(DISH_MISSILE,  37)                        \
    F(RECT_ROOT,     38)                        \
    F(GEM_1, 39)                               \
    F(GEM_2, 40)                               \
    F(GEM_3, 41)                               \
    F(GEM_4, 42)                               \
    F(ISOTRI_25, 43)                            \
    F(ISOTRI_25_MISSILE, 44)                    \
    F(ISOTRI_13, 45)                                    \
    F(ISOTRI_13_MISSILE, 46)                            \
    F(ISOTRI_6, 47)                                      \
    F(HEPTAGON_LAUNCHER, 48)                             \
    F(HEPTAGON, 49)                                      \
    F(GEM_2_LAUNCHER, 50)                                \
    F(GEM_3_LAUNCHER, 51)                               \
    F(GEM_4_LAUNCHER, 52)                               \
    F(RECT_QUARTER, 53)                                    \
    F(ISOTRI_3, 54)                                        \
    F(ISOTRI_25_WEAPON, 55)                               \
    F(NONAGON, 56)                                        \
    F(ISOTRI_80, 57)                                      \
    F(THRUSTER_RECT, 58)                                  \
    F(SQUARE_HALF, 59)                                       \
    F(SQUARE_LAUNCHER, 60)                                   \
    F(SQUARE_MISSILE, 61)                                    \
    F(RIGHT_TRI_30L, 62)                                   \
    F(RIGHT_TRI_30R, 63)                                   \
    F(OCTAGON_1, 64)                                      \
    F(SQUARE_1, 65)                                       \
    F(CANNON2, 66)                                        \
    F(RECT_V1, 67)                                        \
    F(MISSILE_LAUNCHER, 68)                               \
    F(MISSILE_SHORT, 69)                                  \
    F(COMMAND_MISSILE, 70)                                \
    F(COUNT, 71)                                          \
    F(RESERVED_LAST, 99)


#define PORT_TYPES(F)                                   \
    F(NORMAL, 1<<0)                                     \
    F(WEAPON_OUT, 1<<1)   /* away from ship */          \
    F(WEAPON_IN, 1<<2)   /* towards the ship */       \
    F(THRUSTER_OUT, 1<<3)   /* away from ship */    \
    F(THRUSTER_IN, 1<<4)   /* towards the ship */ \
    F(LAUNCHER, 1<<5)                                 \
    F(NONE, 1<<6)   /* can't connect to anything! */  \
    F(ROOT, 1<<7)                                   \
    F(MISSILE, 1<<8)                              \
    F(INVALID, 1<<9)                            \
    F(ANY, 0xffff)


DEFINE_ENUM(uint, EShape, BLOCK_SHAPES);
#define TO_SHAPE_ENUM(X, V) SHAPE_##X=V,
enum BlockShape : EShape::value_type { BLOCK_SHAPES(TO_SHAPE_ENUM) };

DEFINE_ENUM(ushort, EPort, PORT_TYPES);
#define TO_PORT_ENUM(X, V) PORT_##X=V,
enum PortType : EPort::value_type { PORT_TYPES(TO_PORT_ENUM) };

static const uint kShapeMaxVerts = 16;
static const uint kPlayerDefaultColor0 = 0x113077u;
static const uint kPlayerDefaultColor1 = 0xaaaaaau;

struct ShapeSpec {

    struct PortPos {
        float2   offset;        // offset of connection relative to center of block
        float2   normal;        // direction from center of block to center of connected block
        PortType type = PORT_INVALID;  // which types of port
        int      vidx[2];       // which line contains port (vertex indices)
        int      lidx = -1;      // which line contains port (line index)
    };

    EShape         shape             = SHAPE_SQUARE;
    uchar          scale             = 0;
    bool           launcher_radial   = false;
    bool           mirror_primary    = true;
    EShape         mirror            = SHAPE_SQUARE;
    EPort          portMask;
    uchar          portCount         = 0;
    uchar          convexVertexCount = 0;
    uchar          vertexCount       = 0;
    uint           sid               = 0; // shape id
    float          minradius         = 0.f;
    float          radius            = 0.f;
    AABBox         bbox;
    float          area              = 0.f;
    float          sqrt_area         = 0.f;
    PortPos*       ports             = NULL;
    const float2*  vertices          = NULL;
    const cpVect*  cpVerts           = NULL;
    const uint*    lineIndices       = NULL; // indices for GL_LINES of outline
    const uint*    triIndices        = NULL; // indices for GL_TRIANGLES of inside
    uchar          lineIndexCount    = 0;
    uchar          triIndexCount     = 0;
    const Block   *example           = NULL;

    // used for creation
    bool fillVerts(uint portCount, const vector<float2> &verts);
    bool finalizeVerts();
    void finalizePorts();
    void makeMirror(EShape shape, ShapeSpec &spec);

    // queries
    bool isRegular() const;
    bool isCircular() const;
    // used to attach missiles and launchables to launcher - missiles point in the same direction as directional launchers
    bool isDirectional() const { return !launcher_radial; }
    bool isScale() const;
    int sides() const;
    int outPortThruster() const; // -1 if no out port
    int inPortThruster() const; // -1 if no in port
    int outPortWeapon() const; // -1 if no out port
    int inPortWeapon() const; // -1 if no in port
};

bool isSimilar(EShape a, int size, EShape b, int bs);
BlockShape sidesShape(int sides);
uint getMirrorBlockIdent(uint ident);
uint getMinimizedIdent(uint ident);
uint getDefaultIdent(uint ident);
void minimizeBlockIds(vector<BlockId_t>& idents);
const SerialBlock *getScaleBlock(const SerialBlock &sb, int scalediff, const vector<BlockId_t> &limitIds);
uint getRefactionBlock(BlockId_t ident, Faction_t oldfaction, Faction_t newfaction);

struct OneMod;
void loadShapesFor(const OneMod *mod);

#endif
