
#ifndef PORTPAIR_H
#define PORTPAIR_H

#include "Blocks.h"

struct Port;
struct WatchPortPair;
typedef vector<WatchPortPair> PortPairList;

struct PortPair final {
    Block* handleBlock  = NULL; // the block being moved around (moves)
    uint   handleIndex  = 0;
    Block* connectBlock = NULL; // the block that we are attaching to (does not move)
    uint   connectIndex = 0;
    float  distance     = 0.f;

    PortPair() NOEXCEPT {}
    PortPair(Block *hb, uint hi, Block* cb, uint ci) 
        : handleBlock(hb), handleIndex(hi), connectBlock(cb), connectIndex(ci) {}

    PortPair(const Port &handle, const Port& connect)
        : handleBlock(handle.block), handleIndex(handle.index), connectBlock(connect.block), connectIndex(connect.index) {}

    PortPair(Block *hb, uint hi, const Port& connect)
        : handleBlock(hb), handleIndex(hi), connectBlock(connect.block), connectIndex(connect.index) {}
    
    PortPair(const Port &handle, Block *cb, uint ci)
        : handleBlock(handle.block), handleIndex(handle.index), connectBlock(cb), connectIndex(ci) {}

    PortPair(Block* hb, uint hi)
        : handleBlock(hb), handleIndex(hi), 
          connectBlock(hb->ports[hi].block), connectIndex(hb->ports[hi].index) {}

    PortPair(const Port& handle)
        : handleBlock(handle.block), handleIndex(handle.index),
          connectBlock(handle.connected().block), connectIndex(handle.connected().index) {}

    friend bool operator==(const PortPair& a, const PortPair& b)
    {
        return (a.handleBlock == b.handleBlock &&
                a.connectBlock == b.connectBlock &&
                a.handleIndex == b.handleIndex &&
                a.connectIndex == b.connectIndex);
    }

    Port getConnect() const { return Port(connectBlock, connectIndex); }
    Port getHandle()  const { return Port(handleBlock, handleIndex); }

    const ShapeSpec::PortPos& getConnectPos() const { return connectBlock->spec->ports[connectIndex]; }
    const ShapeSpec::PortPos& getHandlePos()  const { return handleBlock->spec->ports[handleIndex]; }

    BlockPosition handlePosForConnection(const BlockPosition& connect) const;
    BlockPosition handlePosForConnection() const { return handlePosForConnection(BlockPosition(connectBlock)); }
    BlockPosition handleClusterPosForConnection(const BlockPosition& connect) const;
    BlockPosition handleClusterPosForConnection() const { return handleClusterPosForConnection(BlockPosition(connectBlock)); }

    float2 handleDirectionForConnection() const;

    void setConnect(const Port &p) { connectBlock = p.block;
                                     connectIndex = p.index; }
     void setHandle(const Port &p)  { handleBlock = p.block;
                                      handleIndex = p.index; }

     bool isConnected() const { return (live(connectBlock) && live(handleBlock) &&
                                       getConnect().connected().block == handleBlock &&
                                       getHandle().connected().block == connectBlock); }

    // connect two blocks within the same cluster via ports, and recalculate the position and rotation of the handle block.
    void connectPorts() const;
    void connectPortsOnly() const;        // don't recalc handle postion
    void connectPortsAttach() const; // also attach block to cluster
    void connectPortsAttachPatch() const; // also patch ports around block
    void reconnectPorts() const; // don't connect, just recalc pos
    void attach() const;

    // like connectPorts but both the connect and handle block may be part of larger clusters
    // move all blocks connected to the handle block
    void connectPortsRecursive(uint queryVal) const;
    void reconnectPortsRecursive(uint queryVal) const; // don't connect, just recalc pos
    void attachRecursive() const;

    void connectAttachSubcluster() const;
    

    // return true if connecting the ports would not cause any overlapping blocks
    enum CanConnectTests { 
        TYPE               = 1<<0, // check port types
        SPATIAL_CLUSTER    = 1<<1, // check if handle would intersect any block in connect->cluster
        SPATIAL_ALL        = 1<<2, // check if handle would intersect any block
        HANDLE_PORT_FREE   = 1<<3, // check if handle port is free
        CONNECT_PORT_FREE  = 1<<4, // check if connect port is free
        SPATIAL_SPACER     = 1<<8,
        LAUNCH_TYPE        = 1<<9,

        DEF_ASSEMBLE          = TYPE|SPATIAL_CLUSTER|CONNECT_PORT_FREE|HANDLE_PORT_FREE,
        DEF_TRANS_ASSM        = TYPE|SPATIAL_CLUSTER|CONNECT_PORT_FREE,
        DEF_PALETTE           = TYPE|SPATIAL_ALL|CONNECT_PORT_FREE|HANDLE_PORT_FREE,
        DEF_BLUEPRINT_HANDLE  = TYPE|SPATIAL_ALL|CONNECT_PORT_FREE,
        DEF_LAUNCH            = LAUNCH_TYPE|CONNECT_PORT_FREE,
        DEF_SEED              = TYPE|CONNECT_PORT_FREE,
        
    };
    bool canConnect(uint tests, GameZone* zone=NULL, const BlockPosition *connectPos=NULL,
                    const BlockCluster *spatialTestCluster=NULL) const;

    // as above, but take into account any blocks connected to the handle block
    bool canConnectPortsRecursive(GameZone* zone, const BlockPosition &connect,
                                  const BlockCluster* connectCluster, uint queryVal) const;
    uint getActiveConnections(vector<WatchPortPair>& activeConnections, bool connectClusterOnly) const;

    void disconnectPorts() const;

    bool exist() const;

    friend bool operator!=(const PortPair& a, const PortPair& b)
    {
        return !(a == b);
    }

    static bool lessByDist(const PortPair& a, const PortPair& b)
    {
        return a.distance < b.distance;
    }

    float getCurrentDistance() const;
    float2 getConnectAbsolutePos() const;
    float2 getConnectOffset() const;
    int tractorForAssembly(PortPairList& freeConnections);

    void swap()
    {
        std::swap(handleBlock, connectBlock);
        std::swap(handleIndex, connectIndex);
    }

    PortPair swapped() const
    {
        PortPair pp = *this;
        pp.swap();
        return pp;
    }
};

struct WatchPortPair final {
    watch_ptr<Block> handleBlock; // the block being moved around (moves)
    uint             handleIndex;
    watch_ptr<Block> connectBlock; // the block that we are attaching to (does not move)
    uint             connectIndex;

    // store drawing information to avoid threading problems
    float2           spline[4];
    float2           connectedHandlePos;
    float            connectedHandleAngle;

    bool exist() const { return connectBlock && handleBlock; }

    WatchPortPair(const PortPair &pp);

    explicit operator PortPair() const
    {
        return PortPair(handleBlock.get(), handleIndex,
                        connectBlock.get(), connectIndex);
    }
    
    void swap()
    {
        std::swap(handleBlock, connectBlock);
        std::swap(handleIndex, connectIndex);
    }
};


// find active pair list for cluster (i.e. cluster is moved by mouse)
// RADIUS and PORTMASK filter ports on CL
// if FEATURES is non zero, only connect to blocks with features
struct ActiveData;
void findConnectionsForCluster(BlockCluster *cl, ActiveData &active, float radius);

// connect ports output from above function
void connectActiveConnections(PortPairList& activeConnections);

PortPair findFirstSeedConnection(Port handle);

#endif
