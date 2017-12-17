#ifndef TABMENU_H
#define TABMENU_H

#include "ConstructSelector.h"
#include "Constructor.h"
#include "BindingInterface.h"
#include "Interface.h"

struct GSUpgrades final : public GameState, public ITabInterface {

private:
    UpgradeData                       m_plusPUp;
    Button                            m_plusP;
    vector<uint>                      m_ids;
    float2                            m_position;
    float2                            m_size;
    vector<UpgradeData>               m_invData;
    BlockPalette                      m_inventory;
    BlockPalette                      m_palette;

    OverlayMessage                    m_message;
    float                             m_tooltipTime = 0.f;

    int                               m_decrypting = -1;
    float                             m_decryptStart = 0.f;
    int                               m_decryptCount = 0;
    
    void onPChange();
    bool tryUpgrade(int ident);
    
public:

    GameState                        *m_current = this;

    GSUpgrades();
    bool HandleEvent(const Event* event);
    void render(const View& view);
    
    void onStep();
    void onSwapIn();
    void onSwapOut();

    void renderTab(const View &view)
    {
        m_position = view.center;
        m_size = view.size;
        render(view);
    }
};

struct MessagesTab final : public ITabInterface {

    TextInputBase m_messages;
    TipsWidget    m_tips;
    
    MessagesTab();

    void onSwapIn();
    
    virtual bool HandleEvent(const Event* event);
    virtual void renderTab(const View &view);
};

struct GSTabMenu final : public GameState {

    TabWindow m_tabs;

    enum Tabs { TAB_MAP=1, TAB_PROFILE=2, TAB_MESSAGES=4,
                TAB_UPGRADE=8, TAB_BINDING=16, TAB_FLEET=32 };
    MiniMap                 m_map;
    GSUpgrades              m_upgrade;
    BindingInterface        m_binding;
    ConstructSelector       m_selector;
    copy_ptr<ITabInterface> m_profile;
    MessagesTab             m_messages;
    bool                    m_pressedOutside = false;

    GSTabMenu(Tabs selected, uint mask=~0);
    ~GSTabMenu();
    void onPush();

    void playMusic(float foreground, float introAnim)
    {
        globals.sound->OnFadeMusic(introAnim, 1.f);
    }

    GameZone * step(bool first);
    void render(const View& view);
    bool HandleEvent(const Event* event);

    string toString() const;
};

inline bool pushTabMenu(GameState *caller, GSTabMenu::Tabs tab, uint mask=~0)
{
    if (caller->m_list->getCurrentState() != caller)
        return false;
    caller->m_list->pushAboveSelect(caller, new GSTabMenu(tab, mask), 0.5f);
    return true;
}

inline bool pushTabMenuBelow(GameState *caller, GSTabMenu::Tabs tab)
{
    caller->m_list->pushBelow(caller, new GSTabMenu(tab));
    return true;
}


#endif
