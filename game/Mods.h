
#ifndef THE_MODS_H
#define THE_MODS_H

#include "SerialBlock.h"
#include "Steam.h"

static const int kModBlockReloc0Max = 200;
static const int kModBlockReloc1Min = 17000;
static const int kModBlockReloc1Max = 26000;

static const int kModFactionRelocMin = 20;
static const int kModFactionRelocMax = 100;

//static const int kModAiRelocMin = 20; // TODO : CHRIS : Mod AI ID relocation.
//static const int kModAiRelocMax = 100;

static const int kModRelocRange = 10000; // address space of each relocation

class GLTexture;
struct SaveData;
struct FactionData;
struct LogRecorder;

extern string kModDocsUrl;

bool containsModBlock(const BlockCluster *bp);
void enumerateModBlocks(const BlockCluster *bp, std::set<BlockId_t> &bids);

#define REPLACE_FIELDS(F)                       \
    F(SHIPS, 1<<0)                              \
    F(EXTRA_SHIPS, 1<<1)                        \
    F(BLOCKS, 1<<2)                             \


DEFINE_ENUM(uint, EModReplace, REPLACE_FIELDS);

#define MOD_STATUS_FIELDS(F)                    \
    F(OK, 1)                                    \
    F(UNAVAILABLE, 2)                           \
    F(DOWNLOADING, 3)                           \
    F(CORRUPT, 5)                               \
    F(UPDATING, 8)                              \
    F(CREATING, 9)                              \
    F(STEAM_ERROR, 10)                          \
    F(UNSUBSCRIBED, 11)                         \


DEFINE_ENUM(uint, EModStatus, MOD_STATUS_FIELDS);

#define MOD_TYPE_FIELDS(F)                      \
    F(LOCAL, 1)                                 \
    F(WORKSHOP, 2)                              \
    F(PUBLISHED, 3)                             \
    
DEFINE_ENUM(uint, EModType, MOD_TYPE_FIELDS);

const FactionData *saveSlotToMod(const SaveData &save, int slot, bool newblocks);


struct ModEntry {
    string            name;     // name of folder OR title for workshop
    bool              enabled = true;
    EModType          type;
    PublishedFileId_t steam_pfid = 0;

    typedef int VisitEnabled;
    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(name) &&
            vis.VISIT_DEF(enabled, true) &&
            vis.VISIT(type) &&
            vis.VISIT(steam_pfid);
    }

    string toString() const;
};

struct OneMod : public ModEntry {

    string            title;    // pretty title set on steam
    string            author;
    string            description;
    EModReplace       replace;
    uint              relocId = 0;

    std::recursive_mutex mutex;
    string               folder;
    uint64               steamIdOwner  = 0;
    bool                 ownedByPlayer = false;
    volatile bool        gotDetails    = false;
    
    string               error;
    copy_ptr<LogRecorder> loadLog;
    
    mutable EModStatus           status;
    mutable string               currentSource;
    mutable std::set<Faction_t>  factions;
    mutable std::set<lstring>    ships;
    mutable std::set<BlockId_t>  blocks;

#if HAS_STEAM
    UGCUpdateHandle_t    update        = 0;
    STEAM_CALLRESULT(OneMod, SteamUGCRequestUGCDetailsResult);
    STEAM_CALLRESULT(OneMod, SubmitItemUpdateResult);
    STEAM_CALLRESULT(OneMod, CreateItemResult);
    
    UGCUpdateHandle_t updateWorkshop();
#endif

    OneMod() {}
    OneMod(const ModEntry &en);
    void load();

    // store list of factions
    // relocate new faction numbers
    // relocate block indices

    typedef int VisitEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(title) &&
            vis.VISIT(author) &&
            vis.VISIT(description) &&
            vis.VISIT(replace) &&
            vis.VISIT(relocId);
    }

    static string getFolder(const OneMod *mod) { return mod->folder; }

    uint relocateId(uint ident) const;
    uint unrelocateId(uint ident) const;

    BlockId_t relocateBlockId(BlockId_t bid) const
    {
        if (!relocId)
            return bid;
        if ((0 < bid && bid < kModBlockReloc0Max) ||
            (kModBlockReloc1Min <= bid && bid < kModBlockReloc1Max))
        {
            if (kModBlockReloc1Min <= bid && bid < kModBlockReloc1Max)
                bid = bid - kModBlockReloc1Min + kModBlockReloc0Max;
            return relocateId(bid);
        }
        return bid;
    }

    BlockId_t unrelocateBlockId(BlockId_t bid) const
    {
        if (bid < kModRelocBase || !relocId)
            return bid;
        bid = unrelocateId(bid);
        if (bid > kModBlockReloc0Max)
            bid += kModBlockReloc1Min - kModBlockReloc0Max;
        return bid;
    }

    Faction_t relocateFaction(Faction_t fac) const
    {
        if (kModFactionRelocMin <= fac && fac < kModFactionRelocMax)
            return relocateId(fac);
        return fac;
    }

};


struct TheMods {

private:
    std::recursive_mutex m_mutex;
    string               load_index;
    bool                 dirty = false;
    bool                 markedDirty = false;

    STEAM_CALLBACK_NAMED(TheMods, RemoteStoragePublishedFileSubscribed);
    STEAM_CALLBACK_NAMED(TheMods, RemoteStoragePublishedFileUnsubscribed);
    STEAM_CALLBACK_NAMED(TheMods, ItemInstalled);
    STEAM_CALLBACK_NAMED(TheMods, PersonaStateChange);

public:
    vector<OneMod*>  all_mods;

    vector<OneMod*>  load_mods; // highest priority first
    vector<OneMod *> merge_order; // same as load_mods in reverse order
    
    TheMods();
    ~TheMods();

    string getPath(const string &file) const { return getPath(file.c_str()); }
    string getPath(const char* file) const;
    SaveParser getParser(const string &file);
    
    bool isDirty() const { return dirty || markedDirty; }
    void markDirty() { markedDirty = true; }

    bool anyReplace(EModReplace stats) const;
    OneMod *getByReloc(uint reloc);

    int loadMods(int reload, float *progress);
    void loadModEntry(const ModEntry &en);
    void loadModCVars();

    bool writeIndex();

    void onLoadFinish();

    static TheMods &instance()
    {
        static TheMods mods;
        return mods;
    }
};

struct GameState;

GameState *CreateGSMods(GameState *mm);

#define DPRINT_MOD(M, X) { \
        const string msg_ = str_format X; \
        DPRINT(MODS, ("%s", msg_.c_str())); \
        if (M) { (M)->loadLog->Report(msg_); }}

template <typename T>
LoadStatus loadFileAndParseMod(const string& fname, T* data, const OneMod *mod, SaveParser &p)
{
    if (mod)
    {
        p.setLogger(mod->loadLog.get());
        mod->currentSource = fname;
    }
    return loadFileAndParseMaybe1(fname, data, p);
}

template <typename T>
LoadStatus loadFileAndParseMod(const string& fname, T* data, const OneMod *mod)
{
    SaveParser p;
    return loadFileAndParseMod(fname, data, mod, p);
}

LoadStatus loadAiMod(const string& base, const string& ainame, FactionData* data, const OneMod *mod);


#endif // THE_MODS_H

