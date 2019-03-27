
#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include "Blocks.h"
#include "GUI.h"

struct GlobalPlayerLock;
struct MetaZone;
struct ClusterWindow;
struct GalaxyRegion;

extern float kHudVisibleDist;
extern float kHudBorderSize;

#define COLOR_ALLY (ALPHA_OPAQUE|COLOR_PLAYER_HI)

struct MiniMap final : public ITabInterface {

    enum IconLabel {
        ICN_YOU = 0,
        ICN_ACTIVE_STATION,
        ICN_CONTESTED_STATION,
        ICN_STATION,
        ICN_WORMHOLE,
        ICN_TARGET,
        ICN_SLOW,
        ICN_GRAVITY,
        ICN_RAD,
        ICN_ALLY,               // here and below not in legend
        ICN_RES,
        ICN_INVALID,
    };

private:
    float                            m_targetScale  = 500.f; // radius of view in world coordinates
    bool                             m_hovered     = false;
    float2                           m_position;          // screen position of lower left of map
    float2                           m_size;              // width x height of entire space
    View                             m_view;              // view of just map part
    bool                             m_isLarge     = false;
    bool                             m_isEmbedded  = false;
    bool                             m_isOptions   = false;
    MetaZone *                       m_mz = NULL;
    float                            m_objsize     = 0.f;
    float                            m_legendAlpha = 0.f;
    watch_ptr<const MapObjective>    m_hoveredBl;
    int2                             m_hoveredCell  = int2(-1);
    const GalaxyRegion *             m_hoveredRegion = NULL; 
    GLRenderTexture                  m_tex;
    GLTexture                        m_mapColors;
    int                              m_mapColorsVersion;

    void renderWindow(float largef);
    MetaZone *getMetaZone() const;

    float clampViewScale(float scale) const;

public:

    void setPosition(float2 pos, float2 size);
    float2 getPosition() const { return m_view.position; }
    float2 getSize() const { return m_view.sizePoints; }

    void setIsOptions(MetaZone *mz) { m_isOptions = true; m_mz = mz; }

    MiniMap();
    ~MiniMap();

    bool HandleEvent(const Event* event) override;
    void render(const View &view);

    static IconLabel getMapObjectiveIconType(const MapObjective* pbl);

    static void renderMapObjectiveIcon(const MapObjective* pbl, DMesh &mesh, 
                                       float2 pos, float2 rad, float alpha);

    static const char* getIconLabel(IconLabel i);
    static uint  getIconColor(IconLabel i);
    static void renderIconType(IconLabel type, LineMesh<VertexPosColor>& line,
                               TriMesh<VertexPosColor> *tri,
                               float2 pos, float angle, float2 rad, float alpha, bool legend);

    AABBox getBox() const
    {
        AABBox box;
        box.insertRectCorners(m_position, m_position + getSize());
        return box;
    }

    void renderTab(const View &view) override;
};

struct TargetData {
    uint   color;
    string text;
};

struct AllyData {
    float health;
    int   size;
};

struct ControlPanel {

private:
    unique_ptr<ClusterWindow> m_playerWindow;
    unique_ptr<ClusterWindow> m_targetWindow;
    MiniMap                   m_minimap;

    std::mutex                m_mutex;
    TargetData                m_target;
    string                    m_fleetDesc;
    vector<AllyData>          m_allydata;

public:

    float playerWindowWidth = 0.f;

    // do stuff from update thread
    void update();

    bool HandleEvent(const Event* event)
    {
        return m_minimap.HandleEvent(event);
    }

    const MiniMap& getMap() const { return m_minimap; }

    void Draw(const ShaderState &ss, GameZone* zone, const Block* player, 
              GlobalPlayerLock &lock, float foreground);
};

#endif
