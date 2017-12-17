
//
// Notification.h - centralized notification registration/dispatch system
// 

// Copyright (c) 2014-2016 Arthur Danskin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "SerialCore.h"

struct Block;
struct MapObjective;

#define NOTIFICATION_TYPE(F)                    \
    F(GAIN_LC, 1<<0)                            \
    F(GAIN_LEVEL, 1<<1)                         \
    F(PLAYER_BLOCK_DEATH, 1<<3)                   \
    F(OPTION_CHANGED, 1<<4)                       \
    F(ACTIVATE_OBJECTIVE, 1<<5)                      \
    F(CVAR_CHANGED, 1<<6)                         \
    F(WORMHOLE_OPEN, 1<<7)                        \
    F(GAIN_RESOURCE, 1<<8)                        \
    F(PLAYER_DIED, 1<<9)                          \
    F(SECTOR_VISITED, 1<<11)                      \
    F(TRANSFER_RU, 1<<12)                         \
    F(PLAYER_KILLED, 1<<13)                       \
    F(MESSAGE, 1<<14)                             \
    F(PROGRESS, 1<<15)                            \
    F(CONQUERED_SECTOR, 1<<16)                    \
    F(DEBUG_RELOAD, 1<<17)                        \
    F(KEY_CHANGED, 1<<18)                         \
    F(TARGET_DESTROYED, 1<<19)                    \
    F(ENTER_WORMHOLE, 1<<20)                      \
    F(LIBERATED_STATION, 1<<21)                   \
    F(UPGRADE_P, 1<<22)                           \
    F(DECRYPT_BLOCK, 1<<23)                       \
    

DEFINE_ENUM(uint64, ENotification, NOTIFICATION_TYPE);

#define NOTIFY(TYPE, ...) Notifier::instance().notify(Notification(ENotification:: TYPE, ## __VA_ARGS__))

struct Notification {

    enum Type : ENotification::value_type { NOTIFICATION_TYPE(SERIAL_TO_ENUM) };
    
    ENotification  type;
    MapObjective  *objective = NULL;
    Block         *block     = NULL;
    double         value     = 0.f;
    uint64         bits      = 0;
    float2         pos;
    string         message;

    typedef int VisitEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(type) &&
            vis.VISIT(objective) &&
            vis.VISIT(block) &&
            vis.VISIT(value) &&
            vis.VISIT(pos) &&
            vis.VISIT(message);
    }

    Notification(ENotification t) : type(t) {}
    Notification(ENotification t, MapObjective *mo) : type(t), objective(mo) {}
    Notification(ENotification t, Block *bl) : type(t), block(bl) {}
    Notification(ENotification t, double v) : type(t), value(v) {}
    Notification(ENotification t, uint64 b) : type(t), bits(b) {}
    Notification(ENotification t, int v) : type(t), value(v) {}
    Notification(ENotification t, float2 p) : type(t), pos(p) {}
    Notification(ENotification t, float2 p, double v) : type(t), value(v), pos(p) {}

    Notification(ENotification t, const char* msg, ...) __printflike(3, 4);

    string toString() const;

};

struct INotifyHandler;

struct Notifier {

private:
    Notifier() : m_handlers(ENotification::getBitCount()) { }

    vector< vector<INotifyHandler*> > m_handlers;

public:
    
    void registerHandler(uint64 types, INotifyHandler* handler);
    void notify(const Notification& notif);

    static Notifier& instance()
    {
        static Notifier v;
        return v;
    }    
};

struct INotifyHandler {

    virtual void OnNotify(const Notification& notif) = 0;
    void registerTypes(uint64 types) { Notifier::instance().registerHandler(types, this); }
    virtual ~INotifyHandler() { registerTypes(0); }
};



// progress loading bar helper
// load is broken up into N steps, each with a float for progress
// check progress from another thread
struct LoadProgress {
    
    const char* m_name     = NULL;
    int         m_nsteps   = 1;
    int         m_step     = -1;
    float       m_progress = 0;
    double      m_time     = -1.0;
    double      m_startTime = -1.0;

    void start(const char* nm, int steps);
    void step(int st);
    void next() { step(m_step + 1); }
    void finish() { step(m_nsteps); }
    float getPercent() const { return 100.f * (m_step + m_progress) / m_nsteps; }
};


#endif // NOTIFICATION_H
