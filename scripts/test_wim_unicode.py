#!/usr/bin/env python3
"""
PROJECT:     ReliefWin WIM reader tests
LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
PURPOSE:     Exercise actual WIM Unicode conversion and metadata failure paths
COPYRIGHT:   Copyright 2026 ReliefWin contributors

Encoding expectations use Python's Unicode codecs, Unicode chapter 3 and RFC 3629.
https://www.unicode.org/versions/latest/core-spec/chapter-3/
https://www.rfc-editor.org/rfc/rfc3629
"""

import argparse
import json
import os
from pathlib import Path
import random
import re
import struct
import subprocess


ROOT = Path(__file__).resolve().parents[1]
SOURCE = "sdk/lib/libwim/wimcore.c"
FUNCTIONS = (
    "core_zero", "core_alloc", "core_free", "core_read_u32", "core_read_u16",
    "core_read_u64", "core_dentry_init", "core_dentry_free", "core_add_child",
    "core_utf16le_to_utf8", "core_deserialize_one_dentry", "core_deserialize_dentry_tree",
)


def function(source, name):
    match = re.search(r"(?m)^static [^\n]+\n" + name + r"\(", source)
    assert match, name
    start = source.index("{", match.end())
    depth = 1
    end = start + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[match.start():end]


HARNESS = r'''
#include "wimcore.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIMCORE_DENTRY_FIXED_SIZE 102u
static size_t checks, vector_index;
static void check(int condition, const char *expression, int line)
{
    checks++;
    if (!condition)
    {
        fprintf(stderr, "check failed at line %d, vector %zu: %s\n", line, vector_index, expression);
        exit(1);
    }
}
#define CHECK(value) check(!!(value), #value, __LINE__)
typedef struct { size_t size; uint64_t guard; } Allocation;
typedef struct { size_t calls, live, requested; long fail_after; } Allocator;
static void *allocate(size_t size, void *context)
{
    Allocator *state = context;
    Allocation *allocation;
    uint64_t guard = UINT64_C(0x7badc0de45af1903);
    state->calls++;
    state->requested = size;
    if (state->fail_after == 0)
        return NULL;
    if (state->fail_after > 0)
        state->fail_after--;
    CHECK(size <= 1048576);
    allocation = malloc(sizeof(*allocation) + size + sizeof(guard));
    CHECK(allocation != NULL);
    allocation->size = size;
    allocation->guard = guard;
    memcpy((unsigned char *)(allocation + 1) + size, &guard, sizeof(guard));
    state->live++;
    return allocation + 1;
}
static void release(void *pointer, void *context)
{
    Allocator *state = context;
    Allocation *allocation = (Allocation *)pointer - 1;
    uint64_t guard;
    memcpy(&guard, (unsigned char *)pointer + allocation->size, sizeof(guard));
    CHECK(allocation->guard == UINT64_C(0x7badc0de45af1903));
    CHECK(guard == allocation->guard && state->live != 0);
    state->live--;
    free(allocation);
}
static void initialize(WimCoreCtx *context, Allocator *state)
{
    memset(context, 0, sizeof(*context));
    memset(state, 0, sizeof(*state));
    state->fail_after = -1;
    context->Allocator.Alloc = allocate;
    context->Allocator.Free = release;
    context->Allocator.Context = state;
}
'''

