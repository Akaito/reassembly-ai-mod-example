
#ifndef KEYS_H
#define KEYS_H

#include "Save.h"

inline bool isMouseKey(int i)
{
    return (LeftMouseButton <= i && i <= MouseButtonSix);
}

struct KeyBinding {

    enum Group {
        BASIC=0, FLYING, EDITOR, G_COUNT, GAMEPAD, OBSOLETE,
    };

    enum { NUM_KEYS=4 };

    static const char* getGroupName(Group gp);

    int            keys[NUM_KEYS];
    lstring        symbol;
    const char*    descr = NULL;
    const Group    group = BASIC;
    int            def[NUM_KEYS];
    mutable string tmp;
    
    KeyBinding();
    KeyBinding(lstring sym, const char* v, Group gp, int d0, int d1);
    KeyBinding(lstring sym, const char* v, Group gp, int d0, int d1, int d2, int d3);
    
    void setDefault()
    {
        for (int i=0; i<NUM_KEYS; i++)
            keys[i] = def[i];
    }

    KeyBinding getDefault()
    {
        return KeyBinding(symbol, descr, group, def[0], def[1], def[2], def[3]);
    }
    
    typedef int VisitEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        return vis.visit("key0", keys[0], def[0]) &&
            vis.visit("key1", keys[1], def[1]) &&
            vis.visit("key2", keys[2], def[2]) &&
            vis.visit("key3", keys[3], def[3]);
    }
    
    bool operator==(const KeyBinding& o) const
    {
        if (o.symbol != symbol)
            return false;
        for (int i=0; i<NUM_KEYS; i++)
            if (keys[i] != o.keys[i])
                return false;
        return true;
    }
    
    bool isEnabled() const
    {
        const KeyState &ks = KeyState::instance();
        for (int i=0; i<NUM_KEYS; i++)
            if (keys[i] && ks[keys[i]])
                return true;
        return false;
    }

    // like isEnabled but ignore mouse buttons
    bool isKeyEnabled(bool enableMouse=false) const
    {
        const KeyState &ks = KeyState::instance();
        for (int i=0; i<NUM_KEYS; i++)
        {
            if (keys[i] && (enableMouse || !isMouseKey(keys[i])) && ks[keys[i]])
                return true;
        }
        return false;
    }
    
    bool isDownEvent(const Event* evt) const
    {
        return isKey(KeyState::instance().getDownKey(evt));
    }

    bool isKey(int key) const
    {
        if (!key)
            return false;
        for (int i=0; i<NUM_KEYS; i++)
            if (key == keys[i])
                return true;
        return false;
    }
    
    bool isUpEvent(const Event* evt) const
    {
        return isKey(KeyState::instance().getUpKey(evt));
    }
    
    string getKeyStr() const;
    const char* c_str() const;
};

typedef KeyBinding::Group KeyGroup;

#define IDENT_(X) X

