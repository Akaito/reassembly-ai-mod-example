
#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include "Game.h"
#include "ClusterWindow.h"

struct BBlock;
struct TournamentClass;

GameState *CreateGSTournamentOptions();
GameState *CreateGSTournament(TournamentClass *klass);
void loadTournamentOptions();
void playTournamentMusic(float foreground, float introAnim);


struct TTeam final {
    ShipData      data;
    BlueprintList blueprints;

    lstring       path;
    int           totalP = 0;

    int3 getColors() const { return int3(data.color0, data.color1, data.color2); }
    
    TTeam &operator=(const BlockCluster *cl);
    TTeam &operator=(const BlueprintList &bp);
    TTeam &operator=(const SaveData *sd);

    TTeam &operator=(const TTeam &tt)
    {
        data = tt.data;
        blueprints = tt.blueprints;
        path = tt.path;
        update();
        return *this;
    }

    TTeam &operator=(TTeam &&tt)
    {
        data = tt.data;
        blueprints = std::move(tt.blueprints);
        path = tt.path;
        update();
        return *this;
    }

    operator bool() const { return blueprints.size() > 0; }
    size_t size() const { return blueprints.size(); }

    bool operator<(const TTeam &o) const { return data.name < o.data.name; }
    bool operator==(const TTeam &o) const { return data.name == o.data.name; }

    TTeam() {}
    TTeam(const BlockCluster *cl) { *this = cl; }
    TTeam(const BlueprintList &bp) { *this = bp; }
    TTeam(const SaveData *sd) { *this = sd; }
    TTeam(const TTeam &tt) { *this = tt; }
    TTeam(TTeam &&tt) { *this = std::move(tt); }

    const BlockCluster *leader() const { return blueprints.size() ? blueprints[0] : NULL; }
    string getPrettyName() const;
    
    SaveData toSave() const;
    TTeam clone() const;
    void update();
};

struct ArenaMatch {

    // winners of LEFT and RIGHT compete in this match
    // NEXT points to the match that the winner of this match will advance to
    // LNEXT points to the match that the looser will advance to
    ArenaMatch         *left = NULL, *right=NULL, *next=NULL, *lnext=NULL;
    ArenaMatch         *lprev=NULL; // looser of which wins this match
    TTeam               winner;
    int                 acquired = -1; // used by headless pool allocator
    bool                aborted = false;

    TTeam getLeft()  const { return left ? left->winner : TTeam(); }
    TTeam getRight() const { return right ? right->winner : TTeam(); }
    
    ArenaMatch() {}
    ArenaMatch(const TTeam &tt) : winner(tt) {}
    ArenaMatch(ArenaMatch *l, ArenaMatch *r);
    
    bool isPlaceholder() const
    {
        return (!winner && !lprev &&
                (!left || left->isPlaceholder()) &&
                (!right || right->isPlaceholder()));
    }

    void advanceByes();
    void setWinner(int idx);
};

struct TournamentClass final {

    string         name;
    string         path;
    float          minP = 0, maxP=0;
    std::set<uint> allowedBlocks;
    bool           owned;
    bool           ownsShips = false;

    typedef int VisitEnabled;

    template <typename V>
    bool accept(V& vis)
    {
        return (vis.VISIT(name) && 
                vis.VISIT(minP) &&
                vis.VISIT(maxP) &&
                vis.VISIT(path) &&
                vis.VISIT(allowedBlocks));
    }

    vector<TTeam> teams;

    TournamentClass(bool o=true) : owned(o) {}

    ~TournamentClass()
    {
        if (ownsShips)
        {
            foreach (auto &&x, teams)
                BlockCluster::pool_free(x.blueprints);
        }
    }

    string checkShip(const BlockCluster *bp) const;
    string addTeam(TTeam tt);
    void   loadShips();
    bool   seedShips(const vector<TTeam> &teams, MetaZone *mz, string *error);
};

struct IMatches {

    TournamentClass *klass;

    IMatches(TournamentClass *k) : klass(k) {}
    
    virtual ~IMatches()
    {
        if (klass && !klass->owned)
            deleteNull(klass);
    }

    virtual ArenaMatch *getNextMatch() = 0;
};

struct Bracket final : public IMatches {

    const int elimination;
    ArenaMatch *matches = NULL;

    ~Bracket();
    Bracket(TournamentClass *k, int elim);
    ArenaMatch *getNextMatch();
};

struct TournamentPool final : public IMatches {
  
    vector<ArenaMatch*> seed;
    vector<ArenaMatch*> matches;
    bool                isTest;

    ~TournamentPool()
    {
        vec_clear_deep(seed);
        vec_clear_deep(matches);
    }

    TournamentPool(TournamentClass *k, bool it=false);
    ArenaMatch *getNextMatch();
};

struct ChaseCam {

    struct CamFocus {
        LinearRegression line;
        AABBox           box;
        float2           vel;

        void insert(float2 pos, float rad, float2 v)
        {
            box.insertCircle(pos, rad);
            vel += v;
            line.insert(pos);
        }

