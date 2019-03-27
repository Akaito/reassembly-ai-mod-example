
#ifndef STAGING_TOURNAMENT_H
#define STAGING_TOURNAMENT_H

#include "GUI.h"
#include "Tournament.h"
#include "Network.h"
#include "Export.h"

struct BSave final : public Button {

    std::mutex                    mutex;
    ClusterWindow                 window;
    int                           slot = -1;
    SaveData                      save;
    volatile SaveData::SaveStatus saveStatus;

    BSave()
    {
        textSize = 18.f;
        subtextSize = 14.f;
        
    }
    
    BSave(int slot) : BSave() { setSlot(slot); }
    
    void doLoad();
    void setSlot(int slot);
    bool setData(SaveData &data, const char* descr);

    bool isLoading() const  { return saveStatus == SaveData::LOADING; }
    bool isContinue() const { return saveStatus == SaveData::SAVE_OK; }
    bool isRegen() const { return saveStatus == SaveData::FLEET_ONLY; }
    bool isError() const { return saveStatus == SaveData::PARSE_ERROR; }
    bool isEmpty() const { return saveStatus == SaveData::NOT_FOUND; }
    bool isOld() const { return saveStatus == SaveData::OLD_VERSION; }

    void renderButton(DMesh& mesh, bool selected) override
    {
        if (!visible)
            return;
        PushButton(&mesh.tri, &mesh.line, position, size/2.f, getBGColor(), getFGColor(selected), alpha);
    }

    void renderContents(const ShaderState &ss) override;
    void renderContents1(const ShaderState &ss) override;

    ~BSave();
};

struct BTeam final : public Button {

    ClusterWindow window;
    TTeam         team;
    ButtonText      bt;
    
    BTeam() {}
    BTeam(const BlockCluster *cl) : team(cl) {}
    BTeam(const SaveData *sd) : team(sd) {}

    void renderContents(const ShaderState &ss) override
    {
        const BlockCluster *cl = team.leader();
        if (!cl || !visible)
            return;
        window.RecordDraw(cl, cl->getRenderAngle() - (M_PIf/8.f),
                          size - kButtonPad, &ss, position, alpha);
    }
    
    void renderContents1(const ShaderState &ss) override
    {
        const float tht = floor(size.y / 8.f);
        GLText::Fmt(ss, position - size/2.f + kButtonPad, GLText::LEFT,
                    SetAlphaAXXX(COLOR_ENERGIZE0, alpha), tht, "%d ships\n%dP",
                    (int)team.blueprints.size(), team.totalP);
        bt.renderText(ss, position + size/2.f - kButtonPad, size.x, GLText::DOWN_RIGHT,
                      MultAlphaAXXX(kGUIText, alpha), 0.5f * tht, tht, team.data.name.str());
    }
};


static string getSteamFriendsError()
{
    string neterror = Network::instance().getError();
    if (neterror.empty())
        return !Network::instance().isLoggedIn() ? _("Not logged in") : _("Unknown");
    return str_contains(neterror, "Unauthorized") ? string(_("Profile is Private")) : 
		str_substr(neterror, 0, 40);
}


struct GSStaging : public GameState {

    typedef vector<SaveData> State_t;

    mutable std::mutex   mutex;
    int                  slot = 0;
    State_t              leftState;
    
    ButtonWindow<BSave>  right0;
    ButtonWindow<BShip>  right1;
    int                  rightDepth = 0;
    Button               rightBack  = Button(_("<"));
    SaveData             importBlueprints; // for blueprints.lua
    BlockCluster        *importShip  = NULL;
    BSave               *shipsButton = NULL;
    Button               import      = Button(_("Import"));
    
    ButtonWindow<BTeam>  left0;
    ButtonWindow<BShip>  left1;
    int                  leftDepth = 0;
    BTeam               *leftTeam  = NULL;
    Button               leftBack  = Button(_("<"));
    Button               leftSave  = Button(_("Export"));
    TextInputBase        leftName;

    Button       options = Button(_("Options"));
    Button       start = Button(_("Start"));
    Button       clear = Button(_("Clear"));
    string       terror;

    ConfirmWidget confirm;

    int         downloadState = 0;
    float       progress = 0.f;
    static string cachedFriendAgents;

    GSStaging(GameState *mm=NULL)
    {
        m_height = 200.f;
        m_staticBackground = true;
        
        right0.dims = int2(2, 6);
        right1.dims = int2(3, 7);
        left0.dims = int2(3, 5);
        left1.dims = int2(3, 6);
        vector<int> slots = getUsedSaveSlots();
        foreach (int sl, slots)
        {
            BSave *sb = new BSave(sl);
            right0.pushButton(sb);
        }
        loadLeftState();
        loadTournamentOptions();

        leftName.sizeChars = int2(40, 1);
        leftName.textSize = 14.f;
        confirm.message = _("Really Clear?");
    }

