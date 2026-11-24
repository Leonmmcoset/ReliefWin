/*
 * PROJECT:     ReliefWin DirectX Diagnostic Tool
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Rotating ReliefWin mark drawn by the Direct3D tests
 * COPYRIGHT:   Copyright 2026 Ahmed ARIF <arif.ing@outlook.com>
 */

#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <windef.h>

#include "d3dmark.h"

#define MARK_CAMERA_DISTANCE 4.0f

typedef struct
{
    FLOAT x;
    FLOAT y;
} MARK_POINT;

typedef struct
{
    MARK_POINT Point[3];
    DWORD Color;
} MARK_TRIANGLE;

#include "d3dmark_mesh.h"

UINT D3DTestBuildMark(PD3DTEST_VERTEX Vertices, DWORD Elapsed, UINT Width, UINT Height)
{
    FLOAT Angle = (FLOAT)Elapsed * 0.0015f;
    FLOAT CosX = (FLOAT)cos(Angle * 0.7f);
    FLOAT SinX = (FLOAT)sin(Angle * 0.7f);
    FLOAT CosY = (FLOAT)cos(Angle);
    FLOAT SinY = (FLOAT)sin(Angle);
    FLOAT Focal = (FLOAT)(Width < Height ? Width : Height) * 1.2f;
    UINT Triangle, Corner, Count = 0;

    assert(Vertices != NULL);
    assert(sizeof(MarkMesh) / sizeof(MarkMesh[0]) * 3 == D3DTEST_MARK_VERTEX_COUNT);
    if (!Width || !Height)
        return 0;

    for (Triangle = 0; Triangle < sizeof(MarkMesh) / sizeof(MarkMesh[0]); ++Triangle)
    {
        for (Corner = 0; Corner < 3; ++Corner)
        {
            const MARK_POINT *Point = &MarkMesh[Triangle].Point[Corner];
            PD3DTEST_VERTEX Vertex = &Vertices[Count++];
            FLOAT Turned = -Point->x * SinY;
            FLOAT x = Point->x * CosY;
            FLOAT y = Point->y * CosX - Turned * SinX;
            FLOAT Depth = Point->y * SinX + Turned * CosX + MARK_CAMERA_DISTANCE;

            Vertex->x = (FLOAT)Width * 0.5f + x * Focal / Depth;
            Vertex->y = (FLOAT)Height * 0.5f - y * Focal / Depth;
            Vertex->z = (Depth - 2.0f) * 0.25f;
            Vertex->rhw = 1.0f / Depth;
            Vertex->color = MarkMesh[Triangle].Color;
        }
    }
    assert(Count == D3DTEST_MARK_VERTEX_COUNT);
    return Count;
}

VOID D3DTestToClipSpace(PD3DTEST_CLIP_VERTEX Output, const D3DTEST_VERTEX *Input, UINT Count, UINT Width, UINT Height)
{
    UINT i;

    for (i = 0; i < Count; ++i)
    {
        Output[i].Position[0] = Input[i].x * 2.0f / (FLOAT)Width - 1.0f;
        Output[i].Position[1] = 1.0f - Input[i].y * 2.0f / (FLOAT)Height;
        Output[i].Position[2] = Input[i].z;
        Output[i].Position[3] = 1.0f;
        Output[i].Color[0] = (FLOAT)((Input[i].color >> 16) & 0xff) / 255.0f;
        Output[i].Color[1] = (FLOAT)((Input[i].color >> 8) & 0xff) / 255.0f;
        Output[i].Color[2] = (FLOAT)(Input[i].color & 0xff) / 255.0f;
        Output[i].Color[3] = (FLOAT)(Input[i].color >> 24) / 255.0f;
    }
}
