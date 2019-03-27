//
//  Ships.h
//  Outlaws
//
//  Created by Arthur Danskin on 11/2/12.
//  Copyright (c) 2013-2016 Arthur Danskin. All rights reserved.
//

#ifndef __Outlaws__Ships__
#define __Outlaws__Ships__

#include "Blocks.h"
#include "AI_modapi.h"

extern int kExtraShipsFaction;
struct OneMod;
struct ParserLocation;
struct LogRecorder;
struct SaveData;

#define CHRIS_DLL_TEST (1)

struct AIModData {
    //AI_t                aiid          = 0;
    DllHandle           dll           = nullptr;
    int                 versionMajor  = -1;
    int                 versionMinor  = -1;
    Mod_GetApiVersion   getApiVersion = nullptr;
    Mod_CreateAiActions createActions = nullptr;
    LoadStatus          loadStatus    = LS_UNKNOWN;

    AIModData() {}
};

struct FactionData {
    uint             color0 = 0;
    uint             color1 = 0;
    uint             color2 = 0;
    int              primaries = 2;
    ECommandFlags    aiflags;
    string           ainame;
    AIModData        aiMod;
    int              playable = 0;
    lstring          start;     // starting ship name
    string           name;      // name of faction
    int              thrustSFX = 0;
    lstring          thrustSound;

    Faction_t        faction = 0;
    string           root;
    const OneMod     *mod = NULL;

    typedef int VisitEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(color0) &&
            vis.VISIT(color1) &&
            vis.VISIT(color2) &&
            vis.VISIT(primaries) &&
            vis.VISIT(aiflags) &&
            vis.VISIT(ainame) &&
            vis.VISIT(playable) &&
            vis.VISIT(start) &&
            vis.VISIT(name) &&
            vis.VISIT(thrustSFX) &&
            vis.VISIT(thrustSound);
    }

    int3 getColors() const { return int3(color0, color1, color2); }
    void setColors(int3 v) { color0 = v.x; color1 = v.y; color2 = v.z; }

    FactionData() {}
    FactionData(const SaveData &save);
};

struct ShipLoader {

    typedef std::map<Faction_t, FactionData> FactionMap;
    
private:
    std::map<Faction_t, vector<const BlockCluster*> >  allShips;
    std::unordered_map<lstring, BlockCluster*>         blueprints;
    std::unordered_map<uint, const BlockCluster *>     blueprintHashes;
    bool                                               allowLoad = false;
    FactionMap                                         factionData;

    struct LoadShip {
        lstring name, author;
        string file;
        const OneMod *mod = NULL;
    };
    typedef std::map< uint, vector< LoadShip > > LoadFactions;
    
    int enumerateShips(const string &base, const OneMod *mod, LoadFactions &loads);
    LoadStatus loadFactionsFrom(const string &base, const OneMod *mod);
    LoadStatus loadExtraShipsFrom(const char* base, const char *mident, int *step, const OneMod *mod);
    
public:

    float loadProgress = 0.f;
    void setAllowLoad(bool al) { allowLoad = al; }

    void deleteAll();
    
    const BlockCluster* getBlueprint(lstring name, bool allowCreate=false) const;

    vector<lstring> getShipNames() const;
    vector<lstring> getShipNamesForGroup(Faction_t group) const;
    BlueprintList   getOrigBlueprints(Faction_t group) const;
    
    vector<Faction_t> getGroups() const;
    int expandShipRegexes(vector<lstring>& tier) const;

    const BlockCluster* loadShip(lstring name, BlockCluster *bc);
    const BlockCluster* loadShip(SerialCluster &sc, lstring name, lstring author,
                                 const FactionData *fac, const OneMod *mod);     // load a particular ship
    
    void loadTheShips();
    void loadFactionsOnly() { loadFactionsFrom("data", 0); }
    
    void reloadExtraShips(int *step);
    
    void updateColorsById(Faction_t group, int3 colors);

    vector<BlockId_t> getFactionBlocks(Faction_t faction) const;
    const BlockCluster *getBlueprintByHash(uint hash) const;