    ~GSStaging() { }

    void playMusic(float foreground, float introAnim) override
    {
        playTournamentMusic(foreground, introAnim);
    }

    void leftUpdateTeam()
    {
        // hold mutex when called
        if (leftDepth == 0)
            return;
        leftTeam->team.blueprints.clear();
        foreach (const BShip *bu, left1.button_iter()) {
            BlockCluster *cl = bu->cluster;
            if (!cl)
                continue;
            for (int i=0; i<bu->count; i++)
                leftTeam->team.blueprints.push_back(cl);
        }
        leftTeam->team.data.name = lstring(leftName.str());
        leftTeam->team.data.author = lstring();
        leftTeam->team.update();
    }

    void leftAddShip(BShip *button, BlockCluster *cl, const Event *event)
    {
        ASSERT(leftDepth == 1);

        if (cl && !button)
        {
            foreach (BShip *bu, left1.button_iter()) {
                if (bu->cluster->hash == cl->hash) {
                    button = bu;
                    break;
                }
            }
            if (!button) {
                cl->setColors(leftTeam->team.getColors());
                left1.pushButton( (button = new BShip(cl) ));
                button->count = 0;
            }
        }

        if (button)
        {
            int quant = KeyState::instance()[OShiftKey] ? 5 : 1;
            if (button->count == 1 && quant == 5)
                quant = 4; // 1->5 instead of 1->6
            quant = (!event || event->key == 0) ? quant : (event->key == 1) ? -quant : 0;
            button->count = max(1, button->count + quant);
        }
    }

    void leftGoBack()
    {
        std::lock_guard<std::mutex> l(mutex);
        leftBack.pressed = false;
        leftUpdateTeam();
        leftTeam = NULL;
        leftDepth = 0;
    }

    void rightGoBack()
    {
        std::lock_guard<std::mutex> l(mutex);
        rightDepth = 0;
        rightBack.pressed = false;
    }
    
    bool saveLeftState()
    {
        std::lock_guard<std::mutex> l(mutex);
        leftUpdateTeam();
        State_t saveState;
        foreach (const BTeam *bu, left0.button_iter())
            saveState.push_back(bu->team.toSave());

        SaveSerializer ss;
        ss.setFlag(SaveSerializer::EXPLICIT_CLUSTERS_ALWAYS);
        ss.serialize(saveState);
        return SaveFile(str_format("data/tournament%d.lua", slot).c_str(), ss.str());
    }

    bool loadLeftState()
    {
        std::lock_guard<std::mutex> l(mutex);
        vec_clear_deep(left0.buttons);
        vec_clear_deep(left1.buttons);
        leftDepth = 0;
        if (!loadFileAndParse(str_format("data/tournament%d.lua", slot), &leftState))
            return false;
        foreach (SaveData &sd, leftState) {
            left0.pushButton(new BTeam(&sd));
        }
        return true;
    }

    virtual bool isAnimating() const override
    {
        if (downloadState != -1 && downloadState != 2)
            return true;
        std::lock_guard<std::mutex> l(mutex);
        foreach (const BSave *bu, right0.button_iter())
        {
            if (bu->isLoading())
                return true;
        }
        return false;
    }

