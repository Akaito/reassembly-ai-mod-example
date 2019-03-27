
#ifndef SAVE_H
#define SAVE_H

#include "ZipFile.h"

struct SerialCluster;
struct SerialBlock;
struct Level;
struct SaveSerializer;
struct SaveParser;

extern bool kWriteJSON;

// basic io routines - abstract steam, save path, etc.

// write a save file - possibly to steam cloud
bool SaveFile(const char* fname, const char* data, int size);
inline bool SaveFile(const string &fname, const string &data) {
    return SaveFile(fname.c_str(), &data[0], data.size());
}

// write a compressed save file to FNAME.gz, possibly to steam cloud
bool SaveCompressedFile(const char* fname, const char* data, int size);
inline bool SaveCompressedFile(const string &fname, const string &data) {
    return SaveCompressedFile(fname.c_str(), &data[0], data.size());
}

// load a saved file, possibly from steam cloud
string LoadFile(const char* fname);
inline string LoadFile(const string &str) { return LoadFile(str.c_str()); }

// load file without decompression
string LoadFileRaw(const char* fname);
inline string LoadFileRaw(const string &str) { return LoadFileRaw(str.c_str()); }

// recursively delete stuff
int RemoveFileOrDirectory(const char* path);
bool RemoveFile(const char *path);
int FileForget(const char *path);

// is there a file?
bool FileExists(const char* fname);
inline bool FileExists(const string &fname) { return FileExists(fname.c_str()); }

// return list of files in directory
vector<string> ListDirectory(const char* fname, bool local_only=false);
inline vector<string> ListDirectory(const string &fname, bool local_only=false) { return ListDirectory(fname.c_str(), local_only); }

// __DATE__
const char *getBuildDate();

void ReportEarly(const string &x);
void ReportEarlyf(const char* fmt, ...) __printflike(1, 2);
void ReportFlush();

#define PARSE_FAIL(F, ...) return fail1(__func__, __LINE__, 0, F, ##__VA_ARGS__)
#define PARSE_FAIL_UNLESS(X, F, ...) if (!(X)) PARSE_FAIL(F, ##__VA_ARGS__)

#define PARSE_FAIL2(F, ...) return fail1(__func__, __LINE__, 1, F, ##__VA_ARGS__)
#define PARSE_FAIL_UNLESS2(X, F, ...) if (!(X)) PARSE_FAIL2(F, ##__VA_ARGS__)

// prevent things with implicit bool conversions from matching
struct ReallyBool {
    bool val;
    explicit ReallyBool(bool v) : val(v) {}
};


// type traits

// the first # of elements are serialized as indexed, the rest as name=val
template <typename T>
struct GetVisitIndexCount {
    static const int value = INT_MAX;
};

// define a function that is called before object parses
// can change parser state, or something
template <typename T>
struct ParsePrepare {
    bool prepare(const SaveParser &sp, T &obj) { return true; }
};

// define a function that is called after the object finishes parsing
// the function can generate contextualized error messages
template <typename T>
struct ParseValidate {
    bool validate(const SaveParser &sp, T &obj) { return true; }
};


struct SaveSerializer {

    string         o;

    enum Flags : ushort {
        EXPLICIT_FIELDS=1<<1,
        SUMMARIZE_PRETTY=1<<2,
        BINARY=1<<3,
        FORMAT_NONE=1<<4,
        FORMAT_DISPLAY=1<<5,
        EXPLICIT_CLUSTERS_NEVER=1<<6,
        EXPLICIT_CLUSTERS_ALWAYS=1<<7,
        INDEX_CLUSTERS=1<<8,
        IMPLICIT_TYPE_FIELDS=1<<9,
        JSON=1<<10,
    };

protected:
    int    indent               = 0;
    int    lastnewline          = 0;
    int    lastnewlineindentend = -1;
    ushort flags                = 0;
    int    columnWidth          = 80;

    std::unordered_set<uint> indexedClusters;
    vector<lstring>          currentClusterName;

public:
    std::unordered_set<string>   *skipFields = NULL;
    int                           maxIndent = 9999999;

    void clear()
    {
        o.clear();
        indent = 0;
        lastnewline = 0;
        lastnewlineindentend = -1;
        flags = 0;
        columnWidth = 80;
        indexedClusters.clear();
        currentClusterName.clear();
    }

    SaveSerializer() {}
    SaveSerializer(SaveSerializer &&ss) :
        o(std::move(ss.o)),
        lastnewline(ss.lastnewline),
        lastnewlineindentend(ss.lastnewlineindentend),
        flags(ss.flags),
        columnWidth(ss.columnWidth),
        indexedClusters(std::move(ss.indexedClusters)) {}

    static SaveSerializer& instance();
    
    // options
    SaveSerializer& setFlag(Flags flag, bool val=true)
    {
        setBits(flags, (ushort)flag, val);
        if (flags&FORMAT_DISPLAY)
            indent = -1;
        return *this;
    }

    bool getFlag(Flags flag) const { return (flags&flag); }
    
    SaveSerializer& setColumnWidth(int v)
    {
        if (v < 0)
            setFlag(FORMAT_NONE, true);
        else
            columnWidth = v;
        return *this;
    }    

    template <typename T>
    static string toString(const T& v)
    {
        SaveSerializer s;
        s.serialize(v);
        return s.str();
    }

    ~SaveSerializer(){}

    const string& str() const { return o; }
    const char* c_str() const { return o.c_str(); }
    bool        empty() const { return o.empty(); }
    int         size() const { return o.size(); }

    float prepare(float f) const;

    template <typename T>
    void serialize(const copy_ptr<T>& ptr) { serialize(ptr.get()); }

    void serialize(const Block* bl);
    void serialize(const SerialBlock& b) { serialize(&b); }
    void serialize(const SerialBlock* b);

    template <typename T>
    SaveSerializer &serialize(const SerialEnum<T> &enm)
    {
        serializeE(enm.value, T::getType());
        return *this;
    }
        
    void serializeEAll(const EnumType &e);

