
//
// Shaders.h - some basic GLSL shaders
// 

// Copyright (c) 2013-2016 Arthur Danskin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef SHADERS_H
#define SHADERS_H

#include "Graphics.h"

struct ShaderPosBase : public ShaderProgramBase {

    virtual void UseProgram(const ShaderState& ss, const float2* ptr) const = 0;
    virtual ~ShaderPosBase() {}


    // a b
    // d c
    void DrawQuad(const ShaderState& ss, float2 a, float2 b, float2 c, float2 d) const;

    void DrawSquare(const ShaderState &ss, float r) const
    {
        DrawQuad(ss, float2(-r, -r), float2(-r, r), float2(r, r), float2(r, -r));
    }

    void DrawRect(const ShaderState &ss, float2 r) const
    {
        DrawQuad(ss, float2(-r.x, -r.y), float2(-r.x, r.y), r, float2(r.x, -r.y));
    }

    void DrawRect(const ShaderState &ss, float2 pos, float2 r) const
    {
        DrawQuad(ss, pos + float2(-r.x, -r.y), pos + float2(-r.x, r.y), 
                 pos + r, pos + float2(r.x, -r.y));
    }

    void DrawColorRect(ShaderState& ss, uint color, float2 r) const
    {
        if (color&ALPHA_OPAQUE) {
            ss.color32(color);
            DrawRect(ss, r);
        }
    }

    void DrawRectCorners(const ShaderState &ss, float2 a, float2 b) const
    {
        float2 bl(min(a.x, b.x), min(a.y, b.y));
        float2 ur(max(a.x, b.x), max(a.y, b.y));
        DrawQuad(ss, float2(bl.x, ur.y), ur, float2(ur.x, bl.y), bl);
    }

    void DrawLineRect(const ShaderState &ss, float2 pos, float2 rad) const
    {
        DrawLineRectCorners(ss, pos - rad, pos + rad);
    }

    // a b
    // c d
    void DrawLineQuad(const ShaderState& ss, float2 a, float2 b, float2 c, float2 d, 
                      bool outline=true, bool cross=false) const
    {
        const float2        v[] = { a, b, c, d };
        static const ushort i[] = {0,1, 1,3, 3,2, 2,0, 0,3, 1,2};

        UseProgram(ss, v);
        ss.DrawElements(GL_LINES, (outline ? 8 : 0) + (cross ? 4 : 0), outline ? i : (i + 8));
        UnuseProgram();
    }

    void DrawLineSquare(const ShaderState& ss, float r) const
    {
        DrawLineQuad(ss, float2(-r, -r), float2(-r, r), float2(r, -r), float2(r, r));
    }

    void DrawLineRect(const ShaderState& ss, float2 r) const
    {
        DrawLineQuad(ss, float2(-r.x, -r.y), float2(-r.x, r.y), float2(r.x, -r.y), r);
    }

    void DrawLineRectCorners(const ShaderState& ss, float2 a, float2 b) const
    {
        float2 bl(min(a.x, b.x), min(a.y, b.y));
        float2 ur(max(a.x, b.x), max(a.y, b.y));
        DrawLineQuad(ss, float2(bl.x, ur.y), ur, bl, float2(ur.x, bl.y));
    }

    void DrawLine(const ShaderState& ss, float2 a, float2 b) const
    {
        float2 v[] = { a, b };
        UseProgram(ss, v);
        ss.DrawArrays(GL_LINES, 2);
        UnuseProgram();
    }

    void DrawLineTri(const ShaderState& ss, float2 a, float2 b, float2 c) const
    {
        float2 v[] = { a, b, c };
        static const ushort i[] = {0, 1, 1, 2, 2, 0};
        UseProgram(ss, v);
        ss.DrawElements(GL_LINES, 6, i);
        UnuseProgram();
    }


    void DrawTri(const ShaderState& ss, float2 a, float2 b, float2 c) const
    {
        float2 v[] = { a, b, c };
        UseProgram(ss, v);
        ss.DrawArrays(GL_TRIANGLES, 3);
        UnuseProgram();
    }

    void DrawGrid(const ShaderState &ss, double width, double3 first, double3 last) const;

    void DrawLineCircle(const ShaderState& ss, float radius, uint numVerts=32) const;
    void DrawCircle(const ShaderState& ss, float radius, uint numVerts=32) const;
};

struct ShaderUColor final : public ShaderPosBase, public ShaderBase<ShaderUColor> {

    uint m_colorSlot;
        
