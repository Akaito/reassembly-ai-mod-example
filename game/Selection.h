
#ifndef SELECTION_H
#define SELECTION_H

#include "GUI.h"

struct GameZone;
struct ActiveData;

struct SelectionStats {

    int    count = 0;
    float  mass   = 0.f;
    int    energy = 0;
    float  health = 0.f;
    AABBox bbox;
};

struct Selection {

    typedef pair<BlockCluster*, uint> RotationHandle;

private:
    bool                       rectEnabled = false;
    float2                     rectStart;

    bool                       draggedBlockDragged = false; // true if draggedBlock actually moved
    float2                     dragOffset;
    copy_ptr<ActiveData>       active;
    watch_ptr<Block>           lastBlock[2];

    bool                       rotationHandleRotated = false; // true if cluster actually moved
    RotationHandle             rotationHandle;

    bool                       rotationKeyRotated = false;
    vector<float3>             rotationKeyClusterStart;
    float                      rotationStartAngle;
    float2                     rotationCenter;

    float                      selectionBlinkTime = 0.f;
    double                     lastDragTime = 0.0;
    OverlayMessage             message;

    watch_ptr<Block>           hoveredBlock;

    GameZone                  *zone = NULL;
    SaveGame                  *save = NULL;

    // construction mode
    mutable std::recursive_mutex mutex;
    WatchBlockList             blocks;
    watch_ptr<Block>           draggedBlock;
    ClusterList                clusters;
    ClusterList                clipboard;

    // command mode
    WatchBlockList             commands;
public:

    Selection(GameZone *z);
    ~Selection();

    void CancelDrag();
    void Clear(bool doundo=true);
    bool Duplicate(float2 cursor);
    bool Delete();
    int Replace(int from, int to);
    AABBox getBBox() const;
    void Mirror();
    bool Scale(int diff, bool allowReset=true);
    void Rotate(float angle, float clamp);
    void Purge();
    void SetDragged(BlockCluster *sel);
    void SetSelected(BlockCluster *sel);
    void SetSelected(const BlockList &bl);
    void SetSelected(const ClusterList &cls);
    bool OnSwitchMode(UIMode mode);

    void Render(const View &view);
    void RenderImpendingDelete(LineMesh<VertexPosColor> &mesh, const View& view) const;

    bool OnUpdateHandleKeys();
    bool OnMouseDown(uint key);
    bool OnMouseDragged(uint key, bool append);
    bool OnMouseUp(uint key);
    bool OnKeyDown(const Event* event);
    bool OnScroll(float vel);

    bool isSelected(const Block* block) const;
    bool isDragged(const Block* block) const;
    Block* getEditorSelectedCluster();
    BlockList getEditorSelectedCommands();
    ClusterList getEditorSelectedClusters();
    BlockList getSelectedBlocks() const { return vec_map(blocks, [](const watch_ptr<Block>& pbl) { return pbl.get(); }); };

    const Block *getDragged() const { return draggedBlock.get(); }
    Block *mouseSelectBlock();

    void Copy();
    void Paste(float2 pos);

    void coverBlock(const Block& bl, float mult);

    SelectionStats getStats() const;
    float2 drawStats(const ShaderState &ss, float2 pos, float2 relnorm, float alpha) const;
    const Block *getHovered() const { return hoveredBlock.get(); }
    void setMessage(const string& msg) { message.setMessage(msg); }
    
    bool isEmpty() const;

    void setSave(SaveGame *sv) { save = sv; }
    GameZone *getZone() { return zone; }
    std::recursive_mutex &getMutex() const { return mutex; }

    void saveSelectedBlockPositions(vector<float2> &sel);
    void restoreSelectedBlockPositions(const vector<float2> &sel);
    
private:

    void UnselectUnique();
    bool CheckBlockLimit();
    static void accumulateCommands(cpShape* shape, void* data);
    static void accumulateBlocks(cpShape* shape, cpContactPointSet *points, void* data);
    void setDestinationForCommands(float2 destination);

    bool dragSelected();
    void selectBlocksToClusters();

    cpBB getRectBB(float2 cursorPosWorld) const
    {
        cpBB bb = {min(rectStart.x, cursorPosWorld.x), // l
                   min(rectStart.y, cursorPosWorld.y), // b
                   max(rectStart.x, cursorPosWorld.x), // r
                   max(rectStart.y, cursorPosWorld.y)}; // t
        return bb;
    }

    

    // relative to cluster pos/angle
    float2 getRotationHandleOffset(const RotationHandle& rhl) const;
    float   getRotationHandleWorldRadius(BlockCluster *cl) const;
    RotationHandle getHoveredRotationHandle(float2 cpos);

    AABBox getClusterBBox() const;

};


#endif // SELECTION_H
