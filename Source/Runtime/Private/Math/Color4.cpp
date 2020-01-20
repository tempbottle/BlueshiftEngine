// Copyright(c) 2017 POLYGONTEK
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Precompiled.h"
#include "Math/Math.h"

BE_NAMESPACE_BEGIN

const Color4 Color4::zero       = Color4::FromRGBA8888(  0,   0,   0,   0);
const Color4 Color4::black      = Color4::FromRGBA8888(  0,   0,   0, 255);
const Color4 Color4::white      = Color4::FromRGBA8888(255, 255, 255, 255);
const Color4 Color4::red        = Color4::FromRGBA8888(255,   0,   0, 255);
const Color4 Color4::green      = Color4::FromRGBA8888(  0, 255,   0, 255);
const Color4 Color4::blue       = Color4::FromRGBA8888(  0,   0, 255, 255);
const Color4 Color4::yellow     = Color4::FromRGBA8888(255, 255,   0, 255);
const Color4 Color4::cyan       = Color4::FromRGBA8888(  0, 255, 255, 255);
const Color4 Color4::magenta    = Color4::FromRGBA8888(255,   0, 255, 255);
const Color4 Color4::orange     = Color4::FromRGBA8888(255, 127,   0, 255);
const Color4 Color4::pink       = Color4::FromRGBA8888(255,   0, 127, 255);
const Color4 Color4::lawn       = Color4::FromRGBA8888(127, 255,   0, 255);
const Color4 Color4::mint       = Color4::FromRGBA8888(  0, 255, 127, 255);
const Color4 Color4::violet     = Color4::FromRGBA8888(127, 127, 255, 255);
const Color4 Color4::aquamarine = Color4::FromRGBA8888(112, 219, 147, 255);
const Color4 Color4::grey       = Color4::FromRGBA8888(192, 192, 129, 255);
const Color4 Color4::darkGrey   = Color4::FromRGBA8888( 84,  84,  84, 255);
const Color4 Color4::darkBrown  = Color4::FromRGBA8888( 92,  64,  51, 255);

void Color4::Clip(bool clipAlpha) {
    Clamp01(r);
    Clamp01(g);
    Clamp01(b);

    if (clipAlpha) {
        Clamp01(a);
    }
}

void Color4::Invert(bool invertAlpha) {
    r = 1.0f - r;
    g = 1.0f - g;
    b = 1.0f - b;

    if (invertAlpha) {
        a = 1.0f - a;
    }
}

Color4 &Color4::Grayscale() {
    r = r * 0.299f;
    g = g * 0.587f;
    b = b * 0.114f;

    return *this;
}

Color4 Color4::FromRGBA8888(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    float invNorm = 1.0f / 255.0f;
    return Color4(r * invNorm, g * invNorm, b * invNorm, a * invNorm);
}

uint32_t Color4::ToUInt32() const {
    uint32_t ur = (uint32_t)Clamp(((int)(r * 255.0f)), 0, 255);
    uint32_t ug = (uint32_t)Clamp(((int)(g * 255.0f)), 0, 255);
    uint32_t ub = (uint32_t)Clamp(((int)(b * 255.0f)), 0, 255);
    uint32_t ua = (uint32_t)Clamp(((int)(a * 255.0f)), 0, 255);
    return (ua << 24) | (ub << 16) | (ug << 8) | ur;
}

Color4 Color4::FromString(const char *str) {
    Color4 v;
    sscanf(str, "%f %f %f %f", &v.r, &v.g, &v.b, &v.a);
    return v;
}

BE_NAMESPACE_END
