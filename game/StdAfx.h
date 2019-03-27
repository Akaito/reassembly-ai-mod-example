//
//  StdAfx.h
//  Outlaws
//
//  Created by Arthur Danskin on 11/2/12.
//  Copyright (c) 2013-2016 Arthur Danskin. All rights reserved.
//

#ifndef Outlaws_StdAfx_h
#define Outlaws_StdAfx_h

#if WIN32
#  ifdef BUILDING_REASSEMBLY
#    pragma warning(disable: 4251)
#    define DLLFACE __declspec(dllexport)
#  else
#  define DLLFACE __declspec(dllimport)
#  endif
#else
# define DLLFACE
#endif

#include <cstring>
#include <cstdarg>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <queue>
#include <string>
#include <iterator>
#include <type_traits>
#include <memory>

#include <utility>
#include <typeinfo>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <array>

using std::unordered_map;
using std::unordered_set;
using std::priority_queue;

// always use doubles
#define CP_USE_DOUBLES 1
#if !(defined(__LP64__) && __LP64__)
#  define CP_USE_CGPOINTS 0
#endif

#define CP_ALLOW_PRIVATE_ACCESS 1

#include "../chipmunk/include/chipmunk/chipmunk.h"
#include "../chipmunk/include/chipmunk/chipmunk_unsafe.h"

#include "StdAfx_core.h"

static const int kSteamAppid = 329130;
static const int kSteamAppidFieldsDLC = 1012980;


// for GOG stat based achievement unlocking
struct AchievementStat {
    const char* stat;
    int         minval;
    const char* achievement;
};

#include "Steam.h"

#ifdef POSIX
#undef POSIX
#endif
#include "re2/re2.h"

using re2::RE2;

inline cpVect cv(const float2 &v) { return cpv(v.x, v.y); }
inline cpVect cv(const double2 &v) { return cpv(v.x, v.y); }
inline float2 f2v(const cpVect &v) { return float2(v.x, v.y); }
inline float2 f2v(const double2 &v) { return float2(v); }
inline double2 d2v(const cpVect &v) { return double2(v.x, v.y); }
inline double2 d2v(const float2 &v) { return double2(v); }

#include "Types.h"

// do it like this instead of using templates to avoid including huge header files everywhere
#define DCCV(TYPE) TYPE CreateCVar_ ## TYPE(const char* name, TYPE* vptr, TYPE value, const char* comment="")
DCCV(float);
DCCV(int);
DCCV(uint);
DCCV(bool);
DCCV(float2);
DCCV(int2);
#undef DCCV

#define DEFINE_CVAR(TYPE, NAME, VALUE)                           \
    TYPE NAME = CreateCVar_ ## TYPE (#NAME, &NAME, (VALUE))

