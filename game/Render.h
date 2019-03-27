
#ifndef RENDER_H
#define RENDER_H

#include "Graphics.h"
#include "Shaders.h"
#include "Colors.h"
#include "RGB.h"

typedef MeshPair<VertexPos2ColorTime, VertexPosColor> ClusterMesh;
typedef TriMesh<VertexPosColorLuma>  LumaMesh;

extern template struct MeshPair<VertexPos2ColorTime, VertexPosColor>;
extern template struct MeshPair<VertexPosColorLuma, VertexPosColor>;

struct IRenderable {
    virtual void render(const ShaderState &ss, const View &view)=0;
    virtual ~IRenderable() {}
};

struct RenderContext {
    
    RenderContext(const View& view_, DMesh* screenM=NULL, DMesh* worldM=NULL);

    View         view;
    ShaderState  screen;
    ShaderState  world;
    DMesh  *screenMesh = NULL;
    DMesh  *worldMesh = NULL;

    typedef DMesh::Scope Scope;
};

struct IBackground : public IDeletable
{
    virtual void renderUnder(const View &view)=0;
};

struct BackgroundRenderer {
    unique_ptr<IBackground> m_stars;
    unique_ptr<IBackground> m_worley;

    void render(const View &view, const GameZone *zone);
};

IBackground* CreateNebulaBackground();
IBackground* CreateWorleyBackground(const GameZone* zone);

bool reloadShaders();
void clearRenderCaches();

struct MapObjective;
void renderOrbit(DMesh &mp, uint color, const View &view, const BlockCluster *cl, const MapObjective &mo);

void renderGravityField(LineMesh<VertexPosColor> &line, float2 opos, float orad, int detail);
void renderSlowZone(LineMesh<VertexPosColor> &line, float2 opos, float orad, int detail);
void renderField(const MapObjective *pbl, const ShaderState &ss, const View &view);

#endif // RENDER_H
