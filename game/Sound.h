//
//  Sound.h
//  Outlaws
//
//  Created by Arthur Danskin on 4/24/13.
//  Copyright (c) 2013-2016 Arthur Danskin All rights reserved.
//

#ifndef __Outlaws__Sound__
#define __Outlaws__Sound__

#include "Types.h"
struct SoundEvent;
struct OneMod;

#define AUDIO_FLAGS(F)                                \
    F(STREAM,      1<<0)                              \
    F(LOOP,        1<<1)                              \
    F(ROUND_ROBIN, 1<<2)                              \
    F(MUSIC,       1<<3)                              \
    F(CROSSSYNC,   1<<4)                              \
    F(CLUSTER,     1<<5)                              \
    F(CULL_VOL,    1<<6)                              \
    F(CULL3D_VOL,  1<<7)                              \
    

DEFINE_ENUM(uint, EAudioFlags, AUDIO_FLAGS);

// loaded per-event type data
struct EventDescription {

    enum : EAudioFlags::value_type { AUDIO_FLAGS(SERIAL_TO_ENUM) };

    // read from audio.lua
    vector< lstring >  samples;
    float          volume         = 1.0;
    float          pitch          = 1.0;
    float          pitchRandomize = 0.f;
    EAudioFlags    flags;
    float          rolloff  = 1.f;
    float          minDist  = 1.f;
    float          maxDist  = 9999999999.f;
    int            priority = 0;
    float2         delay;

    lstring        name;
    const OneMod*  mod = NULL;

    mutable uint   m_index = 0;
    
    void advance(int delta) const
    {
        if (samples.empty())
            return;
        const uint tracks = samples.size();
        if (tracks <= 1) {
            m_index = 0;
        } else if (flags&EventDescription::ROUND_ROBIN) {
            m_index = (m_index + delta) % tracks;
        } else {
            const uint last = m_index;
            do {
                m_index = randrange(0, tracks);
            } while (m_index == last);
        }
    }

    static EventDescription &getDefault()
    {
        static EventDescription def(false);
        return def;
    }

    EventDescription(bool usedef = true)
    {
        if (usedef)
            *this = getDefault();
    }

    uint getSampleCount() const
    {
        return samples.size();
    }
    
    typedef int VisitEnabled;

    template <typename V>
    bool accept(V& vis)
    {
        return (vis.VISIT(samples) &&
                vis.VISIT(volume) &&
                vis.VISIT(pitch) &&
                vis.VISIT(pitchRandomize) &&
                vis.VISIT(rolloff) &&
                vis.VISIT(minDist) &&
                vis.VISIT(maxDist) &&
                vis.VISIT(flags) &&
                vis.VISIT(delay) &&
                vis.VISIT(priority));
    }
};

struct ISoundMgr {
  
    virtual void Update(float timeFactor) {}
    virtual void Reload()  {}
    
    // world events
    virtual void OnCannonFire(const Block* cannon, float size) {}
    virtual void OnLaserFire(const Block* laser, float gain) {}
    virtual void OnLaunch(const Block* missile, const Block *launcher) {}
    virtual void OnExplosion(const Explosion* exp) {}

    // tracking for attached sounds
    virtual void RemoveBlockSound(const Block *block) {}

    // UI events
    virtual void OnPlayerTarget(const Block* target) {}
    virtual void OnButtonPress() {}
    virtual void OnButtonHover() {}

    virtual void OnThrust(float vol, float total, int faction){}

    virtual void OnEvent(const char* name)  {}
    virtual void OnBlockEvent(const char* name, const Block *bl)  {}
    virtual void OnBlockDestroyed(const Block *bl)  {}

    virtual void OnPlayMusic(lstring name, float volume, lstring aname, float avolume)  {}
    virtual void OnFadeMusic(float crossfade, float vol)  {}
    virtual void OnAdvanceMusic(int delta) {}
    virtual void OnSetZone(GameZone* zone, float crossfade) {}

    virtual const EventDescription *getEvent(const char* name) { return NULL; }
    virtual uint getUsedSources() { return 0; }
    virtual uint getTotalSources() { return 0; }

    virtual float *GetLoadProgress() { return NULL; }
    virtual void ShutDown()  {}

    virtual ~ISoundMgr() {}
};

ISoundMgr* CreateCAudioSoundMgr();
ISoundMgr* CreateDummySoundMgr();

inline ISoundMgr* CreateDummySoundMgr() { return new ISoundMgr(); }

#endif /* defined(__Outlaws__Sound__) */