#define DBG_TYPES(F)                            \
    F(SENSORS   , uint64(1)<<0)                         \
    F(NAV_PATH  , uint64(1)<<1)                         \
    F(CONNECT   , uint64(1)<<2)                         \
    F(AI        , uint64(1)<<3)                         \
    F(TOGGLE    , uint64(1)<<4)                         \
    F(ASSEMBLY  , uint64(1)<<5)                         \
    F(SOUND     , uint64(1)<<6)                         \
    F(RESOURCES , uint64(1)<<7)                         \
    F(SECTOR    , uint64(1)<<8)                         \
    F(SPACERS  , uint64(1)<<9)                          \
    F(FPS      , uint64(1)<<10)                          \
    F(STUTTER  , uint64(1)<<11)                          \
    F(SPATIAL_HASH  , uint64(1)<<12)                     \
    F(PROFILE  , uint64(1)<<13)                          \
    F(WORLEY  , uint64(1)<<14)                           \
    F(BLOCKS  , uint64(1)<<15)                           \
    F(UNDO    , uint64(1)<<16)                           \
    F(SPAWN,    uint64(1)<<17)                           \
    F(GENERATE, uint64(1)<<18)                           \
    F(DESERIALIZE,   uint64(1)<<19)                           \
    F(CROSSFADE, uint64(1)<<20)                               \
    F(GUI,      uint64(1)<<21)                                \
    F(EVENTS,   uint64(1)<<22)                                  \
    F(NO_LOAD_UNLOAD, uint64(1)<<23)                            \
    F(SECTOR_PROFILE, uint64(1)<<24)                            \
    F(MISSION, uint64(1)<<25)                                   \
    F(NOTIFICATION, uint64(1)<<26)                              \
    F(CAMERA, uint64(1)<<27)                                    \
    F(SOUND_EVENTS, uint64(1)<<28)                              \
    F(PERSISTENT, uint64(1)<<29)                                \
    F(SHADER, uint64(1)<<30)                                   \
    F(DEPRECATED, uint64(1)<<31)                               \
    F(CONSOLE, uint64(1)<<32)                                  \
    F(TIMESTAMP, uint64(1)<<33)                                \
    F(GLERROR, uint64(1)<<34)                                  \
    F(GAMESTATE, uint64(1)<<35)                                \
    F(CAUDIO, uint64(1)<<36)                                   \
    F(AGENT, uint64(1)<<37)                                    \
    F(NETWORK, uint64(1)<<38)                                  \
    F(SHIP_LOADER, uint64(1)<<39)                              \
    F(STEAM, uint64(1)<<40)                                    \
    F(SAVE, uint64(1)<<41)                                     \
    F(ROOT, uint64(1)<<42)                                     \
    F(MESSAGE, uint64(1)<<43)                                  \
    F(DEFERRED, uint64(1)<<44)                                 \
    F(NET_VERBOSE, uint64(1)<<45)                              \
    F(MODS, uint64(1)<<46)                                     \
    F(MULTI, uint64(1)<<47)                                    \
    F(STREAMER_PROFILE, uint64(1)<<48)                         \
    F(STARS, uint64(1)<<49)                                    \
    F(SLEEPING, uint64(1)<<50)                                 \
    F(GOG, uint64(1)<<51)                                      \
    F(FILE, uint64(1)<<52)                                     \