    void LoadTheProgram()
    {
        LoadProgram("ShaderUColor",
                    "varying vec4 DestinationColor;\n"
                    , 
                    "uniform vec4 SourceColor;\n"
                    "void main(void) {\n"
                    "    DestinationColor = SourceColor;\n"
                    "    gl_Position = Transform * Position;\n"
                    "}\n"
                    ,
                    "void main(void) {\n"
                    "    gl_FragColor = DestinationColor;\n"
                    "}\n"
            );
        m_colorSlot = getUniformLocation("SourceColor");
    }
    void UseProgram(const ShaderState& ss, const float2* ptr) const
    {
        UseProgramBase(ss, ptr, (float2*)NULL);
        float4 c = abgr2rgbaf(ss.uColor);
        glUniform4fv(m_colorSlot, 1, &c[0]);
        glReportError();
    }

    void UseProgram(const ShaderState& ss, const VertexPos* ptr, const VertexPos* base) const
    {
        UseProgramBase(ss, &ptr->pos, base);
        float4 c = abgr2rgbaf(ss.uColor);
        glUniform4fv(m_colorSlot, 1, &c[0]);
        glReportError();
    }
};

struct ShaderIridescent final : public ShaderProgramBase, public ShaderBase<ShaderIridescent> {

    int SourceColor0;
    int SourceColor1;
    int TimeA;

    void LoadTheProgram()
    {
        LoadProgram("ShaderIridescent");
        GET_ATTR_LOC(SourceColor0);
        GET_ATTR_LOC(SourceColor1);
        GET_ATTR_LOC(TimeA);
    }

    void UseProgram(const ShaderState& input, const VertexPos2ColorTime* ptr, const VertexPos2ColorTime* base) const
    {
        UseProgramBase(input, &ptr->pos, base);
        vertexAttribPointer(SourceColor0, &ptr->color, base);
        vertexAttribPointer(SourceColor1, &ptr->color1, base);
        vertexAttribPointer(TimeA, &ptr->time, base);
    }
};

struct ShaderResource : public ShaderProgramBase, public ShaderBase<ShaderResource> {

    int SourceColor0, SourceColor1, Radius;
    int ToPixels;
    
    void LoadTheProgram()
    {
        LoadProgram("ShaderResource");
        GET_ATTR_LOC(SourceColor0);
        GET_ATTR_LOC(SourceColor1);
        GET_ATTR_LOC(Radius);
        GET_UNIF_LOC(ToPixels);
    }

    mutable float pointsToPixels = 1.f;

    void UseProgram(const ShaderState& input, const VertexPos2ColorTime* ptr, const VertexPos2ColorTime* base) const
    {
        UseProgramBase(input, &ptr->pos, base);
        vertexAttribPointer(SourceColor0, &ptr->color, base);
        vertexAttribPointer(SourceColor1, &ptr->color1, base);
        vertexAttribPointer(Radius, &ptr->time, base);
        glUniform1f(ToPixels, pointsToPixels);
    }
};

struct ShaderWormhole : public ShaderProgramBase, public ShaderBase<ShaderWormhole> {

    int SourceColor0, SourceColor1, TexCoord;
    
    void LoadTheProgram()
    {
        LoadProgram("ShaderWormhole");
        GET_ATTR_LOC(TexCoord);
    }

    void UseProgram(const ShaderState& input, const VertexPos2ColorTex* ptr, const VertexPos2ColorTex* base) const
    {
        UseProgramBase(input, &ptr->pos, base);
        vertexAttribPointer(TexCoord, &ptr->tex, base);
    }
};


struct ShaderTextureBase : public ShaderProgramBase {

    void bindTextureDrawElements(const ShaderState &ss, const GLTexture &texture,
                                 VertexPosTex *vtx, int icount, const uint *idxs) const;
    
    virtual void UseProgram(const ShaderState &ss, const VertexPosTex *ptr, const GLTexture& ot) const = 0;

    // a b
    // d c
    void DrawQuad(const ShaderState& ss, const GLTexture& texture,
                  float2 a, float2 b, float2 c, float2 d) const;

    void DrawRectCorners(const ShaderState &ss, const GLTexture& texture, float2 a, float2 b) const
    {
        float2 bl(min(a.x, b.x), min(a.y, b.y));
        float2 ur(max(a.x, b.x), max(a.y, b.y));
        DrawQuad(ss, texture, float2(bl.x, ur.y), ur, float2(ur.x, bl.y), bl);
    }

    void DrawRect(const ShaderState &ss, const GLTexture& texture, float2 pos, float2 rad) const
    {
        DrawRectCorners(ss, texture, pos - rad, pos + rad);
    }

    void DrawRectScale(const ShaderState &ss, const GLTexture& texture,
                       float2 scale, float2 pos, float2 rad) const;

    void DrawRectWidth(const ShaderState &ss, const GLTexture& texture, float2 pos, float width) const
    {
        float2 rad(width, width * texture.size().y / texture.size().x);
        DrawRect(ss, texture, pos, rad);
    }

