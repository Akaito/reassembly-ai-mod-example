
#pragma once

#include "Mods.h"

static void scaleCluster(BlockCluster *cl, int scale)
{
    if (scale == 0)
        return;
    foreach (Block *bl, cl->blocks)
        bl->doScale(scale, vector<BlockId_t>());
    cl->repatchPorts();
    cl->init();
}

static const lstring l_proggy = lstring("proggy");
static const lstring l_def = lstring("def");

struct BlockFont {

    lstring                             name;
    int                                 scale = 1;
    std::map<char, copy_ptr<const BlockCluster>> data;
    string                              path;
    static const uint persistentIdent = 0xc0011321;

    typedef std::map< pair<lstring, int>, BlockFont> FontDatabase;
    
    typedef int VisitIndexedEnabled;
    
    template <typename V>
    bool accept(V& vis)
    {
        return vis.VISIT(data);
    }

    const BlockCluster *getCharCluster(char chr) const	
    {
        return or_(map_get(data, chr).get(), map_get(data, islower(chr) ? toupper(chr) : tolower(chr)).get());
    }

    float2 getOffset(char chr) const
    {
        if (name == l_proggy)
        {
            switch (chr) {
            case 'y':
            case 'g':
            case 'p':
            case 'q':
                return justY(-2.f * scale * kComponentWidth);
            }
        }
        else if (name == l_def)
        {
            switch (chr) {
            case 'q': return justY(-scale * kComponentWidth);
            }
        }
        return float2(0.f);
    }

    float getLeft(char chr, const BlockCluster *bp) const
    {
        float width = -bp->bbox.mn.x;
        if (name == l_proggy)
        {
            switch (chr) {
            case 'l': width -= scale * kComponentWidth; break;
            }
        }
        else if (name == l_def)
        {
            switch (chr) {
            case 't':
            case 'y': width -= scale * 1.f * kComponentWidth; break;
            }
        }
        return width;
    }

    float getRight(char chr, const BlockCluster *bp) const
    {
        float width = bp->bbox.mx.x;
        if (name == l_def)
        {
            switch (chr) {
            case 'q': width -= scale * 2.f * kComponentWidth; break;
            case 'k':
            case 'l': width -= scale * 1.f * kComponentWidth; break;
            }
        }
        return width;
    }

    float getWidth(char chr, const BlockCluster *bp) const
    {
        return getLeft(chr, bp) + getRight(chr, bp);
    }

    struct TextLayout {
        float2 charsize;
        float2 size;
        vector<float> widths;
        float2 padding;
    };

    AABBox getBBox(const string& text, float2 start, float2 rval, TextLayout *pts=NULL) const
    {
        TextLayout ts;
        foreach (auto &x, data)
        {
            const float2 rad = x.second->bbox.getRadius();
            ts.charsize.x += 2.f * rad.x;
            ts.charsize.y = max(ts.charsize.y, 2.f * rad.y);
        }
        ts.charsize.x /= data.size();
        ts.padding = ts.charsize * float2(0.25f, 0.1f);

        float width2 = 0.f;
        bool lastWasEscape = false;
        foreach (char cr, text)
        {
            if (cr == '^') {
                lastWasEscape = true;
            } else if (lastWasEscape) {
                lastWasEscape = false;
            } else if (cr == ' ') {
                ts.size.x += ts.charsize.x;
            } else if (cr == '\n') {
                ts.widths.push_back(ts.size.x);
                width2 = max(width2, ts.size.x);
                ts.size.x = 0.f;
                ts.size.y += ts.charsize.y + ts.padding.y;
            } else {
                const BlockCluster *bp = getCharCluster(cr);
                if (!bp)
                    continue;
                ts.size.x += getWidth(cr, bp) + ts.padding.x;
            }
        }
        ts.widths.push_back(ts.size.x);
        ts.size.x = max(ts.size.x, width2);
        ts.size.y += 0.5f * ts.charsize.y;

        if (pts)
            *pts = ts;

        AABBox bbox;
        bbox.insertPoint(start);
        bbox.insertPoint(start - float2(rval.x, 1.f - rval.y) * ts.size);
        bbox.insertPoint(start + float2(1 - rval.x, 0.f) * ts.size);
        return bbox;
    }

    float2 insertText(const string &text, GameZone *zone, float2 start, float2 rval, bool particles) const
    {
        TextLayout ts;
        getBBox(text, start, rval, &ts);

        const float2 spos = start - float2(rval.x, 1.f - rval.y) * ts.size;
        float2 pos = spos;
        int line = 0;

        pos.x += (ts.size.x - ts.widths[0]) * rval.x;

        bool lastWasEscape = false;
        uint color[3] = { 0xffffff, 0xcccccc, 0x808080 };
        foreach (char cr, text)
        {
            if (cr == '^') {
                lastWasEscape = true;
            } else if (lastWasEscape) {
                lastWasEscape = false;
                color[0] = OLG_GetQuake3Color(cr - '0');
                color[1] = colorChangeValue(color[0], 0.2);
                color[2] = colorChangeValue(color[0], 0.5);
            } else if (cr == ' ') {
                pos.x += ts.charsize.x;
            } else if (cr == '\n') {
                pos.y -= ts.charsize.y + ts.padding.y;
                pos.x = spos.x;
                line++;
                pos.x += (ts.size.x - ts.widths[line]) * rval.x;
            } else {
                const BlockCluster *bp = getCharCluster(cr);
                if (!bp)
                    continue;
                pos.x += getLeft(cr, bp);
                BlockCluster *cl = bp->instantiate();
                foreach (Block *bl, cl->blocks)
                {
                    bl->sb.fillColor = color[0];
                    bl->sb.fillColor1 = color[1];
                    bl->sb.lineColor = color[2];
                    bl->sb.persistentIdent = persistentIdent;
                }
                const float2 offset = getOffset(cr);
                const float2 adjust = float2(0.f, -(ts.charsize.y / 2.f) - bp->bbox.mn.y);
                cl->makeVegetable(true);
                cl->addToGameZone(zone, pos + adjust + offset);
                if (particles)
                    magicCoverCluster(cl);
                pos.x += getRight(cr, bp) + ts.padding.x;
            }
            
        }
        return ts.size;
    }

    static FontDatabase &instance()
    {
        static FontDatabase *fd = new FontDatabase();
        return *fd;
    }

    static BlockFont &get(lstring name, int scale)
    {
        BlockFont &font = instance()[make_pair(name, scale)];
        if (font.data.empty())
        {
            foreach (OneMod *mod, TheMods::instance().load_mods)
            {
                const string path = str_path_join(mod->folder, name.str() + ".lua");
                LoadStatus ls = loadFileAndParseMaybe(path, &font);
                if (ls == LS_ERROR) {
                    mod->status = EModStatus::CORRUPT;
                    break;
                } else if (ls == LS_OK) {
                    font.path = path;
                    break;
                }
            }
            if (font.data.empty())
            {
                const string path = str_format("data/blockfonts/%s.lua", name.c_str());
                loadFileAndParse(path, &font);
                font.path = path;
            }
            
            foreach (auto &x, font.data) {
                BlockCluster *cl = const_cast<BlockCluster*>(x.second.get());
                scaleCluster(cl, scale-1);
                foreach (Block *bl, cl->blocks)
                    bl->sb.features |= Block::ENVIRONMENTAL;
                if (name == l_def)
                {
                    foreach (Block *bl, cl->blocks)
                        bl->sb.features |= Block::INTLINES;
                }
            }
        }
        font.name = name;
        font.scale = scale;
        return font;
    }
    
};
