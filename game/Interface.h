

#ifndef INTERFACE_H
#define INTERFACE_H

#include "Game.h"
#include "Levels.h"
#include "Keys.h"

float splashAnim();
extern float2 s_splashViewCenterStart;
extern const int kSubHeaderSize;

struct BLogin : public Button {
    const bool canButton;
    float height = 0.f;
    BLogin();
    bool HandleEvent(const Event* event, GameState *self, GameState* mainmenu);
    void renderContents(const ShaderState &ss);
private:
    using ButtonBase::HandleEvent;
};


struct GSQuit final : public GameState {

    GSQuit()
    {
        m_allowTransitionStep = true;
    }
    
    void playMusic(float foreground, float introAnim)
    {
        globals.sound->OnFadeMusic(foreground, 0.f);
    }

    void renderCursor(const View &view) {}

    void onPush()
    {
        if (!nearZero(s_splashViewCenterStart))
            m_view.position = s_splashViewCenterStart;
    }

    virtual void render(const View &view)
    {
        fadeFullScreen(view, COLOR_BLACK);
        if (view.alpha >= 1.f) {
            OL_DoQuit();
        }
    }
};

struct GSSub : public GameState {

    GameState*   mainmenu = NULL;
    Button       back;
    float2       corner = float2(0.2f, 0.65f);

    GSSub(GameState *mm);
    void initGUI();

    ~GSSub()
    {
    }

    void updateView(bool sizeChanged)
    {
        if (mainmenu)
            mainmenu->updateView(sizeChanged);
    }

    GameZone* step(bool first) 
    {
        return mainmenu ? mainmenu->step(false) : NULL; 
    }
    
    bool HandleEvent(const Event* event);

    float2 getUpperLeftCorner() const
    {
        return corner * m_view.sizePoints;
    }

    float2 renderSubTitle(const ShaderState &ss, const char* text, float introAnim) const;

    void renderBackButton(const ShaderState &ss, float alpha)
    {
        renderWidget(back, ss, float2(0.1) * m_view.sizePoints);
    }
};


struct OptionEditor;

// sound effect volume
// music volume
// graphics performance options (particles, background, blur)
// key binding
struct GSOptions final : public GSSub, public INotifyHandler {

    vector<OptionEditor*> options;
    BContext<string>      language;

	GSOptions(GameState *mm);
    void initGUI();
    ~GSOptions();
    
    void OnNotify(const Notification& notif);

    void playMusic(float foreground, float introAnim)
    {
        // FIXME better to play game music when from options screen?
        globals.sound->OnFadeMusic(introAnim, 1.f);
        //globals.sound->OnPlayMusic(this, "OptionsMusic", foreground);
        if (mainmenu && m_zone)
            m_zone->m_sfxVolume = max(m_zone->m_sfxVolume, introAnim);
    }

    void onPush()
    {
        m_staticBackground = false; // live preview
    }

    void render(const View &view);
    bool HandleEvent(const Event* event);
};

float2 renderOptionList(const ShaderState &ss, const vector<OptionEditor*> &ops, float2 tp, const View &view);
void renderOptionTooltip(const ShaderState &ss, const vector<OptionEditor*> &ops, float2 tp, const View &view);

struct GSCredits final : public GSSub {

    GSCredits(GameState *mm) : GSSub(mm) { }

    void render(const View &view);

private:

    struct Credit { uint size; string title; string name; };
    
    AABBox         m_bbox;
    int            kstart = -1;
    int            mstart = -1;
    float          column = 0.f;
    vector<Credit> credits;

    float2 renderCred(const ShaderState& ss, const Credit& cred, float2 tp, uint tcolor, uint tcolor1);
    float2 renderScrollingCreds(const ShaderState &ss, float2 tp, int &start,
                                const char* title, int2 rc, const char** names, int count,
                                uint tcolor, uint tcolor1);
};

struct GSControls final : public GSSub {

    TabWindow                 tabs;
    AABBox                    bbox;
    
    GSControls(GameState *mm, KeyGroup tab);
    ~GSControls() ;
    GameZone *step(bool first);
    bool HandleEvent(const Event* event);
    void render(const View &view);
};

struct TipsWidget : public WidgetBase {

    std::mutex mutex;
    int        index = -1;

    TipsWidget();
    void render(const ShaderState &ss);
    bool HandleEvent(const Event *event);
};

struct CompactionProgress;

