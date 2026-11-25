/*
 * PROJECT:     ReliefMath
 * LICENSE:     Apache-2.0 (https://spdx.org/licenses/Apache-2.0)
 * PURPOSE:     Resolve installed formula renderer and font resources
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#ifndef RELIEFMATH_RUNTIME_H
#define RELIEFMATH_RUNTIME_H
namespace rm {
const char *worker_path();
const char *resource_path();
const char *text_font_path();
void open_directory(const char *);
struct Raster;
bool copy_raster(const Raster &, void *);
}
#endif
