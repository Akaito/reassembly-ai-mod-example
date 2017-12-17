
#ifndef MESSAGE_H
#define MESSAGE_H

#include "Save.h"

DECLARE_ENUM(uint64, EProgress);

struct TutorialData;

struct Messages {

    typedef vector< pair<string, float3> > PopupData;
    typedef unordered_map<string, lstring> Text_t;
    
    unordered_map<lstring, lstring>  messages;
    vector<string>                   tips;
    unordered_map<string, PopupData> popups;
    Text_t                           text;
    vector<TutorialData*>            tutorial;

    void init();
    
    static Messages& instance()
    {
        static Messages m;
        return m;
    }

    static lstring get(lstring key)
    {
        return instance().messages[key];
    }

    static string getTip(int idx)
    {
        if (idx < 0)
            idx = randrange(instance().tips.size());
        return instance().tips[idx];
    }

private:
    Messages() { }
};

void updateBlockLanguage();

#endif
