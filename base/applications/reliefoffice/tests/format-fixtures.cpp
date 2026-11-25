/*
 * PROJECT:     ReliefOffice API tests
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Generate and verify documents against the original Unix implementations
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */
#include <reliefword/document.h>
#include <reliefexcel/document.h>
#include <reliefppt/document.h>
#include <relieftodo/document.h>
#include <reliefdiagram/document.h>
#include <reliefmathpp/document.h>
#include <reliefmath/document.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char root[4096];
static bool verify;
static unsigned checks;

static void check(bool condition)
{
    ++checks;
    if (!condition) { fprintf(stderr, "Format fixture check %u failed\n", checks); abort(); }
}

static const char *path(const char *extension)
{
    static char output[4096];
    int count = snprintf(output, sizeof(output), "%s/fixture.%s", root, extension);
    check(count > 0 && size_t(count) < sizeof(output));
    return output;
}

static void bytes(const char *extension, const void *data, size_t size)
{
    if (verify) {
        FILE *file = fopen(path(extension), "rb");
        check(file != nullptr);
        for (size_t at = 0; at < size; ++at)
            check(fgetc(file) == static_cast<const unsigned char *>(data)[at]);
        check(fgetc(file) == EOF && !ferror(file));
        check(!fclose(file));
    } else {
        FILE *file = fopen(path(extension), "wb");
        check(file != nullptr);
        check(fwrite(data, 1, size, file) == size);
        check(!fclose(file));
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    verify = !strcmp(argv[1], "--verify");
    check(strlen(argv[2]) < sizeof(root));
    strcpy(root, argv[2]);
    {
        rw::Editor editor;
        check(editor.insert_utf8("中文 ReliefOffice\nA & <B> e\xcc\x81", strlen("中文 ReliefOffice\nA & <B> e\xcc\x81")));
        check(editor.doc.title.set("含中文的标题"));
        rw::Text encoded, error;
        check(rw::encode_document(editor.doc, rw::FileFormat::Native, encoded, error));
        bytes("rword", encoded.c_str(), encoded.size());
        rw::Document loaded;
        check(rw::decode_document(encoded.c_str(), encoded.size(), rw::FileFormat::Native, loaded, error));
        rw::Text second;
        check(rw::encode_document(loaded, rw::FileFormat::Native, second, error));
        check(encoded == second);
    }
    {
        rx::Workbook workbook;
        rx::Text error;
        check(workbook.init());
        check(workbook.set_input(workbook.sheets[0].id, 0, 0, "项目名称"));
        check(workbook.set_input(workbook.sheets[0].id, 1, 0, "12.5"));
        check(workbook.set_input(workbook.sheets[0].id, 2, 0, "=SUM(A2:A2)*2"));
        if (verify) {
            rx::Workbook loaded;
            check(rx::load_workbook(path("rexl"), loaded, error));
            check(loaded.sheets.size() == workbook.sheets.size());
            check(rx::save_workbook(path("rexl.out"), workbook, error));
            FILE *file = fopen(path("rexl.out"), "rb");
            check(file != nullptr);
            char buffer[65536];
            size_t size = fread(buffer, 1, sizeof(buffer), file);
            check(!ferror(file) && feof(file));
            check(!fclose(file));
            bytes("rexl", buffer, size);
            check(!remove(path("rexl.out")));
        } else check(rx::save_workbook(path("rexl"), workbook, error));
    }
    {
        rp::Deck deck;
        rp::Text error, encoded, second;
        check(rp::example_deck(deck, error));
        check(rp::encode_deck(deck, encoded, error));
        bytes("rppt", encoded.c_str(), encoded.size());
        rp::Deck loaded;
        check(rp::decode_deck(encoded.c_str(), encoded.size(), loaded, error));
        check(rp::encode_deck(loaded, second, error));
        check(encoded == second);
    }
    {
        rt::Document document;
        rt::Text error;
        rt::TaskId id;
        check(document.title.set("任务和截止日期"));
        check(rt::add_task(document, "完成移植", "验证保存、恢复", "ReliefWin", "2026-11-24", rt::Priority::High, "测试,中文", id));
        document.tasks[0].created = 1795478400;
        document.tasks[0].updated = 1795478400;
        rt::Bytes encoded, second;
        check(rt::encode_document(document, encoded, error));
        bytes("rtodo", encoded.data(), encoded.size());
        rt::Document loaded;
        check(rt::decode_document(encoded.data(), encoded.size(), loaded, error));
        check(rt::encode_document(loaded, second, error));
        check(encoded.size() == second.size() && !memcmp(encoded.data(), second.data(), second.size()));
    }
    {
        rd::Document document;
        rd::Text error;
        rd::Bytes encoded, second;
        check(rd::example(document, 0, error));
        check(rd::encode(document, encoded, error));
        bytes("rdiagram", encoded.data(), encoded.size());
        rd::Document loaded;
        check(rd::decode(encoded.data(), encoded.size(), loaded, error));
        check(rd::encode(loaded, second, error));
        check(encoded.size() == second.size() && !memcmp(encoded.data(), second.data(), second.size()));
    }
    {
        rmp::Document document;
        rmp::Text error;
        rmp::Bytes encoded, second;
        check(rmp::example(document, 0));
        check(rmp::encode(document, encoded, error));
        bytes("rmathpp", encoded.data(), encoded.size());
        rmp::Document loaded;
        check(rmp::decode(encoded.data(), encoded.size(), loaded, error));
        check(rmp::encode(loaded, second, error));
        check(encoded.size() == second.size() && !memcmp(encoded.data(), second.data(), second.size()));
    }
    {
        rm::Document document;
        rm::Error error;
        rm::Bytes encoded, second;
        rm::FormulaId id;
        check(document.title.set("公式文档"));
        check(rm::add_formula(document, "二次方程", "x=\\frac{-b\\pm\\sqrt{b^2-4ac}}{2a}", id, error));
        check(rm::encode_document(document, encoded, error));
        bytes("rmath", encoded.data(), encoded.size());
        rm::Document loaded;
        check(rm::decode_document(encoded.data(), encoded.size(), loaded, error));
        check(rm::encode_document(loaded, second, error));
        check(encoded.size() == second.size() && !memcmp(encoded.data(), second.data(), second.size()));
    }
    printf("Seven original ReliefOffice formats: %u checks passed\n", checks);
    return 0;
}