    template <typename T>
    bool serializeFieldDef(const char* name, const T& val, const T& def=T(), char delimit=',')
    {
        if (val == def)
            return false;
        if (skipFields && skipFields->count(name))
            return false;
        serializeField(name, val, delimit);
        return true;
    }

    template <typename T>
    void serializeField(const char* name, const T& val, char delimit=',')
    {
        insertName(name);
        insertToken('=');
        serialize(val);
        insertToken(delimit);
    }

    template <typename K, typename V>
    void serialize(const std::pair<K, V> &v)
    {
        insertToken('{');
        serialize(v.first);
        insertToken(',');
        serialize(v.second);
        insertToken('}');
    }

    void insertToken(char token);
    void insertTokens(const char* tksn);
    void insertName(const char* name);

protected:

    void serializeE(uint64 v, const EnumType &e);

    void indent1()
    {
        if ((flags&FORMAT_NONE) || indent <= 0)
            return;
        for (int i=0; i<(indent * 2); i++)
            o += ' ';
    }

    void chompForToken(int token);

    template <typename T>
    void serializeList(const T &v)
    {
        const char* parens = (flags&JSON) ? "[]" : "{}";
        insertToken(parens[0]);
        bool first = true;
        bool newline = false;
        foreach (const auto &x, v)
        {
            // can parse vectors of structs without commas
            if (!first)
                insertToken(',');
            if (newline)
                insertToken('\n');

            first = false;

            const int start = o.size();
            serialize(x);
            newline = !(flags&FORMAT_NONE) && columnWidth > 0 && (o.size() - start) > columnWidth/3;
        }
        insertToken(parens[1]);
    }

public:

    template <typename T> void serialize(const vector<T> &v)   { serializeList(v); }
    template <typename T> void serialize(const deque<T> &v)    { serializeList(v); }
    template <typename T> void serialize(const std::set<T> &v) { serializeList(v); }
    template <typename T, size_t S> void serialize(const std::array<T, S> &v) { serializeList(v); }


    static bool isIdent(const char* s);
    void serializeKey(const string& s) { serializeKey(s.c_str()); }
    void serializeKey(const char* k)   { if (isIdent(k) && !(flags&JSON)) { o += k; } else { serialize(k); } }
    template <typename T> void serializeKey(const T& k) { serialize(k); }

    void comment(const string &s)
    {
        if (flags&JSON)
            return;
        o += "-- ";
        o += s;
        o += '\n';
    }
    
    template <typename K, typename V>
    void serialize(const std::map<K, V> &v)
    {
        insertToken('{');
        for (typename std::map<K, V>::const_iterator it=v.begin(), end=v.end(); it != end; ++it) {
            if (it != v.begin())
                insertToken(',');
            
            serializeKey(it->first);
            o += "=";
            serialize(it->second);
        }
        insertToken('}');
    }
    
    template <typename K, typename V>
    void serialize(const std::unordered_map<K, V> &v)
    {
        const std::map<K, V> m(v.begin(), v.end());
        serialize(m);
    }

    void serialize(float f);

    template <typename T, int S, fixed_scalar_t I>
    void serialize(FixedPt<T, S, I> v) { serialize((float) v); }

    // faster than insertToken(',') and never inserts newline
    void insertComma()
    {
        o += (flags&FORMAT_NONE) ? "," : ", ";
    }

    template <typename T>
    void serialize(const glm::tvec2<T> &v)
    {
        const char* parens = (flags&JSON) ? "[]" : "{}";
        o += parens[0]; serialize(v.x); insertComma(); serialize(v.y); o += parens[1];
    }

    template <typename T>
    void serialize(const glm::tvec3<T> &v)
    {
        const char* parens = (flags&JSON) ? "[]" : "{}";
        o += parens[0]; serialize(v.x); insertComma(); serialize(v.y); insertComma(); serialize(v.z); o += parens[1];
    }

    template <typename T>
    void serialize(const glm::tvec4<T> &v)
    {
        const char* parens = (flags&JSON) ? "[]" : "{}";
        o += parens[0]; serialize(v.x); insertComma(); serialize(v.y); insertComma();
                  serialize(v.z); insertComma(); serialize(v.w); o += parens[1];
    }

    void serialize(const char* s);
    void serialize(const string& s) { serialize(s.c_str()); }
    void serialize(lstring st)      { serialize(st.c_str()); }
    void serialize(double f) { str_append_format(o, "%.12g", f); }
    void serialize(uint i);
    void serialize(uint64 i);
    void serialize(int i)    { str_append_format(o, "%d", i); }
    void serialize(char i)   { str_append_format(o, "%d", (int)i); }
    void serialize(uchar i)  { str_append_format(o, "%d", (int)i); }
    void serialize(short i)  { str_append_format(o, "%d", (int)i); }
    void serialize(ushort i) { str_append_format(o, "%d", (int)i); }
    void serialize(ReallyBool v)   { o += (v.val ? "1" : "0"); }

    void serialize(const SerialCluster* sc);
    void serialize(const BlockCluster* bc, float2 offset);
    void serialize(const BlockCluster* bc) { serialize(bc, float2(0.f)); }

    template <typename T>
    bool visitSkip(const char *name)
    {
        return true; // keep going
    }

    int visitIndex = -1;
    int visitIndexCount = -1;

    template <typename T>
    bool visit(const char* name, const T &val, const T& def=T())
    {
        if (0 <= visitIndex && visitIndex < visitIndexCount) {
            visitIndex++;
            if (visitIndex == visitIndexCount && val == def)
                return true;
            serialize(val);
            insertToken(',');
        } else {
            serializeFieldDef(name, val, def);
        }
        return true;            // keep going
    }

    template <typename T>
    void serializeVisitable(const T *val)
    {
        const char* parens = ((flags&JSON) && visitIndex == 0) ? "[]" : "{}";
        insertToken(parens[0]);
        if ((flags&SUMMARIZE_PRETTY) && indent > maxIndent)
        {
            o += "...";
        }
        else if (val)
        {
            visitIndexCount = GetVisitIndexCount<T>::value;
            const_cast<T*>(val)->accept(*this);
        }
        insertToken(parens[1]);
    }

