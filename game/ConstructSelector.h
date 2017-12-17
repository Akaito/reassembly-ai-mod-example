
#ifndef CONSTRUCT_SELECTOR_H
#define CONSTRUCT_SELECTOR_H

#include "Save.h"
#include "Game.h"
#include "ControlPanel.h"
#include "ClusterWindow.h"
#include "StationInterface.h"

struct BShip final : public Button {

    ClusterWindow   window;
    BlockCluster   *cluster    = NULL;
    const bool      printIndex = false;
    bool            locked     = false;
    bool            useRecording = false;
    bool            copyHack = false;
    int             count = 1;
    ButtonText      bt;

    BShip(BlockCluster *cl, int idx=0, bool pri=false);

    void updateLocked();
    void renderContents(const ShaderState &ss);
    void renderDragged(const ShaderState &ss, float2 pos) const;
};



struct ConstructSelector final : public GameState, public ITabInterface {

    GameState*      m_gs        = NULL;

private:

    std::mutex      m_mutex;
    Scrollbar       m_scrollbar;
    vector<BShip*>  m_buttons;
    float2          m_position; // center
    float2          m_size;
    BShip*          m_dragged   = NULL;
    BShip*          m_cloned    = NULL;
    int             m_deleting  = -1;
    bool            m_confirmUp = false;
    bool            m_isTab     = false;
    SerialCommand * m_command   = NULL;
    vector<uint>    m_initialChildHashes;
    bool            m_bps_dirty = false;

    enum ESort { SPOINTS_ASC, SPOINTS_DEC, SNAME, SMASS_ASC, SMASS_DEC };
    BContext<ESort> m_sort;
public:

    ConstructSelector();
    ~ConstructSelector();

    void render(const View& view);
    bool HandleEvent(const Event* event);

    void renderTab(const View &view)
    {
        m_isTab = true;
        m_position = view.center;
        m_size = view.size;
        render(view);
    }

    void setCommand(SerialCommand *cmd) { m_command = cmd; }

    void onSwapIn();
    void onSwapOut();
    void onStep();
};

#endif