struct GSMainMenu final : public GameState
{
    enum ButtonName
    {
        B_START,
        B_TEST,
        B_MODS,
        B_SANDBOX,
        B_DEMO,
        B_CONTROLS,
        B_OPTIONS,
        B_CREDITS,
        B_EXIT,
    };

    int       selected = 0;
    Fleetspec spec0, spec1;
    float     m_foreground = 0.f;

    volatile bool resetting;
    
    vector<Button> buttons;
    BLogin         login;
    Button         bdlc;

    float2         logoWindowSize;
    float          logoLastSpawn = -1000.f;

    GSMainMenu(bool reset);
    void initGUI();
    void onPush();
    ~GSMainMenu();

    GameZone* step(bool first);

    void playMusic(float foreground, float introAnim);
    void updateView(bool sizeChanged);
    void render(const View &view);
    bool HandleEvent(const Event* event);

    void createBattle();
};

struct GSSplash final : public GameState {

    GameState *m_mm;

    GSSplash(GameState *mm);
    void onPush();

    GameZone* step(bool first);
    bool HandleEvent(const Event *event);

    // hide cursor
    virtual void renderCursor(const View &view) {}
    virtual void render(const View &view);
};


struct GSMessageBox final : public GSSub {

    MessageBoxWidget mb;
    
    GSMessageBox(const string& msg, GameState *mm) : GSSub(mm)
    {
        Reportf("Message Box: %s", msg.c_str());
        mb.message = str_word_wrap(msg, 80);
        mb.active = true;
    }

    GameZone* step(bool first) { return NULL; }
    
    virtual void render(const View &view)
    {
        ShaderState ss = view.getScreenShaderState();

        mb.alpha = view.alpha;
        mb.alpha2 = view.introAnim;
        mb.render(ss, view);
    }

    bool HandleEvent(const Event* event)
    {
        bool handled = mb.HandleEvent(event);
        if (!mb.active) {
            m_list->popDown(this, 0.25f);
        }
        return handled;
    }
};

template <typename T> T& to_ref(T* v) { return *v; }
template <typename T> T& to_ref(T &v) { return v; }

template <typename But>
static float2 renderButtonsDown(const ShaderState &ss, But* buttons, uint count, 
                                float2 pos, float xsize, float ypad, float introAnim, int selected=-1)
{
    if (abs(introAnim) < epsilon)
        return float2();
    
    ButtonLayout layout;
    layout.start(pos);
    layout.buttonCount.y = count;

    theDMesh().start();
    
    for (uint i=0; i<count; i++)
    {
        auto &but = to_ref(buttons[i]);
        if (!but.visible)
            continue;
        but.position = layout.pos;
        but.size.x   = xsize;
        but.alpha    = introAnim < 0.f ? -introAnim : layout.getButtonAlpha(introAnim);
        but.renderButton(theDMesh(), selected == but.index);
        if (selected == but.index)
            but.renderSelected(ss, but.defaultBGColor, but.hoveredLineColor, but.alpha);
        // but.render(ss, selected == but.index);
        layout.buttonFootprint.y = but.size.y + ypad;
        layout.row();
    }

    theDMesh().Draw(ss, ShaderColor::instance(), ShaderColor::instance());
    theDMesh().finish();

    for (uint i=0; i<count; i++)
    {
        to_ref(buttons[i]).renderContents(ss);
    }

    return float2(layout.buttonFootprint.x, pos.y - layout.pos.y);
}

struct GSMessages final : public GSSub {

    string                key;
    vector<string>        messages;
    float                 fontSize = -1.f;
    int                   current = 0;
    float2                size;
    std::map<int, float3> highlights;
    float3                hipos;

    void init();

    GSMessages(GameState *parent, const char* ky);

    virtual void render(const View &view);

    // pause game
    GameZone *step(bool first) { return NULL; }

    bool HandleEvent(const Event* event);
};


struct GSWormhole final : public GSSub {

    Button           upload;
    Button           regenerate;
    BLogin           login;
    MapObjective    *wormhole;
    std::mutex       mutex;
    string           error;
    string           note;
    vector<Button*>  buttons;
    bool             uploadAllowed = true;

    enum State { DIALOG, UPLOADING, REGENERATING, COMPLETE } state = DIALOG;

    GSWormhole(GameState *mm, MapObjective *mo);

    void onPush()
    {
        m_zone = NULL;
    }
    
    virtual void render(const View &view);
    bool HandleEvent(const Event* event);
    GameZone *step(bool first);
};

#endif // INTERFACE_H
