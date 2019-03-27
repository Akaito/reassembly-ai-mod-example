
#ifndef EDITOR_H
#define EDITOR_H

#include "Game.h"

struct Sector;
struct MapObjective;
struct ImportShips;

extern int kHeadlessMode;

struct GSEditor : public GameState
{
    float2                cursorPos;
    float                 cursorAngle = 0.f;
    TextInputCommandLine  console;
    bool                  warpedMouse = false;
    string                script;
    float                 scriptTime = 0.f;

    watch_ptr<Block>      m_player;
    uint64                m_dbgRender;
    bool                  m_viewMode;

    watch_ptr<ResourcePocket> m_mouseRp;
    watch_ptr<MapObjective>   m_mousePb;

    Sector*               m_sector = NULL;
    unique_ptr<ImportShips> m_import;

    Block* getSelectedCommand() const;
    vector<Block*> getSelectedCommands() const;
    Sector* getSector();

    BlockCluster *addShip(BlockCluster *cl);

    bool sectorGen(uint level);

    float getSectorSize()  const;
    int2 getSectorPos()    const;
    int2 getSectorCenter() const;
    Level* getLevel();

    GSEditor(GameZone* z);
    ~GSEditor();
    void onClose();
    void onPush() override;

    void updateView(bool sizeChanged) override;

    void playMusic(float foreground, float introAnim) override;
    void render(const View &offset) override;
    void renderCursor(const View &view) override;

    GameZone* step(bool first) override;
    void switchToMode(UIMode mode);
    bool HandleEvent(const Event* event) override;

    bool isEditor() const override { return true; }

    SaveGame *getSave();
};

void doDebugReload(int which);
bool editorSaveCluster(string sname, BlockCluster *cl, string &error, bool reload);

vector<BlockCluster*> loadShipsFromDirectory(const char* dirpath);

string cmd_arena(void* data, const char* name, const char *args);
string cmd_test(void* data, const char* name, const char *args);
string cmd_screenshot(void* data, const char* name, const char *args);

string cmd_macro_play(void* data, const char* name, const char *args);
string cmd_macro_load(void* data, const char* name, const char *args);
string cmd_macro_save(void* data, const char* name, const char *args);

#endif
