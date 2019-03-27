
#ifndef STATION_INTERFACE_H
#define STATION_INTERFACE_H

#include "Render.h"
#include "GUI.h"

struct Tutorial;
struct ShipPosition;
struct CollectAnim;

typedef vector<Tutorial*> TutorialVec;

struct StationInterface final : public INotifyHandler, public IRenderable {

private:
    watch_ptr<Block>              m_station;     // currently interfaced station
    GameZone*                     m_zone = NULL;
    GameState*                    m_gs = NULL;
    const ControlPanel           *m_panel = NULL;

    MapObjective                 *m_pbl   = NULL;
    float                         m_activateStartTime = -1.f;
    bool                          m_alreadyActivated = false;
    int                           m_lastSaveSimStep = -1;
    
    TextInputBase                 m_messages;
    float                         m_messagesLastTime = -1000.f;

    mutable std::mutex            m_tutorialMutex;
    bool                          m_tutorialsDirty = false;
    TutorialVec                   m_tutorials; // render thread only!

    enum UnlockType {
        UT_NONE = 0,
        UT_ACTIVATE,
        UT_CAPTURE,
        UT_WORMHOLE,
    };

    std::mutex                   m_mutex;
    vector<CollectAnim>          m_collects;
    vector<const MapObjective *> m_drawObjectives;
    vector<ShipPosition>         m_followers;
    int                          m_unlockVisible = 0;
    float3                       m_resHiLite;
    TriMesh<VertexPos2ColorTex>  m_wormholeMesh;

    struct DeferredCollect {
        int    type  = 0;
        float  value = 0.f;
        float2 pos;
    };
    DeferredCollect               m_deferredCollects[4];
    
    void OnNotify(const Notification& notif);

    void flushDeferredCollect(DeferredCollect &dc);
    void addDeferredCollect(DeferredCollect &dc, float2 wpos, float val);
    void addCollect(const CollectAnim& anim);

public:
    
    StationInterface();
    ~StationInterface();

    const Block* getStation() const { return m_station.get(); }
    vector<string> getCompleteTutorials() const;

    void setup(GameState *gs, ControlPanel *panel);
    void update(bool first);
    bool HandleEvent(const Event* event);

    // in world stuff
    void renderOverlay(RenderContext& ctx);

    // not in world
    void renderHUD(RenderContext& ctx);

    // effects
    void render(const ShaderState &ss, const View &view);
};



#endif // STATION_INTERFACE_H