    // factions.lua
    const FactionData *getFactionData(Faction_t faction) const { return map_addr(factionData, faction); }
    const FactionData *getFactionData(const string& name) const;
    
    vector<int> getPlayableFactions() const;
    int isFactionPlayable(Faction_t faction) const;
    const BlockCluster *getPlayerStartShip(Faction_t faction) const;
    
    const FactionData *setFactionData(Faction_t num, const FactionData &dat)
    {
        FactionData &fac = factionData[num];
        fac = dat;
        return &fac;
    }
};


struct BlockEntry
{
    SerialBlock            first;
    copy_ptr<SerialBlock>  second;
    BlockType              bt;
    lstring                name;
    lstring                blurb;
    const OneMod          *mod = NULL;
};
typedef std::unordered_map<BlockId_t, unique_ptr<BlockEntry>> BlockMap;

struct IndexReadData {
    std::map<BlockId_t, SerialBlock>               tempBlocks;
    vector<BlockType*>                             freeList;
    std::unordered_map<BlockId_t, shared_ptr<ParserLocation>> blockSources;
    BlockMap                                       lastAllBlocks;

    IndexReadData();
    ~IndexReadData();
};

struct BlockIndex {

    const SerialBlock* serialBlockFromId(BlockId_t ident) const;
    
    const BlockMap& getAllBlocks() const { return allBlocks; }
    BlockEntry *getRecolorBase(BlockId_t ident);

    const vector<BlockId_t> &getGroupIds(Faction_t group) const {
        static const vector<BlockId_t> def;
        return map_get(groupBlocks, group, def);
    }

    bool readTheBlocks(int reload, float *progress);
    void onParseBlock(SaveParser *sp, SerialBlock *sb, bool is_ref, shared_ptr<ParserLocation> &loc);

    void recolorGroup(Faction_t group, int3 colors);
    int4 getRecolor() const { return m_recolor; }
    
    const SerialBlock *getFactionCommand(Faction_t fac) const;

    const OneMod                      *currentMod = NULL;

    const SerialBlock *intern(SerialBlock &sb, shared_ptr<ParserLocation> &loc);
    enum ReadPriority { RD_DISABLE, RD_OVERWRITE, RD_FILL };
    bool isReading() const { return m_isReading != RD_DISABLE; }
    void setReading(ReadPriority is);
    LogRecorder *getLogger() const;
    const BlockEntry *get_entry(BlockId_t ident);

    void deleteAll();

    const BlockType *get_blocktype(BlockId_t ident);
    unique_ptr<IndexReadData> read;

private:
    void createTheBlocks(int reload);
    bool readBlocksFrom(const OneMod *mod);
    BlockEntry &get_create_entry(BlockId_t ident);

    int4      m_recolor;
    ReadPriority m_isReading = RD_DISABLE;
    BlockId_t lastIdent   = 0;
    BlockMap  allBlocks;
    std::map<Faction_t, vector<BlockId_t> > groupBlocks;
};


// random cluster creation
BlockCluster* createTileAsteroid(int size, int sides1, int sides2, int sides3);
BlockCluster* createRandomCluster(const vector< pair<const SerialBlock *, uint> > &blocks);
BlockCluster* createPenroseAsteroid(int iterations);
bool refactionCluster(BlockCluster *cl, Faction_t faction, const Block **firstFailed);

static const int kAsteroidTilings = 7;
BlockCluster *createRandomTiling(int size, int type);

static const int kAsteroidTypes = 12;
BlockCluster *createRandomAsteroid(int size, int type);

static const int kPlantColors = 3;
static const int kPlantSubcolors = 5;
Block *createRandomPlant(int coloridx, int subidx, int size);

void writeBlocksToFile(const string& root, const vector<SerialBlock*> &blocks, bool writestats);
bool parseShipName(const char* file, string *name, Faction_t *faction);
lstring relocateShip(lstring name, const OneMod *mod);

const BlockCluster *getIndexedBlueprint(lstring name, bool allowCreate=false);

void clearTempZone();
void releaseClusters(SerialBlock *sb);

#endif /* defined(__Outlaws__Ships__) */
