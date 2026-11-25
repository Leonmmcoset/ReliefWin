/*
 * PROJECT:     ReliefMath API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Validate formula paths, raster pixels, PNG encoding and limits
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include "worker/renderer.h"
#include "protocol.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CHECK(value) do { ++checks; if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)
int main(int argc, char **argv) {
    CHECK(argc == 2);
    rm::Renderer renderer;
    rm::Error error;
    CHECK(renderer.initialize(argv[1], error));
    const char *sources[] = {"x^2+y^2=z^2", "\\frac{a+b}{c+d}", "\\sqrt{x_1^2+x_2^2}", "\\sum_{i=1}^{n} i", "\\text{公式测试}", "\\int_0^1 x^2\\,dx"};
    for (const char *source : sources) {
        rm::Snapshot snapshot;
        snapshot.request_id = 73;
        snapshot.formula.id = 42;
        snapshot.formula.generation = 7;
        CHECK(snapshot.formula.source.set(source));
        snapshot.artifacts = rm::WantRaster | rm::WantSvg | rm::WantPng;
        rm::Rendered output;
        CHECK(renderer.render(snapshot, output, error));
        CHECK(output.request_id == 73 && output.formula_id == 42 && output.generation == 7);
        CHECK(output.raster.width > 0 && output.raster.height > 0);
        CHECK(output.raster.rgba.size() == size_t(output.raster.stride) * output.raster.height);
        CHECK(output.raster.stride == output.raster.width * 4);
        CHECK(output.png.size() > 24 && memcmp(output.png.data(), "\x89PNG\r\n\x1a\n", 8) == 0);
        CHECK(output.svg.size() > 100 && memcmp(output.svg.data(), "<svg ", 5) == 0);
        bool filled = false, transparent = false;
        for (size_t at = 3; at < output.raster.rgba.size(); at += 4) {
            filled |= output.raster.rgba.data()[at] != 0;
            transparent |= output.raster.rgba.data()[at] == 0;
        }
        CHECK(filled && transparent);
        CHECK(isfinite(output.baseline) && output.baseline > 0);
    }
    rm::Snapshot bad;
    CHECK(bad.formula.source.set("x"));
    bad.scale = INFINITY;
    rm::Rendered untouched;
    untouched.request_id = 987;
    CHECK(!renderer.render(bad, untouched, error) && error.code == rm::Code::Limit);
    CHECK(untouched.request_id == 987);
    bad.scale = 1;
    bad.artifacts = 0;
    CHECK(!renderer.render(bad, untouched, error) && error.code == rm::Code::Limit);
    CHECK(bad.formula.source.set("\\frac{"));
    bad.artifacts = rm::WantRaster;
    CHECK(!renderer.render(bad, untouched, error));
    printf("ReliefMath render: %u checks passed\n", checks);
    return 0;
}