    void DrawButton(const ShaderState &ss, const GLTexture& texture, float2 pos, float2 r) const;
};

struct ShaderTexture1 : public ShaderTextureBase {

    uint m_uTexture;
    uint m_uColorSlot;
    uint m_aTexCoords;

    void onLoad()
    {
        m_uTexture = getUniformLocation("texture1");
        m_uColorSlot = getUniformLocation("SourceColor");
        m_aTexCoords = getAttribLocation("SourceTexCoord");
    }

    void UseProgram(const ShaderState &ss, const VertexPosTex *ptr, const GLTexture &ot) const override
    {
        UseProgram(ss, ptr, (const VertexPosTex*) NULL);
    }

    template <typename Vtx>
    void UseProgram(const ShaderState &ss, const Vtx *ptr, const Vtx *base) const
    {
        UseProgramBase(ss, &ptr->pos, base);

        vertexAttribPointer(m_aTexCoords, &ptr->tex, base);
        glUniform1i(m_uTexture, 0);

        float4 c = abgr2rgbaf(ss.uColor);
        glUniform4fv(m_uColorSlot, 1, &c[0]); 
    }
};

struct ShaderTexture final : public ShaderTexture1, public ShaderBase<ShaderTexture>
{
    void LoadTheProgram() override
    {
        LoadProgram("ShaderTexture");
        onLoad();
    }
};

struct ShaderText final : public ShaderTexture1, public ShaderBase<ShaderText>
{
    void LoadTheProgram() override
    {
        LoadProgram("ShaderText",
                    "varying vec2 DestTexCoord;\n"
                    "varying vec4 DestColor;\n"
                    ,
                    "attribute vec2 SourceTexCoord;\n"
                    "uniform vec4 SourceColor;\n"
                    "void main(void) {\n"
                    "    DestTexCoord = SourceTexCoord;\n"
                    "    DestColor    = SourceColor;\n"
                    "    gl_Position  = Transform * Position;\n"
                    "}"
                    ,
                    "uniform sampler2D texture1;\n"
                    "void main(void) {\n"
                    "    vec2 texCoord = DestTexCoord;\n"
                    "    gl_FragColor = DestColor * texture2D(texture1, texCoord);\n"
                    "}");
        onLoad();
    }
};

struct ShaderTextureWarp final : public ShaderTextureBase, public ShaderBase<ShaderTextureWarp> {

    GLuint texture1, warpTex, SourceColor, SourceTexCoord, camWorldPos, camWorldSize;

    mutable float2 camPos;
    mutable float2 camSize;
    
    void LoadTheProgram() override
    { 
        LoadProgram("ShaderTextureWarp");
        GET_UNIF_LOC(texture1);
        GET_UNIF_LOC(warpTex);
        GET_UNIF_LOC(SourceColor);
        GET_UNIF_LOC(camWorldPos);
        GET_UNIF_LOC(camWorldSize);
        GET_ATTR_LOC(SourceTexCoord);
    }

    void UseProgram(const ShaderState &ss, const VertexPosTex *ptr, const GLTexture &ot) const override
    {
        UseProgram(ss, ptr, (const VertexPosTex*) NULL);
    }

    template <typename Vtx>
    void UseProgram(const ShaderState &ss, const Vtx *ptr, const Vtx *base) const
    {
        UseProgramBase(ss, &ptr->pos, base);

        vertexAttribPointer(SourceTexCoord, &ptr->tex, base);
        glUniform1i(texture1, 0);
        glUniform1i(warpTex, 1);
        glUniform2fv(camWorldPos, 1, &camPos[0]);
        glUniform2fv(camWorldSize, 1, &camSize[0]);

        float4 c = abgr2rgbaf(ss.uColor);
        glUniform4fv(SourceColor, 1, &c[0]); 
    }
};

struct ShaderTextureHSV final : public ShaderTextureBase, public ShaderBase<ShaderTextureHSV> {

    uint m_uTexture;
    uint m_uColorSlot;
    uint m_aTexCoords;

    void LoadTheProgram() override
    {
        LoadProgram("ShaderTextureHSV");
        m_uTexture = getUniformLocation("texture1");
        m_uColorSlot = getUniformLocation("SourceColor");
        m_aTexCoords = getAttribLocation("SourceTexCoord");
    }

    void UseProgram(const ShaderState &ss, const VertexPosTex *ptr, const GLTexture &ot) const override
    {
        UseProgram(ss, ptr, (const VertexPosTex*) NULL);
    }

