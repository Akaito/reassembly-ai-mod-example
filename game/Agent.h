
#ifndef AGENT_H
#define AGENT_H

#include "Save.h"
#include "Player.h"

struct MapObjective;
struct Block;

static const int kAgentFactionBase = 100;

struct Agent {

    // serialized
    SaveData save;

    // runtime
    int                    index = -1;
    MapObjective          *obj   = NULL;
    watch_ptr<Block>       command;
    copy_ptr<FactionData>  fdata;

    Agent();
    Agent(int idx);
    Agent(SaveData &&s) NOEXCEPT;
    ~Agent();

    typedef int VisitEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        return (vis.VISIT(save) &&
                vis.VISIT_SKIP(string, "path"));
    }

    Faction_t getFaction() const { return kAgentFactionBase + index; }
    bool finalize(SaveGame *player);
    BlockList composeFleet() const;
};

#endif // AGENT_H

