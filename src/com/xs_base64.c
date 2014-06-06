
/*
 * This file is part of the xs Library.
 *
 * Copyright (C) 2011 by Guoliang Xue <xueguoliang@gmail.com>
 *
 * The xs Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The xs Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the xs Library. If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef __cplusplus
extern "C"{
#endif
#include "xs.h"
uint8_t g_de_tab[] = 
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 0 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 8 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 16 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 24 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 32 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 40 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 48 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 56 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 64 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 72 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 80 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 88 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 96 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 104 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 112 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 120 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 136 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 152 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 168 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 184 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 200 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 208 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 216 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 232 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 240 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 248 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 256 */
};

uint8_t g_en_tab[] = "abcdefghijklmnopqrstuvwxyz"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "1234567890_+";
uint8_t g_base64_extra = 0;

static inline void xs_base64_init()
{
    uint32_t i;
    if(g_base64_extra == 0)
    {
        g_base64_extra = '.';
        for(i=0; i<sizeof(g_en_tab)-1; ++i) 
        {
            g_de_tab[g_en_tab[i]] = i;
        }
    }
}

int xs_base64_encode(uint8_t* src, int ssize, uint8_t** dst)
{
    uint8_t* s = src;
    uint8_t* d;
    int g = ssize / 3;
    int i = 0, j = 0;
    int dummy;

    xs_base64_init();

    *dst = d = xs_malloc(ssize / 3 * 4 + 5);

    for(i=0; i<g; ++i)
    {
        d[0] = ((s[0] & 0xfc) >> 2);
        d[1] = (((s[0] & 0x03) << 4) + ((s[1] & 0xf0) >> 4));
        d[2] = (((s[1] & 0x0f) << 2) + ((s[2] & 0xc0) >> 6));
        d[3] = (s[2] & 0x3f);

        for(j=0; j<4; ++j)
        {
            d[j] = g_en_tab[d[j]];
        }

        s+=3;
        d+=4;
    }

    dummy = ssize - (s-src);
    if(dummy > 0)
    {
        d[0] = ((s[0] & 0xfc) >> 2);
        d[1] = (((s[0] & 0x03) << 4) + ((s[1] & 0xf0) >> 4));
        d[2] = (((s[1] & 0x0f) << 2) + ((s[2] & 0xc0) >> 6));
        d[3] = (s[2] & 0x3f);

        for(j=0; j<4; ++j)
        {
            d[j] = g_en_tab[d[j]];
        }

        for(i=0; i<(3-dummy); ++i)
        {
            d[3-i] = g_base64_extra;
        }
        d += 4;
    }
    *d = 0;
    return d-*dst;
}

int xs_base64_decode(uint8_t* src, int ssize, uint8_t** dst)
{
    uint8_t* d;
    uint8_t* s = src;

    int g = ssize / 4;
    int i, j;
    int dummy = 0;
    if(src[ssize -1] == g_base64_extra)
    {
        ++dummy;
        if(src[ssize -2] == g_base64_extra)
            ++dummy;
    }

    *dst = d = xs_malloc(g*3+1);

    xs_base64_init();

    for(i=0; i<g; ++i)
    {
        for(j=0; j<4; ++j)
            s[j] = g_de_tab[s[j]]; 

        d[0] = (s[0] << 2) + ((s[1] & 0xf0) >> 4);
        d[1] = ((s[1] & 0xf) << 4) + ((s[2] & 0xfc)>> 2);
        d[2] = ((s[2] & 3) << 6) + s[3];

        s += 4;
        d += 3;
    }
    d -= dummy;
    *d = 0;
    return d-*dst;
}
#ifdef __cplusplus
}
#endif
