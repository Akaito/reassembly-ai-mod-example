
#ifndef PARTICLE_EFFECTS_H
#define PARTICLE_EFFECTS_H

#include "Particles.h"

extern uint kParticleSmokeColor;
extern float kParticleSmokeAlpha;

struct EffectsParticleSystem final : public ParticleSystem, public INotifyHandler {
private:
    GameZone *m_zone = NULL;
    
public:
    
    EffectsParticleSystem(GameZone *zone);
    
    void OnNotify(const Notification& notif);
    
    // 1 particle effects
    void spark(float2 pos, float2 vel, float size, uint color, float time=0.5f);
    void explodeFade(float2 pos, float2 vel, float randv, float size, float time=0.f, bool gradient=true);
    void muzzleFlash(float2 pos, float2 vel, float size, uint color, float time)
    {
        muzzleFlash(float3(pos, 2.52f), vel, size, color, time);
    }
    void muzzleFlash(float3 pos, float2 vel, float size, uint color, float time);
    void thrust(float2 pos, float2 vel, float angle, float size, uint color);
    void particle(float3 pos, float2 vel, float size, float time, uint color, float alpha);

    void magicSpark(float2 pos);

    // multi particle effects

    void magicCover(float2 pos, float rad, uint color);
    void magicFlash(float2 pos, float rad);
    void levelUpFlash(float2 pos);

    // trails
    void smokeExplodeTrail(float2 pos, float rad);
    void xpTrail(float2 pos, float rad, uint color0, uint color1);

};


#endif // PARTICLE_EFFECTS_H