    template <typename T>
    void serialize(const T *val, typename T::VisitEnabled=0)
    {
        visitIndex = -1;
        return serializeVisitable<T>(val);
    }

    template <typename T>
    void serialize(const T *val, typename T::VisitIndexedEnabled=0)
    {
        visitIndex = 0;
        return serializeVisitable<T>(val);
    }

    template <typename T, typename = typename std::enable_if<!std::is_scalar<T>::value>::type >
    void serialize(const T &val)
    {
        return serialize(&val);
    }

    inline friend bool SaveFile(const string &fname, const SaveSerializer &ss) {
        return SaveFile(fname, ss.o);
    }

    inline friend bool SaveCompressedFile(const string &fname, const SaveSerializer &ss) {
        return SaveCompressedFile(fname, ss.o);
    }
};


struct ParserLocation {

    string fname;
    int    line = -1;
    
    string currentLine;
    int    currentLineColumn = -1;
    bool   isEof = false;
    vector<string> includes;

    mutable LogRecorder *logger = NULL;

    ParserLocation() {}
    ParserLocation(string s) : fname(move(s)) {}

    string format(const char* type, const string &msg, bool first=true) const;
    string shortLoc() const;
    void Report(const char* type, const string &msg) const;
};

// prettyprint types
// use like TypeSerializer::toString<TYPE>()
#define PRETTY_TYPE(T) (TypeSerializer::toString<T>().c_str())
#define PRETTY_TYPE_S(T) (TypeSerializer::toString<T>())
class TypeSerializer {

    static string cleanupType(string s);

    static string prettyTypeName(const lstring *v) { return "string"; }
    static string prettyTypeName(const std::string *v) { return "string"; }
    static string prettyTypeName(const uint64* v) { return "uint64"; }
    static string prettyTypeName(const unsigned int* v) { return "uint"; }
    static string prettyTypeName(const unsigned char* v) { return "uint8"; }
    static string prettyTypeName(const unsigned short* v) { return "uint16"; }
    static string prettyTypeName(const char* v) { return "int8"; }
    static string prettyTypeName(const short* v) { return "int16"; }

    template <typename T, int S, fixed_scalar_t I>
    static string prettyTypeName(const FixedPt<T, S, I> *v) {
        typedef FixedPt<T, S, I> F;
        return str_format("fixed<%g to %g by %g>", F::lowest(), F::max(), F::epsilon());
    }
    
    template <typename T>
    static string prettyTypeName(const T *val) { return cleanupType(typeid(T).name()); }

    template <typename T>
    static string prettyTypeName(const T **val) { return prettyTypeName((T*)NULL); }
    
    template <typename T>
    static string prettyTypeName(const copy_ptr<T> *val) { return prettyTypeName((T*)NULL); }
    
    template <typename T>
    static string prettyTypeName(const SerialEnum<T> *val) {
        string s = TYPE_NAME_S(T);
        s.pop_back(); // remove _
        return s;
    }

    template <typename T>
    static string prettyTypeName(const vector<T> *val) {
        return str_format("vector<%s>", prettyTypeName((T*)NULL).c_str());
    }

    template <typename T>
    static string prettyTypeName(const deque<T> *val) {
        return str_format("vector<%s>", prettyTypeName((T*)NULL).c_str());
    }

    template <typename T>
    static string prettyTypeName(const std::set<T> *val) {
        return str_format("vector<%s>", prettyTypeName((T*)NULL).c_str());
    }

    template <typename T, size_t S>
    static string prettyTypeName(const std::array<T, S> *val) {
        return str_format("array<%s, %d>", prettyTypeName((T*)NULL).c_str(), (int)S);
    }

    template <typename K, typename V>
    static string prettyTypeName(const std::map<K, V> *val) {
        return str_format("map<%s, %s>", prettyTypeName((K*)NULL).c_str(), prettyTypeName((V*)NULL).c_str());
    }

    template <typename K, typename V>
    static string prettyTypeName(const std::unordered_map<K, V> *val) {
        return str_format("map<%s, %s>", prettyTypeName((K*)NULL).c_str(), prettyTypeName((V*)NULL).c_str());
    }

    template <typename T, typename U>
    static string prettyTypeName(const pair<T, U> *val) {
        return str_format("pair<%s, %s>", prettyTypeName((T*)NULL).c_str(),
                          prettyTypeName((U*)NULL).c_str());
    }

    template <typename T>
    static string prettyTypeName(const glm::tvec2<T> *val) { return prettyTypeName((T*)NULL) + "2"; }
    template <typename T>
    static string prettyTypeName(const glm::tvec3<T> *val) { return prettyTypeName((T*)NULL) + "3"; }
    template <typename T>
    static string prettyTypeName(const glm::tvec4<T> *val) { return prettyTypeName((T*)NULL) + "4"; }

public:
    
    template <typename T> static string toString(const T* val=NULL) { return prettyTypeName(val); }
};


struct IFieldDispatch {
    virtual bool parse(SaveParser &p, void *ptr) const = 0;
    virtual bool skip(SaveParser &p) const = 0;
    virtual ~IFieldDispatch() {}
};

template <typename T>
struct FieldDispatcher : public IFieldDispatch {

    bool parse(SaveParser &p, void *ptr) const override;
    bool skip(SaveParser &p) const override;

    static FieldDispatcher &instance()
    {
        static FieldDispatcher it;
        return it;
    }
};

template <typename V>
struct FieldTypeIndex {

private:
    struct Entry {
        IFieldDispatch *type;
        uintptr_t       offset;
    };
    unordered_map<string, Entry> names;
    vector<Entry>                indexes;
    V*                           setup_base;

    FieldTypeIndex(V* def) : setup_base(def)
    {
        def->accept(*this);
    }

