
#ifndef NETWORK_H
#define NETWORK_H

const char* getVersionStr();

struct Network {

private:
    enum Operation {
        NONE,
        UPLOAD_AGENT,
        DOWNLOAD_AGENTS,
        DOWNLOAD_FRIENDS,
        SESSION_HELLO,
        LOGIN,
    };
    
public:
    typedef void (*Callback_t)(const char* output, void *data);
    int xfer_callback(size_t dltotal, size_t dlnow, size_t ultotal, size_t ulnow);

private:
    
    bool                            finished = false;
    bool                            started = false;
    std::thread                     thread;
    std::condition_variable         waitForWork;
    mutable std::condition_variable waitOutput;
    mutable std::mutex              mutex;
    string                          input;
    int                             input_idx = 0;
    string                          output;
    int                             output_idx = -1;
    int                             count = 0;
    int                             points = 0;
    float                           progress = 0.f;
    int                             downloaded = 0;
    int                             uploaded = 0;
    Operation                       operation = NONE;
    string                          auth_user;     // username
    string                          auth_password; // plaintext password
    string                          auth_session;  // encrypted session key
    bool                            logged_in = false;
    string                          errmsg;
    mutable bool                    waiting = false;
    Callback_t                      callback = NULL;
    void*                           callback_data = NULL;

    bool                            use_steam_auth = false;
    int                             auth_state = 0;
    bool                            use_gog_auth = false;

    Network();
    ~Network();
    void networkThreadMain();

    static size_t read_callback(char *buffer, size_t size, size_t nitems, void *instream);
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
    static int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

    static const char* getOperationStr(Operation op);

    bool begin(Operation op);
    void waitForAuth();

    STEAM_CALLBACK_NAMED(Network, GetAuthSessionTicketResponse);

public:

    void setCallback(Callback_t cb, void *data);
    void setProgress(float pgs) { progress = pgs; }
    bool isFinished() const { return finished; }
    bool isLoggedIn() const { return logged_in; }
    string getUsername() const;
    string getError() const;

    bool isBusy() const { return operation != NONE; }
    bool isWaiting() const { return waiting; }
    float getProgress() const { return progress; }
    bool isSteamAuth() const { return use_steam_auth || use_gog_auth; }

    void setAuthReady(int res);
    
    const char* getOutput() const; // return immediately, NULL if not ready
    bool waitForOperation() const; // return true on success

    bool startup();
    void shutdown();

    // operations
    bool uploadSaveGame(const string& data);
    bool downloadAgents(int count, int points);
    bool downloadFriends();
    bool sessionHello();
    bool login(const string& username, const string& password);

    static Network &instance()
    {
        static Network *n = new Network();
        return *n;
    }
};


#endif // NETWORK_H
