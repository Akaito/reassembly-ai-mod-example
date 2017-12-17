
#ifndef CLUSTER_WINDOW_H
#define CLUSTER_WINDOW_H

#include "Blocks.h"
#include "Graphics.h"

struct ClusterWindow {

    float scaleCoef = 1.f;
    uint  style;
    float3 hsv = float3(1.f, 1.f, 1.f);

private:

    GLRenderTexture tex;

    View          windowView;   // window at capture position

    AABBox              m_recordedBBox;
    float               m_recordedAngle = 0.f;
    const BlockCluster *m_recordedCluster = NULL;
    uint                m_recordedVersion = -1;

public:

    ClusterWindow(ButtonStyle st=S_CORNERS) : style(st)
    {
        tex.setFormat(GL_RGBA);
    }

    float2      getSize()       const { return windowView.sizePoints; }
    bool        empty()         const { return tex.empty(); }
    const View& getWindowView() const { return windowView; }
    View& getWindowView() { return windowView; }
    void        clear()               { tex.clear(); }

    const GLTexture &getTexture() const { return tex; }

    bool hasClusterVersion(const BlockCluster *cl) const 
    {
        return cl == m_recordedCluster && cl->version == m_recordedVersion;
    }

    void setupWindowView(const BlockCluster *cluster, float angle, float2 ws);
    
    // record new texture
    void Record(const BlockCluster *cluster, float angle, float2 ws);

    void DrawRecording(const ShaderState *ss, float2 position, float alpha) const;
    
    void RecordDraw(const BlockCluster *cluster, float angle, float2 ws,
                    const ShaderState *ss, float2 position, float alpha);
};

#endif // CLUSTER_WINDOW_H