    bool dispatch(SaveParser &p, const Entry &en, void *base) const
    {
        if (en.offset != ~0) {
            return en.type->parse(p, (char*) base + en.offset);
        } else {
            return en.type->skip(p);
        }
    }
    
public:

    static FieldTypeIndex& instance(V *def=NULL)
    {
        static FieldTypeIndex it(def);
        return it;
    }

    template <typename T>
    bool visit(const char* str, T &val, const T& def=T())
    {
        Entry e = { &FieldDispatcher<T>::instance(), (uintptr_t) &val - (uintptr_t) setup_base };
        names[str] = e;
        indexes.push_back(e);
        return true;
    }

    template <typename T>
    bool visitSkip(const char *name)
    {
        Entry e = { &FieldDispatcher<T>::instance(), (uintptr_t) ~0 };
        names[name] = e;
        // indexes.push_back(e);
        return true;
    }

    void registerAbbrev(const char* name, const char* abbrev)
    {
        DASSERT(map_contains(names, name));
        names[abbrev] = names[name];
    }

    int parse(SaveParser &p, void *base, const string &name)
    {
        const auto &it = names.find(name);
        if (it == names.end())
            return -1;
        return dispatch(p, it->second, base);
    }

    int parse(SaveParser &p, void *base, uint idx)
    {
        if (idx >= indexes.size())
            return -1;
        return dispatch(p, indexes[idx], base);
    }
};


struct SaveFileParseContext {
    string       fname;
    string       buffer;
    const char*  data         = NULL;
    const char*  start        = NULL;
    const char*  dend         = NULL;
    uint         dataline     = 0;
    const char*  dataLastLine = NULL;
};

struct SaveParser : private SaveFileParseContext {

private:
    
    float*       progress     = NULL;
    mutable int  error_count  = 0;
    mutable int  warn_count   = 0;
    bool         fail_ok      = false;
    bool         cliMode      = false;
    bool         mergeMode    = false;
    bool         skipSpaceEntrant = false;
    LogRecorder *logger       = NULL;
    // std::unordered_map<uint, const BlockCluster*> indexedClusters;
    struct CachedCluster {
        const BlockCluster*     bp = NULL;
        copy_ptr<SerialCluster> sc;
    };
    unordered_map<uint, CachedCluster> indexedClusters;
    uint                               incompleteClusterHash = 0;
    vector<const BlockCluster **>      incompleteClusters;
    vector<SaveFileParseContext>       includeStack;

    bool parseBinaryIntegral(uint64 *v);
    bool parseIntegral(uint64* v);
    bool parseIntegral(long long *v);

    template <typename T>
    bool parseIntegral(T* v)
    {
        long long lval = 0;
        if (!parseIntegral(&lval))
            return false;
        PARSE_FAIL_UNLESS2((lval >= std::numeric_limits<T>::min() &&
                           lval <= std::numeric_limits<T>::max()),
                          "integral constant %lld is out of range for %s", lval, TYPE_NAME(T));
        PARSE_FAIL_UNLESS2(*data != '.', "Expecting integer, got float");
        *v = (T) lval;
        return true;
    }
    
    bool parseFieldIndex(SerialBlock *sb, uint index);
    bool parseField(SerialBlock* sb, const string& s);
    bool parseBinaryBlock(SerialBlock *sb);
    bool parseScalar(SerialCluster *sc);
    bool addToIndex(const SerialCluster *sc);
    bool addBlueprintToIndex(CachedCluster &cc, BlockCluster *bp);
    bool getCachedBlueprint(uint hash, const BlockCluster** obp);

    bool parseQuotedString(string *s, char quote);

    template <typename T>
    bool parseScalar(T* f) { return false; }

    template <typename T, typename V>
    bool parseScalarVec(V *t)
    {
        T v{};
        fail_ignore i(this);
        if (!parse(&v))
            return false;
        *t = V(v);
        return true;
    }

    template <typename T>
    bool parseScalar(glm::tvec2<T> *t) { return parseScalarVec<T>(t); }

    template <typename T>
    bool parseScalar(glm::tvec3<T> *t) { return parseScalarVec<T>(t); }

    template <typename T>
    bool parseStruct(T* v)
    {
        if (parseToken('{'))
            return parseStructContents(v, '}');
        if (parseScalar(v))
            return true;
        PARSE_FAIL("expected opening '{' for %s", PRETTY_TYPE(T));
    }
    
    template <typename T>
    bool parseField(glm::tvec2<T>* v, const string& s)
    {
        switch (s[0]) {
        case 'x': return parse(&v->x);
        case 'y': return parse(&v->y);
        }
        return false;
    }

    template <typename T>
    bool parseFieldIndex(glm::tvec2<T>* v, uint i)
    {
        return (i < 2) && parse(&(*v)[i]);
    }

    template <typename T>
    bool parseField(glm::tvec3<T>* v, const string& s)
    {
        switch (s[0]) {
        case 'x': return parse(&v->x);
        case 'y': return parse(&v->y);
        case 'z': return parse(&v->z);
        }
        return false;
    }

    template <typename T>
    bool parseFieldIndex(glm::tvec3<T>* v, uint i)
    {
        return (i < 3) && parse(&(*v)[i]);
    }

    template <typename T>
    bool parseField(glm::tvec4<T>* v, const string& s)
    {
        switch (s[0]) {
        case 'x': return parse(&v->x);
        case 'y': return parse(&v->y);
        case 'z': return parse(&v->z);
        case 'w': return parse(&v->w);
        }
        return false;
    }

    template <typename T>
    bool parseFieldIndex(glm::tvec4<T>* v, uint i)
    {
        return (i < 4) && parse(&(*v)[i]);
    }

    template <typename T, typename V, typename IT>
    bool parseList(V *v, IT it)
    {
        if (!mergeMode)
            v->clear();
        
        PARSE_FAIL_UNLESS(parseToken('{'), "expected opening '{' for %s", PRETTY_TYPE(V));

        int i=0;
        while (!parseToken('}'))
        {
            T el{};
            PARSE_FAIL_UNLESS(parse(&el), "while parsing %s[%d]", PRETTY_TYPE(V), i);
            *it = std::move(el);
            ++it;
            ++i;

            parseToken(','); // optional
        }

        return true;
    }


