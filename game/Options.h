
#ifndef OPTIONS_H
#define OPTIONS_H

#include "Save.h"

#define USER_OPTIONS(F)                         \
    F(float, musicVolume, 1.f)                  \
    F(float, sfxVolume, 1.f)                    \
    F(int,   particleLevel, 1)                  \
    F(int,   backgroundLevel, 2)                \
    F(int,   blurLevel, 2)                      \
    F(int,   bloom, 2)                          \
    F(int,   fullscreen, 0)                     \
    F(int,   vsync, 3)                          \
    F(int,   lineQuality, 2)                    \
    F(int,   physicsFps, 60)                    \
    F(int,   gamepadEnabled, 1)                 \
    F(int,   gamepadMouseStyle, 0)              \
    F(float, gamepadDeadzone, 0.2f)             \
    F(float, gamepadDamping, 0.5f)              \
    F(int,   version, 0)                        \
    F(string, language, string())               \

    

#define OBSOLETE_USER_OPTIONS(F)                \
    F(int, shake, 0)                            \


struct UserOptions : public Serializable<UserOptions> {

    enum OpVsync {
        CAP60=0,
        UNLIMITED=1,
        VSYNC=2,
        AD_VSYNC=3
    };

    DECLARE_SERIAL_STRUCT_CONTENTS(UserOptions, USER_OPTIONS, OBSOLETE_USER_OPTIONS);

    bool   supportsBloom = false;
    float2 aliasedLineWidthRange;
    float2 smoothLineWidthRange;
    float  smoothLineWidthGranularity = 0.f;
    bool  readFromFile = false;

    // 2 letter lang codes (e.g. en)
    std::set<string> supportedLanguages;
    string           currentLanguage;

    float getTimeStep() const
    {
        return 1.f / clamp((float) physicsFps, 15.f, 160.f);
    }
};


#endif