    GameZone *step(bool first) override
    {
        if (rightDepth != 0)
            return NULL;

        auto &buttons = right0.buttons;
        for (int i=0; i<buttons.size(); i++)
        {
            BSave *sb = static_cast<BSave*>(buttons[i]);
            if (sb->isEmpty())
            {
                std::lock_guard<std::mutex> l(mutex);
                vec_erase(buttons, i);
                delete sb;
            }
        }

        if (Network::instance().isSteamAuth())
        {
            Network &nw = Network::instance();
            if (downloadState == 0 && !nw.isLoggedIn() && !nw.isBusy()) {
                downloadState = -1;
            } else if ((downloadState == 1 && nw.getOutput()) ||
                       (downloadState == 0 && cachedFriendAgents.size()))
            {
                SaveParser sp((downloadState == 1) ? nw.getOutput() : cachedFriendAgents.c_str());
                if (sp.size())
                {
                    sp.setProgress(&progress);
                    vector<SaveData> net_agents;
                    sp.parse(&net_agents); // might partially parse if timeout
                    if (net_agents.size())
                    {
                        DPRINT(NETWORK, ("Parsed %d agents", (int)net_agents.size()));
                        if (downloadState == 1)
                            cachedFriendAgents = nw.getOutput();
                        std::lock_guard<std::mutex> l(mutex);
                        downloadState = 2;
                        foreach (SaveData &sd, net_agents)
                        {
                            BSave *sb = new BSave;
                            if (sb->setData(sd, _("Friends"))) {
                                right0.buttons.push_back(sb);
                            } else {
                                delete sb;
                            }
                        }
                    }
                }

                if (downloadState != 2)
                    downloadState = -1;
            } else if (downloadState == 0 && nw.downloadFriends()) {
                downloadState = 1;
            }
        }

        if (importBlueprints.blueprints.size())
        {
            std::lock_guard<std::mutex> l(mutex);
            BSave *sb = new BSave;
            sb->setData(importBlueprints, str_format(_("Imported Fleet %s"), importBlueprints.name.c_str()).c_str());
            right0.buttons.push_back(sb);
        }

        if (importShip && importShip->size())
        {
            std::lock_guard<std::mutex> l(mutex);
            if (shipsButton) {
                shipsButton->text = str_format(_("%d ships"), 1+(int)shipsButton->save.blueprints.size());
                shipsButton->save.blueprints.push_back(importShip);
            } else {
                shipsButton = new BSave;
                if (right0.scrollbar.total > right0.scrollbar.visible)
                    right0.scrollbar.first = right0.scrollbar.total - right0.scrollbar.visible;
                right0.pushButton(shipsButton);
                SaveData sd;
                sd.version = kSaveCurrentVersion;
                sd.blueprints.push_back(importShip);
                shipsButton->setData(sd, _("Imported Ships"));
                shipsButton->text = _("1 ship");
            }
            importShip = NULL;
        }
        
        return NULL;
    }

    void render(const View& view) override
    {
        fillBackground(view.alpha);
        const ShaderState ss = view.getScreenShaderState();
        ButtonWindowBase &rite = rightDepth ? (ButtonWindowBase&)right1 : (ButtonWindowBase&)right0;
        ButtonWindowBase &lft = leftDepth ? (ButtonWindowBase&)left1 : (ButtonWindowBase&) left0;
        const float2 size = clamp_aspect(view.sizePoints, kAspectMinMax.x, kAspectMinMax.y);

        float2 ltpos;
        {
            float2 lsize = float2(0.45f, 0.8f) * size;
            float2 lpos = float2(0.25f, 0.5f) * view.sizePoints;

            ltpos = lpos + flipX(lsize/2.f);
            if (leftDepth == 1)
            {
                const float adj = leftName.size.y + 4.f * kButtonPad.y;
                lsize.y -= adj;
                lpos.y -= adj/2.f;
            }
        
            lft.size = lsize;
            lft.position = lpos;
        }
        rite.size = float2(0.45f, 0.8f) * size;
        rite.position = float2(0.75f, 0.5f) * view.sizePoints;

        int leftTotalP = 0;
        {
            std::lock_guard<std::mutex> l(mutex);
            renderWidget(lft, ss);
            renderWidget(rite, ss);

            if (leftDepth == 1) {
                foreach (const BShip *bu, left1.button_iter()) {
                    BlockCluster *cl = bu->cluster;
                    leftTotalP += cl ? bu->count * cl->getDeadliness() : 0;
                }
            }
        }

        const float hsize = 24.f;
        const uint tcolor = MultAlphaAXXX(kGUIText, view.alpha);
        GLText::Put(ss, rite.position + flipX(rite.size/2.f), GLText::LEFT, tcolor, hsize, _("Available"));
        GLText::Put(ss, ltpos, GLText::LEFT, tcolor, hsize, leftDepth ? _("Team") : _("Contestants"));
        
        const float2 bsize = float2(lft.size.x/4.f, 0.05f * size.y);

        if (leftDepth == 0)
        {
            options.size = bsize;
            options.setAboveBelow(lft, float2(-1.f));
            renderWidget(options, ss);

            start.size.x = bsize.x;
            start.active = lft.buttons.size() > 1;
            start.setAboveBelow(lft, float2(1.f, -1.f));
            renderWidget(start, ss);

            clear.size.x = bsize.x;
            clear.setAdjacent(start, justX(-1.f));
        }
        else
        {
            leftBack.size.x = bsize.x;
            leftBack.setAboveBelow(lft, float2(1.f, -1.f));
            renderWidget(leftBack, ss);
            leftSave.size.x = bsize.x;
            leftSave.setAboveBelow(lft, float2(-1.f));
            renderWidget(leftSave, ss);
            leftName.size.x = 0.75f * lft.size.x;
            leftName.setAboveBelow(lft, float2(-1.f, 1.f));
            renderWidget(leftName, ss);
            GLText::Fmt(ss, float2(lft.position.x + lft.size.x/2.f, leftName.position.y),
                        GLText::MID_RIGHT, SetAlphaAXXX(COLOR_P, view.alpha), 18.f, "%dP", leftTotalP);
            clear.setAdjacent(leftBack, justX(-1.f));
        }

        clear.active = lft.buttons.size() > 0;
        renderWidget(clear, ss);

        if (terror.size())
        {
            GLText::Put(ss, options.position - options.size / 2.f,
                        GLText::DOWN_LEFT, SetAlphaAXXX(COLOR_TARGET, view.alpha),
                        12.f, terror);
        }

        if (rightDepth == 0)
        {
            import.size.x = bsize.x;
            import.setAboveBelow(rite, float2(-1.f));
            renderWidget(import, ss);
        }
        else
        {
            rightBack.size.x = bsize.x;
            rightBack.setAboveBelow(rite, float2(1.f, -1.f));
            renderWidget(rightBack, ss);
        }

        if (Network::instance().isSteamAuth())
        {
            string netmsg;
            if (downloadState == -1) {
                netmsg = _("Friends Unavailable: ") + getSteamFriendsError();
            } else if (downloadState == 0) { 
                netmsg = _("Logging in...");
            } else if (downloadState == 1) {
                netmsg = str_format(_("Downloading Friend Ships: %.f%%"),
                                    100.f * Network::instance().getProgress());
            }
            if (netmsg.size()) {
                GLText::Put(ss, import.position + import.size/2.f + justX(kButtonPad),
                            GLText::DOWN_LEFT, tcolor, 0.6f * hsize, netmsg);
            }

            if (0 <= downloadState && downloadState < 2) {
                renderLoadingSpinner(ss, rite.position - justY(0.5f * rite.size / 2.f),
                                     rite.size * float2(1.f, 0.25f), view.alpha,
                                     0.5f * (Network::instance().getProgress() + progress));
            }
        }

        confirm.alpha = view.alpha;
        confirm.updateFade();
        confirm.render(ss, m_view);
    }

