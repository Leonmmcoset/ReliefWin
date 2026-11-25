/*
 * PROJECT:     ReliefMath API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Exercise native renderer process, storage jobs and cancellation
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "../../reliefmath/process.h"
#include "../../reliefmath/protocol.h"
#include "../../reliefmath/jobs.h"
#include "../../reliefmath/recovery.h"
#include "../../reliefmath/runtime.h"
#include "../../platform/posix.h"
#include <stdio.h>
#include <string.h>
static unsigned checks;
#define CHECK(value) do { ++checks; if (!(value)) { fprintf(stderr, "FAIL %s:%d: %s\nMath error: code=%u message=%s range=%zu:%zu\n", __FILE__, __LINE__, #value, unsigned(error.code), error.message.c_str(), error.begin, error.end); fflush(stderr); return 1; } } while (0)
static bool render_wait(rm::RenderClient &client, rm::Rendered &result, rm::Error &error) {
    ULONGLONG started = GetTickCount64();
    do {
        client.pump();
        if (client.take(result, error)) return true;
        Sleep(1);
    } while (GetTickCount64() - started < 15000);
    return false;
}
static bool job_wait(rm::IoJob &job, rm::IoResult &result) {
    ULONGLONG started = GetTickCount64();
    do {
        if (job.take(result)) return true;
        Sleep(1);
    } while (GetTickCount64() - started < 10000);
    return false;
}
static int public_volume(const char *root) {
    rm::Error error;
    char path[4096], directory[4096];
    int length = snprintf(path, sizeof(path), "%s/公共-ReliefMath-document-name-that-needs-more-directory-slots.rmath", root);
    CHECK(length > 0 && size_t(length) < sizeof(path));
    length = snprintf(directory, sizeof(directory), "%s/ReliefMath-public-recovery", root);
    CHECK(length > 0 && size_t(length) < sizeof(directory));
    ro::unlink(path);
    rm::Document document;
    rm::FormulaId id = 0;
    CHECK(rm::add_formula(document, "FAT公式", "x^2", id, error));
    rm::FileStamp absent, saved;
    CHECK(rm::atomic_save(path, document, absent, saved, error));
    ro::Stat metadata;
    CHECK(ro::stat(path, &metadata) == 0 && S_ISREG(metadata.st_mode));
    CHECK(metadata.st_uid == UINT64_MAX && (metadata.st_mode & 0777) == 0666);
    int descriptor = ro::open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    CHECK(descriptor >= 0);
    CHECK(ro::fchmod(descriptor, 0600) < 0 && errno == ENOSYS);
    CHECK(ro::close(descriptor) == 0);
    rm::Document loaded;
    rm::FileStamp observed;
    CHECK(rm::read_document(path, loaded, observed, error));
    CHECK(loaded.formulas.size() == 1 && loaded.formulas[0].source == "x^2");
    CHECK(rm::same_stamp(saved, observed));
    CHECK(rm::set_formula_source(document, id, "x^3", error));
    rm::FileStamp replaced;
    CHECK(rm::atomic_save(path, document, saved, replaced, error));
    CHECK(!rm::atomic_save(path, document, saved, observed, error) && error.code == rm::Code::Conflict);
    CHECK(rm::read_document(path, loaded, observed, error));
    CHECK(loaded.formulas[0].source == "x^3" && rm::same_stamp(replaced, observed));
    CHECK(ro::mkdir(directory, 0700) == 0 || errno == EEXIST);
    CHECK(ro::stat(directory, &metadata) == 0 && S_ISDIR(metadata.st_mode));
    CHECK(metadata.st_uid == UINT64_MAX && (metadata.st_mode & 0777) == 0777);
    rm::RecoveryStore recovery;
    CHECK(!recovery.open(directory, error) && error.code == rm::Code::Format);
    wchar_t wide_directory[4096];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, directory, -1, wide_directory, 4096) > 0);
    CHECK(RemoveDirectoryW(wide_directory));
    CHECK(ro::unlink(path) == 0);
    printf("ReliefMath public volume save/read/conflict/metadata and private recovery rejection: %u checks passed\n", checks);
    return 0;
}
int main(int argc, char **argv) {
    if (argc == 2 && !strcmp(argv[1], "--render-worker")) { Sleep(INFINITE); return 2; }
    if (argc == 3 && !strcmp(argv[1], "--public-volume")) return public_volume(argv[2]);
    rm::Error error;
    rm::RenderClient client;
    printf("Renderer: %s\nResources: %s\nText font: %s\n", rm::worker_path(), rm::resource_path(), rm::text_font_path());
    fflush(stdout);
    CHECK(client.start(rm::worker_path(), error));
    rm::Snapshot snapshot;
    snapshot.request_id = 37;
    snapshot.formula.id = 12;
    snapshot.formula.generation = 8;
    snapshot.artifacts = rm::WantRaster | rm::WantPng | rm::WantSvg;
    CHECK(snapshot.formula.source.set("\\frac{x^2+1}{\\sqrt{y}}+\\text{中文}"));
    CHECK(client.submit(snapshot, error));
    rm::Rendered rendered;
    CHECK(render_wait(client, rendered, error));
    CHECK(error.code == rm::Code::None);
    CHECK(rendered.request_id == 37 && rendered.formula_id == 12 && rendered.generation == 8);
    CHECK(rendered.png.size() > 24 && !memcmp(rendered.png.data(), "\x89PNG\r\n\x1a\n", 8));
    CHECK(rendered.svg.size() > 100 && !memcmp(rendered.svg.data(), "<svg ", 5));
    CHECK(rendered.raster.rgba.size() == size_t(rendered.raster.width) * rendered.raster.height * 4);
    CHECK(!client.busy());
    ++snapshot.request_id;
    CHECK(client.submit(snapshot, error));
    client.cancel();
    CHECK(client.take(rendered, error) && error.code == rm::Code::Cancelled);
    ++snapshot.request_id;
    CHECK(client.submit(snapshot, error));
    CHECK(render_wait(client, rendered, error) && error.code == rm::Code::None);
    CHECK(rendered.request_id == snapshot.request_id);
    puts("ReliefMath native renderer/protocol/PNG/SVG/cancel/restart passed"); fflush(stdout);
    char path[4096];
    const char *home = ro::getenv("HOME");
    CHECK(home && snprintf(path, sizeof(path), "%s/公式-ReliefMath-test.rmath", home) > 0);
    ro::unlink(path);
    rm::Document document;
    rm::FormulaId id = 0;
    CHECK(rm::add_formula(document, "公式", "x^2", id, error));
    rm::IoJob job;
    rm::FileStamp absent;
    CHECK(job.save(path, document, absent, false, 73, error));
    CHECK(rm::set_formula_source(document, id, "newer", error));
    rm::IoResult saved;
    CHECK(job_wait(job, saved) && saved.success && saved.session == 73);
    CHECK(saved.document.formulas[0].source == "x^2");
    CHECK(job.load(path, 74, error));
    rm::IoResult loaded;
    CHECK(job_wait(job, loaded) && loaded.success && loaded.document.formulas[0].source == "x^2");
    CHECK(job.save(path, document, saved.stamp, false, 75, error));
    rm::IoResult overwritten;
    CHECK(job_wait(job, overwritten));
    error = overwritten.error;
    CHECK(overwritten.success && overwritten.session == 75);
    CHECK(job.load(path, 76, error));
    CHECK(job_wait(job, loaded) && loaded.success && loaded.document.formulas[0].source == "newer" &&
        rm::same_stamp(overwritten.stamp, loaded.stamp));
    CHECK(job.save(path, document, absent, false, 77, error));
    rm::IoResult collision;
    CHECK(job_wait(job, collision) && !collision.success && collision.error.code == rm::Code::Conflict);
    CHECK(ro::unlink(path) == 0);
    puts("ReliefMath native Unicode file jobs/snapshot/conflict passed"); fflush(stdout);
    if (argc == 2 && !strcmp(argv[1], "--timeout")) {
        wchar_t self_wide[32768];
        char self[32768];
        CHECK(GetModuleFileNameW(nullptr, self_wide, 32768) > 0);
        CHECK(WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, self_wide, -1, self, sizeof(self), nullptr, nullptr) > 0);
        rm::RenderClient stalled;
        CHECK(stalled.start(self, error));
        char large_source[60000];
        memset(large_source, 'x', sizeof(large_source));
        CHECK(snapshot.formula.source.set(large_source, sizeof(large_source)));
        CHECK(stalled.submit(snapshot, error));
        for (unsigned index = 0; index < 12; ++index) stalled.pump();
        stalled.cancel();
        CHECK(stalled.take(rendered, error) && error.code == rm::Code::Cancelled);
        CHECK(snapshot.formula.source.set("x"));
        ++snapshot.request_id;
        CHECK(stalled.submit(snapshot, error));
        CHECK(render_wait(stalled, rendered, error) && error.code == rm::Code::Timeout);
        CHECK(!stalled.busy());
        puts("ReliefMath native worker timeout passed"); fflush(stdout);
    }
    printf("ReliefMath native tests: %u checks passed\n", checks);
    return 0;
}
