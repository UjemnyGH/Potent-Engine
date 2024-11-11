#pragma once
#ifndef _POTENT_ENGINE_UI_CORE_
#define _POTENT_ENGINE_UI_CORE_

#include "../Renderer/engineRendererData.h"
#include <sstream>

namespace potent {
    char buffer[24 << 20];
    unsigned char screen[20][79];

    class UITextData : public RenderObjectData {
    private:
        std::stringstream mText;
        std::string mFontPath;
        std::vector<uint8_t> mLoadedFont;
        stbtt_fontinfo mStbFontInfo;
        float mScale;
        int mAscent;
        int mBaseline;
    
    public:
        void openFont(std::string font = "c:/windows/fonts/arialbd.ttf") {
            mFontPath = font;

            std::ifstream fontFile(mFontPath, std::ios::binary | std::ios::ate);

            std::size_t length = fontFile.tellg();
            fontFile.seekg(0, std::ios::beg);

            mLoadedFont.clear();
            mLoadedFont.resize(length);

            fontFile.read((char*)mLoadedFont.data(), mLoadedFont.size());

            fontFile.close();

            stbtt_InitFont(&mStbFontInfo, mLoadedFont.data(), 0);

            mScale = stbtt_ScaleForPixelHeight(&mStbFontInfo, 15);
            stbtt_GetFontVMetrics(&mStbFontInfo, &mAscent, 0, 0);
            mBaseline = (int)(mAscent * mScale);
        }

        /*void makeText(std::string text) {

        }

        void addText() {

        }*/

        template<class T>
        std::iostream operator<<(T& value) {
            stringstream << value;
            
            return mText;
        }

        void bakeText() {
            std::string buffer;
            mText >> buffer;
            
            std::uint32_t index = 0;

            while (buffer[index]) {
                int advance, lsb, x0, y0, x1, y1;
                float x_shift = xpos - (float)floor(xpos);
                stbtt_GetCodepointHMetrics(&mStbFontInfo, buffer[index], &advance, &lsb);
                stbtt_GetCodepointBitmapBoxSubpixel(&mStbFontInfo, buffer[index], mScale, mScale, x_shift, 0, &x0, &y0, &x1, &y1);
                stbtt_MakeCodepointBitmapSubpixel(&mStbFontInfo, &screen[mBaseline + y0][(int)xpos + x0], x1 - x0, y1 - y0, 79, mScale, mScale, x_shift, 0, buffer[index]);
                // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
                // because this API is really for baking character bitmaps into textures. if you want to render
                // a sequence of characters, you really need to render each bitmap to a temp buffer, then
                // "alpha blend" that into the working buffer
                xpos += (advance * mScale);
                if (text[ch + 1])
                    xpos += scale * stbtt_GetCodepointKernAdvance(&mStbFontInfo, buffer[index], buffer[index + 1]);

                index++;
            }
        }
    };

    int main(int arg, char** argv)
    {
        stbtt_fontinfo font;
        int i, j, ascent, baseline, ch = 0;
        float scale, xpos = 2; // leave a little padding in case the character extends left
        char* text = "Heljo World!"; // intentionally misspelled to show 'lj' brokenness

        fread(buffer, 1, 1000000, fopen("c:/windows/fonts/arialbd.ttf", "rb"));
        stbtt_InitFont(&font, buffer, 0);

        scale = stbtt_ScaleForPixelHeight(&font, 15);
        stbtt_GetFontVMetrics(&font, &ascent, 0, 0);
        baseline = (int)(ascent * scale);

        while (text[ch]) {
            int advance, lsb, x0, y0, x1, y1;
            float x_shift = xpos - (float)floor(xpos);
            stbtt_GetCodepointHMetrics(&font, text[ch], &advance, &lsb);
            stbtt_GetCodepointBitmapBoxSubpixel(&font, text[ch], scale, scale, x_shift, 0, &x0, &y0, &x1, &y1);
            stbtt_MakeCodepointBitmapSubpixel(&font, &screen[baseline + y0][(int)xpos + x0], x1 - x0, y1 - y0, 79, scale, scale, x_shift, 0, text[ch]);
            // note that this stomps the old data, so where character boxes overlap (e.g. 'lj') it's wrong
            // because this API is really for baking character bitmaps into textures. if you want to render
            // a sequence of characters, you really need to render each bitmap to a temp buffer, then
            // "alpha blend" that into the working buffer
            xpos += (advance * scale);
            if (text[ch + 1])
                xpos += scale * stbtt_GetCodepointKernAdvance(&font, text[ch], text[ch + 1]);
            ++ch;
        }

        for (j = 0; j < 20; ++j) {
            for (i = 0; i < 78; ++i)
                putchar(" .:ioVM@"[screen[j][i] >> 5]);
            putchar('\n');
        }

        return 0;
    }
}

#endif