    bool fail1(const char* func, uint line, int level, const char* errformat, ...) const __printflike(5, 6);

    char nextChar();

    bool loadFileInclude(string fn);

public:

    SaveParser() ;
    ~SaveParser();

    SaveParser(const char* d, bool fok=false);
    SaveParser(const string &d, bool fok=false);

    bool loadFile(const string& fname_);
    void resetFile();
    bool loadData(const char* dta, const string &fn="-");

    bool isEof()
    {
        skipSpace();
        return !data || *data == '\0';
    }

    void checkEof(const char *tname)
    {
        parseToken(',');        // ignore trailing comma!
        if (isEof()) {
            ReportEarlyf("Loaded %s : %s", fname.c_str(), tname);
        } else {
            error("Garbage after parsing %s", tname);
        }
    }

    void setLogger(LogRecorder *l) { logger = l; }
    LogRecorder *getLogger() const { return logger; }
    
    void setCliMode(bool val) { cliMode = val; }
    bool getCliMode() const { return cliMode; }

    void setMergeMode(bool val) { mergeMode = val; }
    bool getMergeMode() const { return mergeMode; }

    const char* c_str() const { return data; }
    int size() const { return data ? strlen(data) : 0; }
    const std::string &getBuffer() const { return buffer; }

    // return true if data completely parses into v.
    // may write v but return false if there is trailing garbage
    template <typename S, typename T>
    static bool parse(const S &data, T *v, bool fok=false)
    {
        SaveParser p(data, fok);
        return p.parse(v) && p.isEof();
    }

    struct fail_ignore {
        bool        saved_fail_ok;
        SaveParser *self;
        fail_ignore(SaveParser *p) {
            self = p;
            saved_fail_ok = self->fail_ok;
            self->fail_ok = true;
        }
        ~fail_ignore() {
            self->fail_ok = saved_fail_ok;
        }
    };

    const string &getFileName() const { return fname; }
    void setFailOk(bool ok) { fail_ok = ok; }
    bool isFailOK() const { return fail_ok; }
    void setProgress(float *p) { progress = p; }
    size_t size();

    ParserLocation getCurrentLoc() const;
    string errmsg(const char* type, const string& msg) const;
    string errfmt(const char* type, const char* errformat, ...) const __printflike(3, 4);
    void warn(const char* errformat, ...) const __printflike(2, 3);
    bool error(const char* errformat, ...) const __printflike(2, 3);
    
    bool skipSpace();

    template <typename T>
    bool skip()
    {
        T v;
        return parse(&v);
    }

    bool parseToTokens(string *s, const char *token);
    bool parseToken(char token);
    bool parseTokenOnly(const char *token);
    bool atToken(char token);
    bool parseIdent(string* s);
    bool parseIdentAnd(string* s, char token);
    bool parseArg(string* s);
    bool parseName(string* s);
    bool parseName(lstring* s)
    {
        string str;
        if (parseName(&str)) {
            *s = lstring(str);
            return true;
        }
        return false;
    }
    bool parse(string* s);
    bool parse(lstring* s);
    bool parse(bool* v);
    bool parse(uint* v) { return parseIntegral(v); }
    bool parse(uint64* v) { return parseIntegral(v); }
    bool parse(int* v) { return parseIntegral(v); }
    bool parse(char* v) { return parseIntegral(v); }
    bool parse(uchar* v) { return parseIntegral(v); }
    bool parse(schar* v) { return parseIntegral(v); }
    bool parse(short* v) { return parseIntegral(v); }
    bool parse(ushort* v) { return parseIntegral(v); }
    bool parse(float* v);
    bool parse(double* v);
    bool parseE(uint64 *h, const EnumType &e);

    template <typename T, int S, fixed_scalar_t I>
    bool parse(FixedPt<T, S, I> *v)
    {
        typedef FixedPt<T, S, I> F;
        float f = 0.f;
        if (!parse(&f))
            return false;
        if (!(F::lowest() <= f && f <= F::max() && (f == 0.f || abs(f) >= F::epsilon())))
        {
            float g = clamp(f, F::lowest(), F::max());
            if (g != 0 && abs(g) < F::epsilon())
                g = sign(g) * F::epsilon();
            warn("clamping float constant %g -> %g for %s", f, g, PRETTY_TYPE(F));
            f = g;
        }
        // PARSE_FAIL_UNLESS((F::lowest() <= f && f <= F::max()) && (f == 0.f || abs(f) >= F::epsilon()),
                          // "float constant %g is out of range for %s", f, PRETTY_TYPE(F));
        *v = f;
        return true;
    }

    template <typename T>
    bool parse(SerialEnum<T> *enm)
    {
        uint64 v = 0;
        bool success = parseE(&v, T::getType());
        PARSE_FAIL_UNLESS((v >= std::numeric_limits<typename T::value_type>::min() &&
                           v <= std::numeric_limits<typename T::value_type>::max()),
                          "integral constant %lld is out of range for %s", v, PRETTY_TYPE(T));
        enm->value = v;
        return success;
    }

    bool parse(BlockCluster **bc);
    bool parse(const BlockCluster** sc); // may return shipLoader version

    bool parse(SerialBlock* sb);
    bool parse(SerialCluster* sc);

    // may be overridden
    template <typename T>
    bool parse(T* v)
    {
        return parseStruct(v);
    }

    template <typename T>
    bool parse(copy_ptr<T> *ptr)
    {
        return parse(ptr->getPtr()); 
    }
    
    template <typename T>
    bool parse(T** psb)
    {
        typedef typename std::remove_const<T>::type U;
        U* v = const_cast<U*>(*psb);
        if (v)
        {

        }
        else
        {
            v = new U();
            *psb = v;
        }
        return parse(v);
    }

