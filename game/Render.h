
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
    virtual void renderCallback(const ShaderState &ss)=0;
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

#endif // RENDER_H
