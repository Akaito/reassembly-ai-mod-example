
#ifndef GAME_H
#define GAME_H

#include "GUI.h"
#include "GameZone.h"
#include "FlyCam.h"

float2 getHUDMarkerPos(bool* isOffScreen, float2 shipScreenPos, const View& view);
float getDistScale(float2 screenPos, float2 screenSize);

struct Sector;
struct GSList;
struct GSEditor;
struct SaveData;

extern int kBlurMenuRadius;
extern string kSandboxScript;
extern bool kTestAlwaysSaveBlueprints;
extern bool kHeadlessMode;

struct GameState
{
    GameZone*      m_zone     = NULL;
    GSList         *m_list     = NULL;
    View           m_view;      // desired view for this state
    FlyCam         m_cam;
    float          m_height              = 1000.f; // distance from this plane to next game state plane
    bool           IsFinished            = false;
    bool           m_allowZoom           = true;
    bool           m_staticBackground    = false; // display a screenshot of lower states instead of drawing every frame
    bool           m_allowTransitionStep = false; // step even while transitioning
    bool           m_alwaysBlur          = false; // blur even when height == 0
    bool           m_isOverlay           = false; // overlay states sit on top of a fully functioning under state
    bool           m_gamepadToMouse      = true;
    float          m_renderheight        = 0.f;

    GameState() {}
    virtual ~GameState() {}
    virtual void initGUI() {}

    virtual GameZone*  step(bool first) { return NULL; } // once per frame, return zone to run
    virtual void   render(const View& view) {}
    virtual void   renderCursor(const View &view);
    virtual void   playMusic(float foreground, float introAnim) {}
    virtual void   updateView(bool sizeChanged) {}     // change m_view
    virtual View&  getView() { return m_view; }
    const View&    getView() const { return const_cast<GameState*>(this)->getView(); }
    virtual const View& getActiveView();
    virtual bool   onClusterUnload(BlockCluster *cl) { return true; }
    
    virtual bool   HandleEvent(const Event* event) { return false; }
    virtual void   onPush() { }
    virtual bool   isAnimating() const { return false; }

    virtual GameZone* getZone() const    { return m_zone; }
    void           fillBackground(float foreground) const;
    bool           HandleEditorStart(const Event* event);
    bool           isCurrentState() const;
    
    virtual bool isEditor() const { return false; }
    virtual void adjustZoom(float x) { m_cam.adjustZoom(x); }
    virtual string toString() const { return ""; }

    template <typename B>
    void renderWidget(B &but, const ShaderState &ss, float2 pos)
    {
        but.position = pos;
        but.alpha = m_view.alpha;
        but.render(ss);
    }

    template <typename B>
    void renderWidget(B &but, const ShaderState &ss)
    {
        but.alpha = m_view.alpha;
        but.render(ss);
    }

};

struct GSList final {

private:

    mutable std::mutex m_mutex;
    vector<GameState*>  m_actions;
    View               m_view;
    float              m_animIndex              = 0.f;
    float              m_animStep               = 0.f;
    int                m_index                  = 0;
    int                m_lastIndex              = -1;
    GameState*         m_lastStepped            = NULL;
    bool               m_blurLast               = false;
    double             m_transitionFinishedTime = 0.0;
    PostProc           m_postProc;
    GLRenderTexture    m_fbo;
    GameState*         m_staticBgState          = NULL;

    void push(GameState* self, GameState* s, uint idx);

    int getIndex(const GameState* gs) const
    {
        for (int i=0; i<m_actions.size(); i++) {
            if (m_actions[i] == gs)
                return i;
        }
        ASSERT_FAILED("", "GameState not in list");
        return -1;
    }

    void setAnimStep(float time);

public:

    GSList();

    void init();

    std::mutex& getMutex() { return m_mutex; }
    const View& getView()  { return m_view; }

    // Tim Lobes is the bestest.
    const View& getActiveView() const
    { 
        return m_index < m_actions.size() ? m_actions[m_index]->getActiveView() : m_view;
    }

    void pushAboveSelect(GameState* self, GameState *s, float animTime=0.75f);

    template <typename T, typename G>
    void pushAboveSelect(G *self, float animTime=1.f) { return pushAboveSelect(self, new T(self), animTime); }
    
    void pushBelow(GameState* self, GameState *s);
    void push2Below(GameState* self, GameState *s);
    void pushTop(GameState* self, GameState *s);
    void popToTop(GameState* self, float time=1.f);
    void popToBottom(GameState* self, float time=1.f);
    void popDown(GameState *s, float time=1.f);

    void popDownKeepView(GameState* s, float time=1.f)
    {
        popDown(s, time);
        m_actions[m_index]->m_cam.m_targetWorldRadius = s->m_cam.m_targetWorldRadius;
        m_actions[m_index]->m_view = s->m_view;
    }

    GSEditor* getEditor()
    {
        foreach (GameState *gs, m_actions)
        {
            if (gs->isEditor())
                return (GSEditor*) gs;
        }
        return NULL;
    }

    GameState* getCurrentState() { return vec_at(m_actions, m_index); }
    const GameState* getCurrentState() const { return vec_at(m_actions, m_index); }
    GameZone* getCurrentZone();

    GameZone* step();           // once per frame
    void render();              // once per frame
    std::mutex &fuckingRenderMutex() { return m_mutex; }

    bool  OnEvent(const Event* event);

    bool onClusterUnload(BlockCluster *cl);
        
    string toString() const;
    string topToString();
    bool isTransitioning() const;
    bool isAnimating() const;
};

GameState *CreateMainMenu();
ITabInterface *CreateProfileTab();
GameState* CreateControls(GameState *parent, int tab);
GameState *CreateGSAnisoptera();
void ImportFleetShip(GameState *self, SaveData *opbp, BlockCluster **opships);
GameState *CreateBlueprintEditor();

void doHeadlessSandbox();
bool isOLExiting();

vector<int> getUsedSaveSlots();
int getMaxUsedSaveSlot();

std::map<string, string> getLanguageNames();

#endif // GAME_H