#define TO_DBG_ENUM(X, V) DBG_##X=V,
enum DebugRender : uint64 { DBG_TYPES(TO_DBG_ENUM) };
#define DPRINT(TYPE, ARGS)                                              \
    if (globals.debugRender&(DBG_ ## TYPE))                             \
        ::Report("[" #TYPE "] " + str_strip(str_format ARGS))

DEFINE_ENUM(uint64, EDebug, DBG_TYPES);

struct LogRecorder {
    vector<string> lines;

    // void Report(const string &s) { lines.push_back(s); }
    void Report(string s);
    void ReportLines(string s);
    size_t size() const { return lines.size(); }
};

// these guys are actually surprisingly slow
#define ASSERT_MAIN_THREAD() DASSERT(std::this_thread::get_id() == globals.mainThreadId)
#define ASSERT_NOT_MAIN_THREAD() DASSERT(globals.updateThreadId == std::thread::id() || std::this_thread::get_id() != globals.mainThreadId)
#define ASSERT_UPDATE_THREAD() DASSERT(globals.updateThreadId == std::thread::id() || std::this_thread::get_id() == globals.updateThreadId)

#if __GNUC__
#define FORMAT_ARG __attribute__ ((format_arg(1)))
#else
#define FORMAT_ARG
#endif

DLLFACE const char* gettext_(const char* key) FORMAT_ARG;
DLLFACE const char* gettext_(const char* eng, const char* key) FORMAT_ARG;
const char* gettext_(const string &key);
lstring gettext_(lstring key);
#define _(X) gettext_(X)

struct Globals {

    // input
    double    lastLastClickTime[kMouseButtonCount];
    double    lastClickTime[kMouseButtonCount];
    float2    editorCursorPos;

    bool isDoubleClick(int key) const;
    bool isTripleClick(int key) const;

    // time
    double    renderTime;        // real time at start of render thread
    double    updateStartTime;   // real time at start of update thread
    float     simTimeStep;       // desired seconds of sim time per update
    float     updateTimeStep;    // seconds of real time per simulation timestep
    double    updatesPerSecond;
    double    frameTime;         // seconds for the current (last) frame
    double    framesPerSecond;
    uint      swapInterval = -2;
    int       frameStep = 0;         // increment every frame
    int       reloadVersion = 0;

    float2    windowSizePixels;
    float2    viewportSizePixels;
    float2    windowSizePoints;

    ShipLoader*                shipLoader;
    BlockIndex*                blockIndex;

    SaveGame*                  save;

    typedef std::function<void()> Thunk; 

    OL_Thread               updateThread;
    OL_Thread               loaderThread;
    std::thread::id         updateThreadId;
    std::thread::id         mainThreadId;
    std::thread::id         loaderThreadId;
    std::thread::id         streamerThreadId;
    std::recursive_mutex    deferredDeletionsMutex;
    deque<IDeletable*>      deferredDeletions;
    deque<Thunk>            deferredThunks;
    vector<GLuint>          deferredBuffers;
    vector<GLuint>          deferredFramebuffers;
    vector<GLuint>          deferredTextures;
    vector<GLuint>          deferredRenderbuffers;
    std::mutex              loaderMutex;
    int                     loaderWorkCount = 0;
    deque<Thunk>            loaderThunks;
    std::condition_variable loaderWorkAvailable;
    std::condition_variable loaderFinished;

    bool isMainThread() const { return std::this_thread::get_id() == mainThreadId; }
    bool isUpdateThread() const { return std::this_thread::get_id() == updateThreadId; }

    void deleteGLBuffers(int count, GLuint *buffers)
    {
        std::lock_guard<std::recursive_mutex> l(deferredDeletionsMutex);
        deferredBuffers.insert(deferredBuffers.end(), buffers, buffers + count);
    }

    void deleteGLFramebuffers(int count, GLuint *buffers)
    {
        std::lock_guard<std::recursive_mutex> l(deferredDeletionsMutex);
        deferredFramebuffers.insert(deferredFramebuffers.end(), buffers, buffers + count);
    }

    void deleteGLTextures(int count, GLuint *buffers)
    {
        std::lock_guard<std::recursive_mutex> l(deferredDeletionsMutex);
        deferredTextures.insert(deferredTextures.end(), buffers, buffers + count);
    }

    void deleteGLRenderbuffers(int count, GLuint *buffers)
    {
        std::lock_guard<std::recursive_mutex> l(deferredDeletionsMutex);
        deferredRenderbuffers.insert(deferredRenderbuffers.end(), buffers, buffers + count);
    }

    void deleteInMainThread(IDeletable* ptr)
    {
        if (std::this_thread::get_id() == mainThreadId) {
            delete ptr;
        } else if (ptr) {
            ptr->onQueueForDelete();
            std::lock_guard<std::recursive_mutex> l(deferredDeletionsMutex);
            deferredDeletions.push_back(ptr);
        }
    }

    void callFromMainThread(const std::function<void()> &thunk)
    {
        if (std::this_thread::get_id() == mainThreadId) {
            thunk();
        } else {
            std::lock_guard<std::recursive_mutex> l(deferredDeletionsMutex);
            deferredThunks.push_back(thunk);
        }
    }

    void callFromLoaderThread(const std::function<void()> &thunk)
    {
        if (std::this_thread::get_id() == loaderThreadId) {
            thunk();
        } else {
            {
                std::lock_guard<std::mutex> l(loaderMutex);
                loaderThunks.push_back(thunk);
                loaderWorkCount++;
            }
            loaderWorkAvailable.notify_one();
        }
    }

    void waitForLoaderThread()
    {
        std::unique_lock<std::mutex> l(loaderMutex);
        loaderFinished.wait(l);
    }

    void processDeferredFromMainThread();
    void processLoaderThread();

    FrameLogger& getSectorLogger() const;
    FrameLogger& getStreamerLogger() const;

    // UI
    GSList*                    gameState;
    GameZone*                  stepZone;
    const View&                getView() const;
    const ShaderState&         getScreenState();
    EDebug                     debugRender;
    ISoundMgr*                 sound;

    Block*                     getPlayer();
    void                       setPlayer(Block* player);
    bool                       isFromPlayer(const Block *bl) const;
    bool                       isOnPlayer(const Block *bl) const;
    bool                       isPlayerCluster(const BlockCluster *cl) const;
    float2                     getPlayerPos() const;
    Player                     *player;
};

extern Globals globals;

#include "Notification.h"

#endif