    template <typename T> bool parse(vector<T>* v) { return parseList<T>(v, back_inserter(*v)); }
    template <typename T> bool parse(deque<T>* v) { return parseList<T>(v, back_inserter(*v)); }
    template <typename T> bool parse(std::set<T>* v) { return parseList<T>(v, inserter(*v, v->end())); }
    template <typename K, typename V> bool parse(std::map<K, V>* mp) { return parseMap(mp); }
    template <typename K, typename V> bool parse(std::unordered_map<K, V>* mp) { return parseMap(mp); }

    template <typename T, size_t S> bool parse(std::array<T, S> *v)
    {
        PARSE_FAIL_UNLESS(parseToken('{'), "expected opening '{' for array<%s, %d>", PRETTY_TYPE(T), (int)S);

        for (int i=0; i<S; i++)
        {
            PARSE_FAIL_UNLESS(parse(&(*v)[i]), "while parsing array<%s, %d>[%d]", PRETTY_TYPE(T), (int)S, i);
            parseToken(','); // optional            
        }

        PARSE_FAIL_UNLESS(parseToken('}'), "expected closing '}' for array<%s, %d>", PRETTY_TYPE(T), (int)S);

        return true;
    }

    template <typename K, typename V>
    bool parse(std::pair<K, V> *v)
    {
        PARSE_FAIL_UNLESS(parseToken('{'), "expected opening '{' for pair<%s, %s>", PRETTY_TYPE(K), PRETTY_TYPE(V));

        PARSE_FAIL_UNLESS(parse(&v->first), "while parsing pair<%s, %s> key", PRETTY_TYPE(K), PRETTY_TYPE(V));
        parseToken(','); // optional
        PARSE_FAIL_UNLESS(parse(&v->second), "while parsing pair<%s, %s> value", PRETTY_TYPE(K), PRETTY_TYPE(V));

        PARSE_FAIL_UNLESS(parseToken('}'), "expected '}' for pair<%s, %s>", PRETTY_TYPE(K), PRETTY_TYPE(V));
        
        return true;
    }

private:
    
    template <typename T>
    bool parseBracketKey(T *v)
    {
        if (!parseToken('['))
            return false;
        PARSE_FAIL_UNLESS(parse(v), "while parsing %s key", PRETTY_TYPE(T));
        PARSE_FAIL_UNLESS(parseToken(']'), "expected closing ']' after %s while parsing key", PRETTY_TYPE(T));
        return true;
    }

    template <typename T>
    bool parseKey(T *v)
    {
        return parseBracketKey(v) || parse(v);
    }

    bool parseKey(std::string *v)
    {
        return parseBracketKey(v) || parseIdent(v) || parse(v);
    }

    template <typename K, typename V>
    bool parseAssign(std::pair<K, V> *p)
    {
        PARSE_FAIL_UNLESS(parseKey(&p->first), "while parsing map<%s, %s> key", PRETTY_TYPE(K), PRETTY_TYPE(V));
        PARSE_FAIL_UNLESS(parseToken('='), "expected '=' while parsing map<%s, %s>", PRETTY_TYPE(K), PRETTY_TYPE(V));
        PARSE_FAIL_UNLESS(parse(&p->second), "while parsing map<%s, %s> value", PRETTY_TYPE(K), PRETTY_TYPE(V));
        return true;
    }
    
    template <typename T>
    bool parseMap(T* mp)
    {
        PARSE_FAIL_UNLESS(parseToken('{'), "expected opening '{' for %s", PRETTY_TYPE(T));

        while (!parseToken('}'))
        {
            std::pair<typename T::key_type, typename T::mapped_type> p;
            if (!parseAssign(&p))
                return false;
            if (mergeMode)
            {
                (*mp)[std::move(p.first)] = std::move(p.second);
            }
            else
            {
                std::pair<typename T::iterator, bool> x = mp->insert(std::move(p));
                if (!x.second)
                    warn("Ignoring duplicate map key %s", SaveSerializer::toString(x.first->first).c_str());
            }
            parseToken(','); // optional
        }
        
        return true;
    }

    // opening bracket already parsed
    template <typename T>
    bool parseStructContents(T* sb, char terminator)
    {
        ParsePrepare<T>().prepare(*this, *sb);
        
        int i=0;
        std::string s;
        
        while (!parseToken(terminator))
        {
            if (parseIdentAnd(&s, '=')) {
                PARSE_FAIL_UNLESS(parseField(sb, s), "while parsing %s::%s", PRETTY_TYPE(T), s.c_str());
                i = -1;
            } else if (i == -1) {
                PARSE_FAIL("expected field name while parsing %s", PRETTY_TYPE(T));
            } else {
                // this parses one field for built-in types and ALL fields for visitable types
                if (parseFieldIndex(sb, i))
                    i++;
                else
                    i = -1;
            }
        
            parseToken(','); // optional
        }

        PARSE_FAIL_UNLESS(ParseValidate<T>().validate(*this, *sb), "while validating %s", PRETTY_TYPE(T));

        return true;
    }

public:

    bool parse(Level *l);

private:

    template <typename T>
    bool parseField(T *val, const string& s)
    {
        int ret = FieldTypeIndex<T>::instance().parse(*this, val, s);
        PARSE_FAIL_UNLESS(ret != -1, "no field '%s' in %s", s.c_str(), PRETTY_TYPE(T));
        PARSE_FAIL_UNLESS(ret == 1, "while parsing %s for field %s", PRETTY_TYPE(T), s.c_str());
        return true;
    }

    template <typename T>
    bool parseFieldIndex(T *val, uint index, typename T::VisitIndexedEnabled=0)
    {
        PARSE_FAIL_UNLESS(index == 0, "expected closing '}' after %s (%d fields already parsed)", PRETTY_TYPE(T), index);
        int visitIndex = 0;
        int visitIndexCount = GetVisitIndexCount<T>::value;
        do {
            PARSE_FAIL_UNLESS(FieldTypeIndex<T>::instance().parse(*this, val, visitIndex) == 1,
                              "while parsing %s at index %d", PRETTY_TYPE(T), index);
            visitIndex++;
            if (visitIndex >= visitIndexCount)
                return true;
            parseToken(','); // optional
        } while (!atToken('}'));
        return true;
    }