TESTS = r'''
static void vectors(const char *path)
{
    FILE *input = fopen(path, "rb");
    uint32_t header[3];
    CHECK(input != NULL);
    while (fread(header, sizeof(header), 1, input) == 1)
    {
        WimCoreCtx context;
        Allocator state;
        unsigned char *source = malloc(header[0] ? header[0] : 1);
        char *expected = malloc(header[1] + 1), *actual = (char *)(uintptr_t)1;
        int status, matches;
        CHECK(source && expected);
        CHECK(fread(source, 1, header[0], input) == header[0]);
        CHECK(fread(expected, 1, header[1], input) == header[1]);
        expected[header[1]] = 0;
        initialize(&context, &state);
        status = core_utf16le_to_utf8(&context, (const uint16_t *)source, header[0] / 2, &actual);
        CHECK(status == (int32_t)header[2]);
        if (status == 0)
        {
            CHECK(actual != NULL);
            matches = strlen(actual) == header[1] && memcmp(actual, expected, header[1]) == 0;
            if (!matches)
            {
                fprintf(stderr, "UTF8 mismatch vector %zu: actual=", vector_index);
                for (size_t i = 0; i < strlen(actual); i++)
                    fprintf(stderr, "%02x", (unsigned char)actual[i]);
                fprintf(stderr, " expected=");
                for (size_t i = 0; i < header[1]; i++)
                    fprintf(stderr, "%02x", (unsigned char)expected[i]);
                fputc('\n', stderr);
            }
            core_free(&context, actual);
            free(source);
            free(expected);
            CHECK(matches);
        }
        else
        {
            CHECK(actual == NULL);
            free(source);
            free(expected);
        }
        CHECK(state.live == 0);
        vector_index++;
    }
    CHECK(feof(input) && !ferror(input));
    fclose(input);
}
static void arguments_and_allocation(void)
{
    WimCoreCtx context;
    Allocator state;
    uint16_t input[] = {0x0041};
    char *output = NULL;
    initialize(&context, &state);
    CHECK(core_utf16le_to_utf8(NULL, input, 1, &output) == -1);
    CHECK(core_utf16le_to_utf8(&context, input, 1, NULL) == -1);
    CHECK(state.calls == 0);
    CHECK(core_utf16le_to_utf8(&context, NULL, 1, &output) == -1 && !output);
    CHECK(state.calls == 0);
    CHECK(core_utf16le_to_utf8(&context, input, (SIZE_MAX - 1) / 3 + 1, &output) == -1 && !output);
    CHECK(state.calls == 0);
    CHECK(core_utf16le_to_utf8(&context, input, SIZE_MAX, &output) == -1 && !output);
    CHECK(state.calls == 0);
    state.fail_after = 0;
    CHECK(core_utf16le_to_utf8(&context, input, (SIZE_MAX - 1) / 3, &output) == -1 && !output);
    CHECK(state.calls == 1 && state.requested == ((SIZE_MAX - 1) / 3) * 3 + 1);
    CHECK(core_utf16le_to_utf8(&context, input, 1, &output) == -1 && !output);
    CHECK(state.calls == 2 && state.live == 0);
    state.fail_after = -1;
    CHECK(core_utf16le_to_utf8(&context, NULL, 0, &output) == 0 && output && !*output);
    core_free(&context, output);
    CHECK(state.live == 0);
}
static void store16(unsigned char *bytes, uint16_t value)
{
    bytes[0] = (unsigned char)value;
    bytes[1] = (unsigned char)(value >> 8);
}
static void store64(unsigned char *bytes, uint64_t value)
{
    for (size_t i = 0; i < 8; i++)
        bytes[i] = (unsigned char)(value >> (i * 8));
}
static size_t metadata(unsigned char *buffer, uint16_t first, uint16_t second, unsigned length)
{
    memset(buffer, 0, 256);
    buffer[0] = 8;
    store64(buffer + 8, 104);
    buffer[16] = WIM_FILE_ATTRIBUTE_DIRECTORY;
    store64(buffer + 24, 112);
    store64(buffer + 112, 112);
    store16(buffer + 212, (uint16_t)length);
    store16(buffer + 214, first);
    store16(buffer + 216, second);
    return 232;
}
static void metadata_paths(void)
{
    unsigned char buffer[256];
    WimCoreCtx context;
    Allocator state;
    WimDentry root, entry;
    uint64_t next;
    size_t length;
    int status;
    initialize(&context, &state);
    length = metadata(buffer, 0xD83D, 0xDE00, 4);
    memset(&root, 0, sizeof(root));
    CHECK(core_deserialize_dentry_tree(&context, buffer, length, &root) == 0);
    CHECK(root.child_count == 1 && !strcmp(root.children[0].name_utf8, "\xf0\x9f\x98\x80"));
    CHECK(root.children[0].name_utf16_len == 2);
    core_dentry_free(&context, &root);
    CHECK(state.live == 0);
    for (unsigned test = 0; test < 8; test++)
    {
        const unsigned lengths[] = {1, 3, 5, 7, 9, 11, 12, 65535};
        initialize(&context, &state);
        metadata(buffer, 0xD83D, 0xDE00, lengths[test]);
        memset(&entry, 0, sizeof(entry));
        status = core_deserialize_one_dentry(&context, buffer, length, 112, &entry, &next);
        CHECK(status == -1);
        core_dentry_free(&context, &entry);
        CHECK(state.live == 0 && state.calls == 0);
    }
    for (unsigned invalid = 0; invalid < 3; invalid++)
    {
        size_t retained;
        initialize(&context, &state);
        metadata(buffer, invalid == 2 ? 0xDC00 : 0xD800, invalid == 1 ? 0x0041 : 0, invalid == 1 ? 4 : 2);
        memset(&entry, 0, sizeof(entry));
        CHECK(core_deserialize_one_dentry(&context, buffer, length, 112, &entry, &next) == -1);
        retained = state.live;
        core_dentry_free(&context, &entry);
        CHECK(retained == 0 && state.live == 0);
        initialize(&context, &state);
        memset(&root, 0, sizeof(root));
        CHECK(core_deserialize_dentry_tree(&context, buffer, length, &root) == -1);
        core_dentry_free(&context, &root);
        CHECK(state.live == 0);
    }
    initialize(&context, &state);
    metadata(buffer, 0xD83D, 0xDE00, 4);
    store64(buffer + 112, UINT64_MAX);
    memset(&entry, 0, sizeof(entry));
    status = core_deserialize_one_dentry(&context, buffer, length, 112, &entry, &next);
    core_dentry_free(&context, &entry);
    CHECK(status == 0 && state.calls == 0 && state.live == 0);
    for (unsigned test = 0; test < 4; test++)
    {
        const uint64_t offsets[] = {UINT64_MAX, UINT64_MAX - 7, 232, 225};
        CHECK(core_deserialize_one_dentry(&context, buffer, length, offsets[test], &entry, &next) == 0);
        CHECK(state.calls == 0);
    }
    for (long fail = 0; fail < 4; fail++)
    {
        initialize(&context, &state);
        state.fail_after = fail;
        metadata(buffer, 0xD83D, 0xDE00, 4);
        memset(&root, 0, sizeof(root));
        CHECK(core_deserialize_dentry_tree(&context, buffer, length, &root) == -1);
        core_dentry_free(&context, &root);
        CHECK(state.live == 0);
    }
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    vectors(argv[1]);
    arguments_and_allocation();
    metadata_paths();
    printf("WIM Unicode: %zu checks, %zu codec vectors, 0 failures\n", checks, vector_index);
    return 0;
}
'''