    template <typename Vtx>
    void UseProgram(const ShaderState &ss, const Vtx *ptr, const Vtx *base) const
    {
        UseProgramBase(ss, &ptr->pos, base);

        vertexAttribPointer(m_aTexCoords, &ptr->tex, base);
        glUniform1i(m_uTexture, 0);

        float4 c = abgr2rgbaf(ss.uColor);
        glUniform4fv(m_uColorSlot, 1, &c[0]); 
    }
};

struct ShaderTonemapDither final : public ShaderTextureBase, public ShaderBase<ShaderTonemapDither> {

    uint texture1;
    uint dithertex;
    uint SourceTexCoord;

    void LoadTheProgram();

    void UseProgram(const ShaderState &ss, const VertexPosTex *ptr, const GLTexture &ot) const;
};

struct ShaderTonemap final : public ShaderTextureBase, public ShaderBase<ShaderTonemap> {

    uint texture1;
    uint SourceTexCoord;

    void LoadTheProgram();
    void UseProgram(const ShaderState &ss, const VertexPosTex *ptr, const GLTexture &ot) const;
};

// seperable gaussian blur
struct ShaderBlur final : public ShaderTextureBase {

private:
    uint usourceTex;
    uint asourceTexCoord;
    uint uoffsets;
    
    mutable int            dimension = 0;
    mutable vector<float2> offsets;

    int samples = 0;
    int scale = 0;

    float getBlurOffset(int sample) const;
    void LoadTheProgram() { LoadShader(samples, scale); }

public:

    void LoadShader(int smpls, int scl);

    void UseProgram(const ShaderState &ss, const VertexPosTex *ptr, const GLTexture &ot) const;

    void setDimension(uint dim) const 
    {
        dimension = dim; 
    }

    // radius is points of blur radius (not pixels)
    static const ShaderBlur &instance(int radius);
};

struct ShaderColor final : public ShaderProgramBase, public ShaderBase<ShaderColor> {

    int m_colorSlot;

    void LoadTheProgram()
    {
        LoadProgram("ShaderColor",
                    //"#extension GL_EXT_gpu_shader4 : require\n"
                    //"flat varying vec4 DestinationColor;\n"
                    "varying vec4 DestinationColor;\n"
                    ,
                    "attribute vec4 SourceColor;\n"
                    "void main(void) {\n"
                    "    DestinationColor = SourceColor;\n"
                    "    gl_Position = Transform * Position;\n"
                    "}\n"
                    ,
                    "void main(void) {\n"
                    "    gl_FragColor = DestinationColor;\n"
                    "}\n"
                    );
        m_colorSlot = getAttribLocation("SourceColor");
    }

    template <typename Vtx>
    void UseProgram(const ShaderState&ss, const Vtx* ptr, const Vtx* base) const
    {
        UseProgramBase(ss, &ptr->pos, base);
        vertexAttribPointer(m_colorSlot, &ptr->color, base);
    }
};

template <typename T, typename L>
void MeshPair<T, L>::Handle::Draw(const ShaderState& ss)
{
    mp.Draw(ss, ShaderColor::instance(), ShaderColor::instance());
}

struct ShaderColorDither final : public ShaderProgramBase, public ShaderBase<ShaderColorDither> {

    int SourceColor, dithertex;

    void LoadTheProgram()
    {
        LoadProgram("ShaderColorDither");
        GET_ATTR_LOC(SourceColor);
        GET_UNIF_LOC(dithertex);
    }

    template <typename Vtx>
    void UseProgram(const ShaderState&ss, const Vtx* ptr, const Vtx* base) const
    {
        UseProgramBase(ss, &ptr->pos, base);
        vertexAttribPointer(SourceColor, &ptr->color, base);
        getDitherTex().BindTexture(0);
        glUniform1i(dithertex, 0);
    }
};

struct ShaderHsv final : public ShaderProgramBase, public ShaderBase<ShaderHsv> {

    int m_colorSlot;

    void LoadTheProgram()
    {
        LoadProgram("ShaderHsv");
        m_colorSlot = getAttribLocation("ColorHSVA");
    }

    template <typename Vtx>
    void UseProgram(const ShaderState&ss, const Vtx* ptr, const Vtx* base) const
    {
        UseProgramBase(ss, &ptr->pos, base);
        vertexAttribPointer(m_colorSlot, &ptr->color, base);
    }
};

struct ShaderColorLuma final : public ShaderProgramBase, public ShaderBase<ShaderColorLuma> {

    int SourceColor, Luma;

    void LoadTheProgram();

    template <typename Vtx>
    void UseProgram(const ShaderState&ss, const Vtx* ptr, const Vtx* base) const
    {
        UseProgramBase(ss, &ptr->pos, base);
        vertexAttribPointer(SourceColor, &ptr->color, base);
        vertexAttribPointer(Luma, &ptr->luma, base);
    }
};

#endif // SHADERS_H