    template <typename T>
    bool parseFieldIndex(T* val, ...)
    {
        return false;
    }
};

template <typename T>
bool FieldDispatcher<T>::parse(SaveParser &p, void *ptr) const
{
    return p.parse((T*) ptr);
}

template <typename T>
bool FieldDispatcher<T>::skip(SaveParser &p) const
{
    return p.skip<T>();
}


template <typename T>
string SerialEnum<T>::toString() const
{
    return SaveSerializer::toString(*this);
}

template <typename T>
struct Serializable {

    const char* m_filename = NULL;

    bool loadFromFile(const char *f)
    {
        m_filename = f;
        return loadFileAndParse(m_filename, ((T*)this)) == LS_OK;
    }
    
    bool writeToFile() const
    {
        ASSERT(m_filename);
        if (!m_filename)
            return false;
        string ss = toString(100);
        bool success = SaveFile(m_filename, ss);
        Reportf("Wrote %d bytes to '%s': %s %s", (int)ss.size(), m_filename, success ? "OK" : "FAILED",
                       ss.size() < 100 ? ss.c_str() : "");
        return success;
    }
    
    string toString(int columnWidth=-1) const
    {
        SaveSerializer sr;
        sr.setFlag(SaveSerializer::EXPLICIT_FIELDS, true);
        sr.setColumnWidth(columnWidth);
        sr.serializeVisitable((T*) this);
        return sr.str();
    }

    static T& instance()
    {
        static T* v = new T();
        return *v;
    }

    bool isLoaded() const { return m_filename; }
};

template <typename T>
LoadStatus loadFileAndParse1(const string& fname, T* data, SaveParser &p)
{
    if (!p.loadFile(fname)) {
        Reportf("%s does not exist", fname.c_str());
        return LS_MISSING;
    }
    if (!p.parse(data)) {
        Reportf("Error parsing %s : %s", fname.c_str(), PRETTY_TYPE(T));
        return LS_ERROR;
    }
    return LS_OK;
}

template <typename T>
LoadStatus loadFileAndParse(const string& fname, T* data, float* progress=NULL)
{
    SaveParser p;
    p.setProgress(progress);
    LoadStatus status = loadFileAndParse1(fname, data, p);
    if (status != LS_OK)
        return status;
    p.checkEof(PRETTY_TYPE(T));
    return status;
}

template <typename T>
LoadStatus loadFileAndParseMaybe(const string& fname, T* data)
{
    SaveParser p;
    if (!p.loadFile(fname))
        return LS_MISSING;
    if (!p.parse(data)) {
        Reportf("Error parsing %s : %s", fname.c_str(), PRETTY_TYPE(T));
        return LS_ERROR;
    }
    p.checkEof(PRETTY_TYPE(T));
    return LS_OK;
}

template <typename T>
LoadStatus loadFileAndParseMaybe1(const string& fname, T* data, SaveParser &p)
{
    if (!p.loadFile(fname))
        return LS_MISSING;
    if (!p.parse(data)) {
        Reportf("Error parsing %s : %s", fname.c_str(), PRETTY_TYPE(T));
        return LS_ERROR;
    }
    p.checkEof(PRETTY_TYPE(T));
    return LS_OK;
}

template <typename T>
LoadStatus loadDataAndParse(const string& fname, const char* bytes, T* data, bool silent_success=false)
{
    SaveParser p;
    p.loadData(bytes, fname.c_str());
    if (!p.parse(data)) {
        Reportf("Error parsing %s : %s", fname.c_str(), PRETTY_TYPE(T));
        return LS_ERROR;
    }

    if (!silent_success)
        Reportf("Succesfully loaded %s : %s", fname.c_str(), PRETTY_TYPE(T));
    return LS_OK;
}

// write data to LOCAL file (not steam cloud)
template <typename T>
bool serializeToFile(const string &fname, const T& val, SaveSerializer &ss)
{
    ss.setFlag(SaveSerializer::EXPLICIT_CLUSTERS_ALWAYS);
    ss.serialize(val);
    return ZF_SaveFileRaw(fname.c_str(), ss.str());
}

// write data to LOCAL file (not steam cloud)
template <typename T>
bool serializeToFile(const string &fname, const T& val)
{
    SaveSerializer ss;
    ss.setFlag(SaveSerializer::EXPLICIT_CLUSTERS_ALWAYS);
    ss.serialize(val);
    return ZF_SaveFileRaw(fname.c_str(), ss.str());
}

template <typename T>
string serializeForExport(const T& val)
{
    SaveSerializer ss;
    ss.setFlag(SaveSerializer::EXPLICIT_CLUSTERS_ALWAYS);
    if (kWriteJSON)
        ss.setFlag(SaveSerializer::JSON);
    ss.serialize(val);
    return ss.str();
}


struct CVarBase {

protected:
    const lstring m_name;
    const string m_comment;
    bool m_has_minmax = false;

    static std::map<lstring, CVarBase*>& index();
    static std::map<lstring, pair<string, string> >& incomplete_index();
    static std::mutex &mutex();

    bool initialize();

    template <typename T>
    bool parseValueOr1(SaveParser &sp, SerialEnum<T> *val)
    {
        SerialEnum<T> val1;
        if (!sp.parse(&val1))
            return false;
        *val |= val1;
        ReportEarlyf("CVAR %s = %s (via |=)", getName().c_str(), toString().c_str());
        return true;
    }

    template <typename T>
    bool parseValueOr1(SaveParser &sp, T *val) { return false; }

    template <typename T>
    bool parseValueAndNot1(SaveParser &sp, SerialEnum<T> *val)
    {
        SerialEnum<T> val1;
        if (!sp.parse(&val1))
            return false;
        *val &= ~val1;
        ReportEarlyf("CVAR %s = %s (via &~)", getName().c_str(), toString().c_str());
        return true;
    }