        float getSize() const
        {
            f2 rad = box.getRadius();
            const float aspect_wh = globals.windowSizePoints.x / globals.windowSizePoints.y;
            return length(max(rad, f2(rad.y * aspect_wh, rad.x / aspect_wh)));
        }
    };

private:

    // shared / render state
    SlopeLine  slope;
    float      slope_swap = 1.f;
    AABBox     bbox;
    float2     cvel;

public:

    int    followIdx = 0;
    float2 initialRadius;

    float2 seval(float x) const { return float2(x, slope.eval(x)); }

    void update(const CamFocus &focus);
    void reset();
    void updateView(View &view, bool sizeChanged, FlyCam &cam, float timeFactor);
};

struct GSArena final : public GameState {

    ArenaMatch       *match = NULL;
    vector<TTeam>     teams;
    int               round = 0;
    float             endTime = 0.f;
    float             m_size = 0.f;
    mutable float     m_radius = 0.f;
    bool              m_advanced = false;

    copy_ptr<MetaZone>     m_mz;
    copy_ptr<SaveGame>     m_save;
    copy_ptr<MapObjective> m_field;

    struct ShipSummary {
        watch_ptr<Block> command;
        const int        pointsTotal;
        int              pointsUndamaged  = 0;
        ShipSummary(Block *bl) : command(bl),
                                 pointsTotal(bl->getBlueprintDeadliness()),
                                 pointsUndamaged(pointsTotal) {}
    };
    
    struct FleetSummary {
        vector<ShipSummary>   ships;
        string                name;
        string                author;
        uint                  color         = 0;
        int                   pointsUndamaged = 0; // scored against us
        int                   liveCount     = 0;
        const char*           state         = "";
        int                   faction = 0;
        int                   wins = 0;

        int points() const;
    };

    int window_teams[2];
    mutable ClusterWindow windows[2];
    vector<FleetSummary> sdat;
    ChaseCam          m_chase;
    int               winner=-1;
    int               lastwin=-1;
    OverlayMessage    helpText;
    bool              viewMode = false;
    bool              manualCamera = false;

    float getDeployRadius() const;
    void init();
    int teamCount() const { return match ? 2 : teams.size(); }
    TTeam getTeam(int idx) const
    {
        return match ? (idx ? match->getRight() : match->getLeft()) : teams[idx];
    }
    float arenaRadius() const;
    
    void spawnFleet(GameZone *zone, int idx, ChaseCam::CamFocus &cam, float rad);
    void reset();
    GSArena(ArenaMatch* mch);
    GSArena(vector<TTeam> tt);
    void onPush() override;

    ~GSArena();
    
    void updateView(bool sizeChanged) override;

    void playMusic(float foreground, float introAnim) override
    {
        playTournamentMusic(foreground, introAnim);
    }

    void renderWindow(const ShaderState &ss, DMesh &mesh, vector<TextBoxString> &text,
                      const View &view, int idx, float2 pos, float2 rad, float alpha);
    void render(const View &view) override;

    void renderCursor(const View &view) override { }

    GameZone* step(bool fs) override;
    
    bool HandleEvent(const Event* event) override;
};


struct GSTournamentBase : public GameState {

    vector<BBlock*>                      buttons;
    float                                alpha = 0.f;
    std::map<const ArenaMatch*, BBlock>  butmap;
    IMatches                            *matches   = NULL;
    ArenaMatch                          *next      = NULL;
    float                                startTime = 0.f;
    float                                endTime   = 0.f;

    GSTournamentBase()
    {
        m_staticBackground = true;
    }

    virtual AABBox positionButtons(const View &view) = 0;
    virtual void renderOver(const ShaderState &ss, const View &view)  {}
    virtual bool isAnimating() const override { return true; }

    void render(const View &view) override;
    GameZone *step(bool fs) override;

    bool HandleEvent(const Event* event) override;
};


struct GSBracket final : public GSTournamentBase {

    int     depth = 0;
    Bracket bracket;

    int createButtons(ArenaMatch *m);
    
    GSBracket(TournamentClass *k, int elim) : bracket(k, elim)
    {
        depth = createButtons(bracket.matches);
        matches = &bracket;
    }

    AABBox positionButtons(const View &view) override;
    void setupTree(ArenaMatch *m, int dep, float2 pos, float2 delta);
};

struct GSPool final : public GSTournamentBase {

    TournamentPool pool;
    
    GSPool(TournamentClass *p, bool it=false);

    AABBox positionButtons(const View &view) override;
    void renderOver(const ShaderState &ss, const View &view) override;
};


struct GSBrackets final : public GameState {

    std::mutex mutex;
    vector<TournamentClass> classes;
    vector<BlockCluster *>  allships;
    float                   startTime = 0.f;
    int                     index = -1;
    
    GSBrackets();

    void render(const View &view);
    GameZone* step(bool fs);
};


#endif // TOURNAMENT_H
