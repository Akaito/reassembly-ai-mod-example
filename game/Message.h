
#ifndef MESSAGE_H
#define MESSAGE_H

#include "Save.h"

DECLARE_ENUM(uint64, EProgress);

struct TutorialData;

struct Messages {

    typedef vector< pair<string, float3> > PopupData;
    typedef unordered_map<string, lstring> Text_t;
    
    unordered_map<string, string>    messages;
    vector<string>                   tips;
    unordered_map<string, PopupData> popups;
    Text_t                           text;
    vector<TutorialData*>            tutorial;

    void init();
    
    static Messages& instance();

    static string get(const string &key)
    {
        const Messages &msg = instance();
        auto it = msg.messages.find(key);
        return (it != msg.messages.end()) ? it->second : key;
    }

    static string getTip(int idx)
    {
        if (idx < 0)
            idx = randrange(instance().tips.size());
        return instance().tips[idx];
    }

private:
    Messages();
    ~Messages();
};

void updateBlockLanguage();

#endif