    template <typename T>
    bool parseValueAndNot1(SaveParser &sp, const T *val) { return false; }

    template <typename T>
    bool parseValuePlus2(SaveParser &sp, T *v)
    {
        T q{};
        if (!sp.parse(&q))
            return false;
        *v += q;
        ReportEarlyf("CVAR %s = %s (via +=)", getName().c_str(), toString().c_str());
        return true;
    }

    bool parseValuePlus1(SaveParser &sp, float *v) { return parseValuePlus2(sp, v); }
    bool parseValuePlus1(SaveParser &sp, int *v) { return parseValuePlus2(sp, v); }
    template <typename T>
    bool parseValuePlus1(SaveParser &sp, glm::tvec2<T> *v) { return parseValuePlus2(sp, v); }
    template <typename T>
    bool parseValuePlus1(SaveParser &sp, glm::tvec3<T> *v) { return parseValuePlus2(sp, v); }
    template <typename T>
    bool parseValuePlus1(SaveParser &sp, glm::tvec4<T> *v) { return parseValuePlus2(sp, v); }
    
    template <typename T>
    bool parseValuePlus1(SaveParser &sp, const T *val) { return false; }

    virtual bool parseValue(SaveParser &sp) = 0;
    virtual bool parseValueOr(SaveParser &sp) = 0;
    virtual bool parseValuePlus(SaveParser &sp) = 0;
    virtual bool parseValueAndNot(SaveParser &sp) = 0;

public:

    CVarBase(lstring name, const char* comment);
    virtual ~CVarBase();

    lstring getName() const { return m_name; }
    const char* getComment() const { return m_comment.size() ? m_comment.c_str() : NULL; }
    bool hasMinMax() const { return m_has_minmax; }
    virtual bool isDefault() const = 0;
    virtual std::string toString() const = 0;
    virtual std::string getTypeString() const = 0;
    virtual void serialize(SaveSerializer &ss) const = 0;

    static std::string get(lstring key);
    static bool        parseCVars(SaveParser &sp);
    static void        reportIncomplete();
    static bool        writeToFile();
    static std::string getAll();
    static vector<std::string> getAllNames();
    static void        deleteCVars();
};


template <typename T>
struct CVar final : public CVarBase {

    T* const m_vptr;
    const T m_def, m_min, m_max;

    CVar(lstring name, T* vptr, const T& def, const char* comment) :
        CVarBase(name, comment), m_vptr(vptr), m_def(def), m_min(), m_max()
    {
        initialize();
    }

    CVar(lstring name, T* vptr, const T& def, const char* comment, const T& mn, const T& mx) :
        CVarBase(name, comment), m_vptr(vptr), m_def(def), m_min(mn), m_max(mx)
    {
        m_has_minmax = true;
        initialize();
    }

    const T& getValue() const
    {
        return *m_vptr;
    }

    const T getDefault() const 
    {
        return m_def;
    }

    virtual bool isDefault() const
    {
        return *m_vptr == m_def;
    }

    virtual bool parseValue(SaveParser &sp)
    {
        if (sp.parse(m_vptr)) {
            const string val = toString().c_str();
            if (!checkMinMax()) {
                sp.warn("%s: %s: value %s clamped to %s", m_name.c_str(),
                        getTypeString().c_str(), val.c_str(), toString().c_str());
            }
            ReportEarlyf("CVAR %s = %s", getName().c_str(), toString().c_str());
            return true;
        }
        return false;
    }

    virtual bool parseValueOr(SaveParser &sp) { return parseValueOr1(sp, m_vptr); }
    virtual bool parseValuePlus(SaveParser &sp) { return parseValuePlus1(sp, m_vptr); }
    virtual bool parseValueAndNot(SaveParser &sp) { return parseValueAndNot1(sp, m_vptr); }
    
    virtual std::string toString() const
    {
        return SaveSerializer::toString((T) *m_vptr);
    }

    virtual std::string getTypeString() const
    {
        std::string s = PRETTY_TYPE_S(T);
        if (hasMinMax()) {
            s += "[";
            s += SaveSerializer::toString(m_min);
            s += " - ";
            s += SaveSerializer::toString(m_max);
            s += "]";
        }
        return s;
    }

    virtual void serialize(SaveSerializer &ss) const
    {
        ss.serialize((T) *m_vptr);
    }
        
    bool checkMinMax() { return true; }
};

template <typename T>
bool checkMinMax1(CVar<T> *cv)
{
    if (!cv->hasMinMax())
        return true;
    T cl = clamp(*cv->m_vptr, cv->m_min, cv->m_max);
    if (cl == *cv->m_vptr)
        return true;
    *cv->m_vptr = cl;
    return false;
}


template<> inline bool CVar<float>::checkMinMax() { return checkMinMax1(this); }
template<> inline bool CVar<int>::checkMinMax() { return checkMinMax1(this); }
template<> inline bool CVar<float2>::checkMinMax() { return checkMinMax1(this); }

template <typename T>
T CreateCVar(lstring name, T* vptr, const T& value, const char* comment="")
{
    return (new CVar<T>(name, vptr, value, comment))->getDefault();
}

template <typename T>
T CreateCVar(lstring name, T* vptr, const T& value, const char* comment, const T& mn, const T& mx)
{
    DASSERT(clamp(value, mn, mx) == value);
    CVar<T> *cv = new CVar<T>(name, vptr, value, comment, mn, mx);
    return cv->getDefault();
}

#define DEFINE_CVAR1(TYPE, NAME, ...)                     \
    TYPE NAME = CreateCVar< TYPE >(#NAME, &NAME, __VA_ARGS__)
    
#define CREATE_CVAR(NAME, VALUE) CreateCVar(#NAME, &(NAME), (VALUE))

void loadCVars(const char* txt);
void setCVars(const char* txt);
string cmd_cvar(void* data, const char* name, const char *args);
string cmd_save_cvars(void* data, const char* name, const char *args);

typedef SaveSerializer SSerializer;
typedef SaveParser SParser;

bool isParsableFileExt(const string &path);

#endif