def write_vectors(path):
    vectors = []

    def valid(text):
        vectors.append((text.encode("utf-16le"), text.split("\0", 1)[0].encode("utf-8"), 0))

    valid("😀")
    for text in ("", "\0", "ASCII", "安装 ReliefWin", "使用说明", "资料/😀/α.txt",
                 "A\0ignored", "a" * 255, "中" * 255, "😀" * 255):
        valid(text)
    for point in (1, 0x7F, 0x80, 0x7FF, 0x800, 0xD7FF, 0xE000, 0xFFFF,
                  0x10000, 0x1F600, 0x10FFFF):
        valid(chr(point))
    for high in (0xD800, 0xD83D, 0xDBFF):
        for low in (0xDC00, 0xDE00, 0xDFFF):
            text = struct.pack("<HH", high, low).decode("utf-16le")
            valid("prefix" + text + "后缀")
    for point in range(0xD800, 0xE000):
        vectors.append((struct.pack("<H", point), b"", -1))
    for units in ((0xD800, 0x41), (0xD800, 0), (0xD800, 0xD800),
                  (0xDC00, 0xD800), (0xDBFF, 0xE000), (0x41, 0xD800)):
        vectors.append((struct.pack("<" + "H" * len(units), *units), b"", -1))
    generator = random.Random(3629)
    for _ in range(1024):
        points = []
        for _ in range(generator.randrange(1, 12)):
            point = generator.randrange(1, 0x110000 - 0x800)
            points.append(point + 0x800 if point >= 0xD800 else point)
        valid("".join(map(chr, points)))
    with path.open("wb") as output:
        for source, expected, status in vectors:
            output.write(struct.pack("<IIi", len(source), len(expected), status))
            output.write(source)
            output.write(expected)
    return len(vectors)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", default="clang")
    parser.add_argument("--verify-regression", action="store_true")
    parser.add_argument("--work-dir", type=Path, default=Path("/tmp/reliefwin-brand-host/wim-unicode"))
    args = parser.parse_args()
    work = args.work_dir.resolve()
    work.mkdir(parents=True, exist_ok=True)
    vector_file = work / "vectors.bin"
    count = write_vectors(vector_file)
    current = (ROOT / SOURCE).read_text()
    sources = [("current", current, None)]
    if args.verify_regression:
        old = subprocess.check_output(["git", "show", "e01ee711:" + SOURCE], cwd=ROOT).decode()
        sources.append(("baseline", old, "UTF8 mismatch vector 0: actual=eda0bdedb880 expected=f09f9880"))
        before = """    if (file_name_nbytes % sizeof(uint16_t) != 0 || off + file_name_nbytes > entry_length)
        return -1;

    if (file_name_nbytes > 0)"""
        after = "    if (file_name_nbytes > 0 && off + file_name_nbytes <= entry_length)"
        assert current.count(before) == 1
        sources.append(("odd-byte-mutation", current.replace(before, after), "guard == allocation->guard"))
        before = """        if (core_utf16le_to_utf8(ctx, dentry->name_utf16, utf16_count, &dentry->name_utf8) != 0)
        {
            core_dentry_free(ctx, dentry);
            return -1;
        }"""
        after = """        if (core_utf16le_to_utf8(ctx, dentry->name_utf16, utf16_count, &dentry->name_utf8) != 0)
            return -1;"""
        assert current.count(before) == 1
        sources.append(("cleanup-mutation", current.replace(before, after), "retained == 0 && state.live == 0"))
        before = """            if (status < 0)
            {
                core_dentry_free(ctx, &child);
                core_free(ctx, queue);
                return -1;
            }
            if (status == 0)"""
        after = "            if (status <= 0)"
        assert current.count(before) == 1
        sources.append(("propagation-mutation", current.replace(before, after),
                        "core_deserialize_dentry_tree(&context, buffer, length, &root) == -1"))
        before = "entry_length > buffer_length - offset"
        assert current.count(before) == 1
        sources.append(("bounds-mutation", current.replace(before, "offset + entry_length > buffer_length"),
                        "status == 0 && state.calls == 0 && state.live == 0"))
    results = {}
    for name, source, expected_failure in sources:
        harness = work / (name + ".c")
        harness.write_text(HARNESS + "\n".join(function(source, item) for item in FUNCTIONS) + TESTS)
        program = work / name
        subprocess.run([args.cc, "-std=c11", "-Wall", "-Wextra", "-Werror", "-O1", "-g",
                        "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
                        "-I", str(ROOT / "sdk/include/reliefwin/libs/libwim"),
                        str(harness), "-o", str(program)], check=True)
        environment = dict(os.environ, ASAN_OPTIONS="detect_leaks=1", UBSAN_OPTIONS="halt_on_error=1")
        result = subprocess.run([str(program), str(vector_file)], capture_output=True, text=True, env=environment)
        if name == "current":
            assert result.returncode == 0, result.stdout + result.stderr
        else:
            assert result.returncode != 0 and expected_failure in result.stderr, result.stdout + result.stderr
        results[name] = {"exit_code": result.returncode, "stdout": result.stdout.strip(), "stderr": result.stderr.strip()}
    print(json.dumps({"codec_vectors": count, "sanitizers": "ASan/UBSan/leak detection", "runs": results}, indent=2))


if __name__ == "__main__":
    main()