    bool HandleEvent(const Event* event) override
    {
        string msg;
        if (getExportOutput(msg))
        {
            m_list->pushAboveSelect(this, new GSMessageBox(msg, this));
            return true;
        }
        
        if (event->isEscape())
        {
            saveLeftState();
            m_list->popDown(this);
            return true;
        }

        if (leftDepth == 1 && leftName.HandleEvent(event))
            return true;

        if (event->isKeyDown(NSLeftArrowFunctionKey) ||
            event->isKeyDown(NSUpArrowFunctionKey) ||
            event->isKeyDown(NSUpArrowFunctionKey) ||
            event->isKeyDown(NSBackspaceCharacter))
        {
            if (leftDepth == 1 && left1.hovered) {
                leftGoBack();
            } else if (rightDepth == 1 && right1.hovered) {
                rightGoBack();
            }
            return true;
        }

        bool confirmClear = false;
        if (confirm.HandleEvent(event, &confirmClear))
        {
            if (confirmClear)
            {
                std::lock_guard<std::mutex> l(mutex);
                vec_clear_deep(leftDepth ? left1.buttons : left0.buttons);
            }
            return true;
        }
        
        bool handled = false;
        bool isActivate = false;

        if (leftDepth == 0)
        {
            BTeam *actv = NULL;
            BTeam *drag = NULL;
            BTeam *drop = NULL;
            if ((handled = left0.HandleEvent(event, &actv, &drag, &drop)))
            {
                if (drop && left0.getDropped(event))
                {
                    left0.swapButtons(drop, left0.consumeDropped(event));
                    return true;
                }
                else if (actv)
                {
                    std::lock_guard<std::mutex> l(mutex);
                    vec_clear_deep(left1.buttons);
                    std::map<uint, BShip *> hashToBu;
                    foreach (const BlockCluster *bp, actv->team.blueprints)
                    {
                        BShip *& bu = hashToBu[bp->hash];
                        if (bu) {
                            bu->count++;
                        } else {
                            bu = new BShip(const_cast<BlockCluster*>(bp));
                            left1.pushButton( bu );
                        }
                    }
                    left1.scrollbar.first = 0;
                    leftTeam = actv;
                    leftName.setText(leftTeam->team.data.name.c_str());
                    leftDepth = 1;
                }
                else if (drop)
                {
                    if (right1.getDropped(event))
                    {
                        drop->team.blueprints.push_back(right1.consumeDropped(event)->cluster);
                        drop->team.update();
                    }
                }
                else if (event->type == Event::MOUSE_UP)
                {
                    if (right0.getDropped(event))
                        left0.pushButton( new BTeam(&right0.consumeDropped(event)->save) );
                    else if (right1.getDropped(event))
                        left0.pushButton( new BTeam(right1.consumeDropped(event)->cluster) );
                }
            }
            if (left0.HandleDragExternal(event, drag, &drop))
            {
                if (drop && !left0.hovered)
                    left0.popButton(drop);
                return true;    // avoid hovering right side, etc.
            }
            if (handled)
                return true;

            if (options.HandleEvent(event, &isActivate))
            {
                if (isActivate)
                    m_list->pushAboveSelect(this, CreateGSTournamentOptions());
                return true;
            }
        }
        else if (leftDepth == 1)
        {
            BShip *actv = NULL;
            BShip *drag = NULL;
            BShip *drop = NULL;
            if ((handled = left1.HandleEvent(event, &actv, &drag, &drop)))
            {
                if (drop && left1.getDropped(event))
                {
                    left1.swapButtons(drop, left1.consumeDropped(event));
                    return true;
                }
                else if (right1.getDropped(event))
                {
                    leftAddShip(NULL, right1.consumeDropped(event)->cluster, NULL);
                }
                else if (actv)
                {
                    leftAddShip(actv, NULL, event);
                }
            }
            if (left1.HandleDragExternal(event, drag, &drop) && drop && !left1.hovered)
                left1.popButton(drop);
            if (handled)
                return true;

            if (ButtonHandleEvent(leftBack, event, &isActivate))
            {
                if (isActivate)
                    leftGoBack();
                return true;
            }

            if (ButtonHandleEvent(leftSave, event, &isActivate))
            {
                if (isActivate)
                {
                    std::lock_guard<std::mutex> l(mutex);
                    leftUpdateTeam();
                    SaveData sd = leftTeam->team.toSave();
                    startFleetExport(sd, "");
                }
                return true;
            }
        }

        if (rightDepth == 0)
        {
            BSave *drag = NULL;
            BSave *activate = NULL;
            if ((handled = right0.HandleEvent(event, &activate, &drag, NULL)))
            {
                if (activate) {
                    std::lock_guard<std::mutex> l(mutex);
                    BSave *bu = activate;
                    vec_clear_deep(right1.buttons);
                    ClusterList bps;
                    if (bu == shipsButton) {
                        bps = bu->save.blueprints;
                    } else {
                        foreach (const BlockCluster *bp, bu->save.getAllBlueprints())
                            bps.push_back(const_cast<BlockCluster*>(bp));
                    }
                    foreach (BlockCluster *bp, bps)
                    {
                        right1.pushButton( new BShip(bp) );
                    }
                    right1.scrollbar.first = 0;
                    rightDepth = 1;
                    return true;
                }
            }
            right0.HandleDragExternal(event, drag);
            if (handled)
                return true;

            if (ButtonHandleEvent(import, event, &isActivate))
            {
                if (isActivate)
                {
                    if (!importShip)
                        importShip = BlockCluster::pool_alloc();
                    ImportFleetShip(this, &importBlueprints, &importShip);
                }
                return true;
            }
        }
        else if (rightDepth == 1)
        {
            BShip *actv = NULL;
            BShip *drag = NULL;
            if ((handled = right1.HandleEvent(event, (leftDepth == 1) ? &actv : NULL, &drag, NULL)) && actv)
            {
                leftAddShip(NULL, actv->cluster, event);
            }
            
            right1.HandleDragExternal(event, drag);
            if (handled)
                return true;
            
            if (ButtonHandleEvent(rightBack, event, &isActivate))
            {
                if (isActivate)
                    rightGoBack();
                return true;
            }
        }

        if (ButtonHandleEvent(start, event, &isActivate))
        {
            if (isActivate)
            {
                saveLeftState();
                
                vector<TTeam> teams = vec_map<TTeam>(left0.button_iter(), [](const BTeam *bu) { return bu->team; });
                TournamentClass *klass = new TournamentClass(false);
                string error;
                if (klass->seedShips(teams, NULL, &error)) {
                    m_list->pushAboveSelect(this, CreateGSTournament(klass));
                    terror = "";
                } else {
                    delete klass;
                    terror = _("Error: ") + error;
                }
            }
            return true;
        }

        if (ButtonHandleEvent(clear, event, &isActivate))
        {
            if (isActivate)
                confirm.active = true;
            return true;
        }
        
        return false;
    }
};

string GSStaging::cachedFriendAgents;

#endif // STAGING_TOURNAMENT_H
