
#ifndef CONSTRUCTOR_H
#define CONSTRUCTOR_H

#include "ControlPanel.h"
#include "Game.h"
#include "ClusterWindow.h"

struct StationInterface;

struct BBlock final : public ButtonBase {
        
    ClusterWindow       window;
    BlockCluster        cluster;
    string              subtext;
    ButtonText          bt;
    float               angle = 0.f;
    float               highlight = 0;
    const vector<uint> *ids = NULL;

    BBlock() : window(S_BOX) {}
    BBlock(int idx, const vector<uint> *idz) : window(S_BOX), ids(idz) { index = idx; }
    BBlock(const BlockCluster *bp) { resetBlueprint(bp); }

    void resetIdent(uint ident);
    void resetBlueprint(const BlockCluster *cl);

    void renderButton(DMesh& mesh, bool selected=false);
    void renderContents(const ShaderState &sstate);
    void renderDragged(const ShaderState &sstate, float2 pos);
    void setupWindowView();

    lstring getBlueprintName() const { return cluster.getName(); }
    uint getIdent() const { return cluster.size() ? cluster.blocks[0]->sb.ident : 0; }
};

struct UpgradeData final {
    int  costC = 0;
    int  costR = 0;
    bool known  = false;

    void setupButton(ButtonBase &wi) const;
    void renderButton(DMesh& mesh, const ButtonBase &wi, uint style) const;
    void renderContents(const ShaderState &ss, const ButtonBase &wi) const;
    bool canUpgrade() const;
    void payForUpgrade() const;
};

struct BlockPalette final {

    enum Mode { BP_CONSTRUCT, BP_EDIT, BP_UPGRADE_INV, BP_UPGRADE_PAL };
    
    float2               position;
    float2               size;

    Selection           *m_sel = NULL;
    Button               m_upgrade;
    Button               m_binding;
    mutable std::mutex   m_mutex;
    ButtonWindow<BBlock> m_blocks;

private:
    vector<BlockId_t>   *m_ids = NULL;
    vector<UpgradeData> *m_data = NULL;

    BBlock          *m_dragged = NULL;

    Button                m_editDelete;
    Mode                  m_mode;

    mutable string        m_hoveredStats;
    mutable const Block*  m_lastHovered = NULL; // don't dereference! only for comparison
    
public:

    BlockPalette();

    void setIdList(vector<BlockId_t> *ids);
    vector<BlockId_t> &getIds() { return *m_ids; }
    const vector<BlockId_t> &getIds() const { return *m_ids; }
    bool addPaletteId(BlockId_t ident);

    float2 getPos() const { return position; }
    float2 getSize() const { return size; }
    void setPosition(float2 windowSize, float ymin);

    int getDraggedId() const
    {
        return m_dragged ? m_dragged->index : -1;
    }
    
    uint getDraggedBlock() const
    {
        return m_dragged ? getIds()[m_dragged->index] : 0;
    }
    
    void setMode(Mode mode);
    
    void setModeUpgradeInventory(vector<UpgradeData> *dat, vector<BlockId_t> *ids);

    const BBlock* getButton(int i) const
    {
        return static_cast<BBlock*>(m_blocks.buttons[i]);
    }

    void render(const ShaderState &ss, const View& view);
    void renderTooltips(const ShaderState &ss, const View& view) const;
    bool renderDragged(const ShaderState &ss) const;
    string getTooltip() const;
    const BBlock *getHovered() const;
    void updateColors();
    
    bool HandleEvent(const Event* event, int *activated=NULL, int *dropped=NULL);

    bool HandleExternalDragEvent(const Event* event, uint blockId);
    
    bool isHovered() const { return m_blocks.hovered; }
};

struct ConstructEditor final : public GameState {

    enum Flags { ED_BLUEPRINT=1, ED_CHILD=2, ED_EXTRA=4, ED_SELECTOR=8 };

    float2                 m_windowSize;
    watch_ptr<Block>       m_command;
    BlockPalette           m_palette;
    ShipData               m_sdata;
    BlockCluster**         m_editCluster;
    const int              m_flags;
    const SerialCommand*   m_sc;

    Button                 m_exitButton;
    Button                 m_helpButton;
    string                 m_helpMessage;
    Button                 m_controlsButton;
    Button                 m_exportButton;    
    MessageBoxWidget       m_exportMessage;
    Button                 m_importButton;
    BlockCluster          *m_importCluster = NULL;
    Button                 m_testButton;
    copy_ptr<BlockCluster> m_testBlueprint;
    bool                   m_minihelpActive = false;
    bool                   m_bindingChanged = false;
    vector<Button*>        m_allButtons;
    TextInputBase          m_nameText;

    View                   m_bgview;

    ConstructEditor(BlockCluster** pcl, int st, const SerialCommand *sc);

    void OnNotify(const Notification& notif);

    void importCluster(BlockCluster *cl);
    void onPush();

    void saveForExit();
    bool canSave() const;
    bool canBuild() const;

    Block* getCommand();
    const Block* getCommand() const { return const_cast<ConstructEditor*>(this)->getCommand(); }
    
    BlockCluster *getCluster()
    {
        const Block *cmd = getCommand();
        return cmd ? cmd->cluster : NULL;
    }

    virtual ~ConstructEditor();

    virtual GameZone *step(bool first);
    virtual void updateView(bool sizeChanged) { m_cam.smoothZoom(m_view, sizeChanged); }

    void render(const View& view);
    void playMusic(float foreground, float introAnim);

    bool HandleEvent(const Event* event);

    virtual View&  getView() { return m_bgview; }
    const View& getActiveView() { return m_view; }
};



#endif // CONSTRUCTOR_H