#define DEF_BINDING(NAME, DESC, GROUP, KEY0, KEY1) \
    Binding NAME = Binding(#NAME, IDENT ## DESC, Binding::GROUP, KEY0, KEY1)

#define DEF_BINDING4(NAME, DESC, GROUP, KEY0, KEY1, KEY2, KEY3)     \
    Binding NAME = Binding(#NAME, IDENT ## DESC, Binding::GROUP, KEY0, KEY1, KEY2, KEY3)

#define DEF_ED_BINDING(NAME, DESC, KEY)                                  \
    Binding NAME = Binding(#NAME, IDENT ## DESC, Binding::EDITOR, KEY, MOD_CTRL|KEY)

#define DEF_ED_BINDING2(NAME, DESC, KEY, KEY1)                               \
    Binding NAME = Binding(#NAME, IDENT ## DESC, Binding::EDITOR, KEY, KEY1, MOD_CTRL|KEY, 0)


inline float2 axisToShip(float2 va)
{
    // 1, 1 is towards the person and to the right
    return float2(-va.y, -va.x);
}

inline float2 axisToUp(float2 va)
{
    return float2(va.x, -va.y);
}

struct KeyBindings : public Serializable<KeyBindings> {

    typedef KeyBinding Binding;

    // WARNING keep this as the first binding!
    // WARNING only bindings from here to the last binding
    DEF_BINDING(forward, _("Forwards"), BASIC, 'w', NSUpArrowFunctionKey);
    DEF_BINDING(back, _("Backwards"), BASIC, 's', NSDownArrowFunctionKey);
    DEF_BINDING(left, _("Left"), BASIC, 'a', NSLeftArrowFunctionKey);
    DEF_BINDING(right, _("Right"), BASIC, 'd', NSRightArrowFunctionKey);

    DEF_BINDING(fireMain, _("Fire Primary"), FLYING, LeftMouseButton, GamepadTriggerRight);
    DEF_BINDING(fireAlt, _("Fire Secondary"), FLYING, RightMouseButton, GamepadTriggerLeft);
    DEF_BINDING(fireThird, _("Fire Tertiary"), FLYING, MiddleMouseButton, GamepadRightShoulder); 
    DEF_BINDING(stop, _("Stop"), FLYING, 'x', GamepadLeftStick);
    DEF_BINDING(sleft, _("Strafe Left"), FLYING, 'q', 0);
    DEF_BINDING(sright, _("Strafe Right"), FLYING, 'e', 0);
    DEF_BINDING(target, _("Lock Target"), FLYING, ' ', GamepadRightStick);
    DEF_BINDING(activate, _("Spawn Child"), FLYING, 'f', GamepadA);
    DEF_BINDING(transferRes, _("Transfer Resources"), FLYING, 'c', 0);
    DEF_BINDING(utility0, _("Utility Primary"), FLYING, OShiftKey, GamepadLeftShoulder);
    DEF_BINDING(utility1, _("Utility Secondary"), FLYING, 'z', 0);
    DEF_BINDING(utility2, _("Utility Tertiary"), FLYING, '4', 0);
    
    DEF_BINDING(teleport, _("Teleport"), FLYING, 'z', 0);
    DEF_BINDING(destruct, _("Self Destruct"), FLYING, 'j', 0);
    DEF_BINDING(toggleTractor, _("Toggle Command mode"), FLYING, '\t', GamepadX);
    DEF_BINDING(viewMap, _("View Map"), FLYING, 'm', GamepadBack);
    DEF_BINDING(viewBindings, _("View Weapon Bindings"), FLYING, 'b', 0);
    DEF_BINDING(viewUpgrades, _("View Upgrades"), FLYING, 'u', 0);
    DEF_BINDING(mouseRotToggle, _("Cycle controls"), FLYING, 'r', 0);
    DEF_BINDING(autoZoomToggle, _("Toggle auto-zoom"), FLYING, 'y', 0);
    DEF_BINDING(autoAimToggle, _("Toggle auto-aim"), FLYING, 't', 0);
    DEF_BINDING(toggleHUD, _("Toggle minimal HUD"), FLYING, 'v', 0);
    DEF_BINDING(cycleFleet, _("Cycle Fleet AI"), FLYING, 'g', 0);
    DEF_BINDING(zoomOut, _("Zoom Out"), BASIC, '-', GamepadDPadDown);
    DEF_BINDING(zoomIn, _("Zoom in"), BASIC, '=', GamepadDPadUp);
    DEF_BINDING4(viewSelector, _("View Ships"), FLYING, '3', GamepadY, 'i', 0);
    DEF_BINDING(viewConstructor, _("Edit Current Ship"), FLYING, '1', GamepadB);
    DEF_BINDING(viewConstructor2, _("Edit Child Ship"), OBSOLETE, '2', 0);
    DEF_BINDING(rcamLeft, _("Rotate Camera Counterclockwise"), FLYING, ';', 0);
    DEF_BINDING(rcamRight, _("Rotate Camera Clockwise"), FLYING, '\'', 0);

    DEF_BINDING(next, _("Next"), BASIC, ' ', GamepadA);
    DEF_BINDING(skip, _("Skip"), BASIC, '\r', GamepadX);
    DEF_BINDING(console, _("Console"), BASIC, '\\', '~');
    DEF_BINDING(faster, _("Faster"), BASIC, ']', 0);
    DEF_BINDING(slower, _("Slower"), BASIC, '[', 0);
    DEF_BINDING(pause, _("Pause"), BASIC, 'p', 0);

    DEF_BINDING(gpLeftClick, _("Left Click"), GAMEPAD, GamepadTriggerRight, 0);
    DEF_BINDING(gpRightClick, _("Right Click"), GAMEPAD, GamepadTriggerLeft, 0);

    DEF_ED_BINDING2(undo, _("Undo"), 'z', GamepadB);
    DEF_ED_BINDING2(redo, _("Redo"), 'y', 0);
    DEF_ED_BINDING2(mirror, _("Mirror"), 'm', GamepadY);
    DEF_ED_BINDING(copy, _("Copy"), 'c');
    DEF_ED_BINDING(cut, _("Cut"), 'x');
    DEF_ED_BINDING(paste, _("Paste"), 'v');
    DEF_BINDING4(scaleUp, _("Scale Up"), EDITOR, ',', '<', GamepadX, 0);
    DEF_BINDING(scaleDown, _("Scale Down"), EDITOR, '.', '>');
    DEF_BINDING(selectAll, _("Select All"), EDITOR, MOD_CTRL|'a', 0);
    DEF_BINDING(rotateLeft, _("Rotate Counterclockwise"), EDITOR, 'q', GamepadDPadLeft);
    DEF_BINDING(rotateRight, _("Rotate Clockwise"), EDITOR, 'e', GamepadDPadRight);
    DEF_BINDING4(destroy, _("Delete"), EDITOR, NSBackspaceCharacter, GamepadA, NSDeleteFunctionKey, 0);
    // WARNING keep this as the last binding!

    const Binding &getFireKey(int idx) const;
    static const char* getFireDescription(int idx);
    const string getFireName(int idx) const;

    enum Constants {
        FireKeyCount = 3
    };
    
    typedef int VisitEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        for (Binding *ptr=&forward; ptr <= &destroy; ptr++) {
            if (!vis.visit(ptr->symbol.c_str(), *ptr, ptr->getDefault()))
                return false;
        }
        return vis.VISIT_SKIP(float, "scroll");
    }

    void setDefaults(KeyGroup gp);

    KeyBindings();

    float2 getTranslation(bool enableMouse=false) const;
    float2 getTranslationWASD(bool enableMouse=false) const;
    float2 getTranslationVertical() const;
    int2 getMenuTranslation(const Event *event) const;
    float getZoom() const;
    float getRotate() const;
    float getRotateCamera() const;

    // replace $keyName with string representing which key
    string interpolateKeyNames(const string &str, bool color=true) const;
    string getKeyMessage(const char* key, bool color=true) const;
};


#endif  // KEYS_H
