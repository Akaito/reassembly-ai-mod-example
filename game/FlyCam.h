

#ifndef FLYCAM_H
#define FLYCAM_H

static const float kViewScaleMoveTime   = 0.1f;
extern float kCameraSpeed;
extern float kCameraZSpeed;
extern float kCameraRotationSpeed;

extern float kCameraWheelZSpeed;

#if !BUILDER
extern bool kStandaloneBuilder;
extern bool kEnableDevBindings;
#else
#define kStandaloneBuilder (1)
#define kEnableDevBindings (0)
#endif

struct StationInterface;

struct FlyCam {

    float  m_targetWorldRadius = 1440.f;
    float2 m_offsetFromPlayer;
    bool   m_lockedToPlayer = false;
    float  m_viewRadiusMin;           // set in constructor
    float  m_viewRadiusMax;           // set in constructor

    float m_autoZoomWorldRadius = 0.f;

    std::mutex m_debugMutex;
    string     m_debug;

    FlyCam();
    
    void OnNotify(const Notification &notif);

    void autoZoom(const StationInterface *station);
    void smoothZoom(View& view, bool sizeChanged) const;
    void panTowardsCursor(const GameZone* zone, View& view, bool sizeChanged, bool enablePan=true);

    void adjustZoom(float x=-1.f);
    void panZoom(View &view);
    
};


#endif // FLYCAM_H
