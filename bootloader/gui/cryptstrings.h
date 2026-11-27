/*
 * PROJECT:     ReliefWin Boot Loader
 * LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
 * PURPOSE:     Localized graphical pre-boot disk unlock text
 * COPYRIGHT:   Copyright 2026 ReliefWin contributors
 */

#pragma once

typedef struct BM_CRYPT_TEXT
{
    PCWSTR Title, Description, RecoveryDescription, Password, Recovery, Unlock, Cancel, UseRecovery, UsePassword;
    PCWSTR Wrong, PasswordFormat, RecoveryFormat, Busy, Failed, Incomplete, Back, Hint, Language;
} BM_CRYPT_TEXT;

static const BM_CRYPT_TEXT BmCryptText[] =
{
    {
        L"解锁您的设备", L"输入密码以继续启动 ReliefWin", L"输入恢复密钥以继续启动 ReliefWin", L"密码", L"恢复密钥",
        L"解锁", L"返回", L"使用恢复密钥", L"改用密码",
        L"无法解锁，请检查密码或恢复密钥后重试。",
        L"请输入 12–128 位 ASCII 字符的密码。",
        L"请输入 64 位十六进制恢复密钥。", L"正在解锁…",
        L"无法解锁此设备，请从恢复介质检查加密配置。",
        L"磁盘转换尚未完成，请从外部恢复介质继续。",
        L"返回启动菜单", L"Tab 切换选项 · Enter 确认 · Esc 返回 · F2 语言", L"简体中文"
    },
    {
        L"Unlock your device", L"Enter your password to start ReliefWin", L"Enter your recovery key to start ReliefWin", L"Password", L"Recovery key",
        L"Unlock", L"Back", L"Use a recovery key", L"Use a password",
        L"Could not unlock. Check your password or recovery key.",
        L"Enter a password of 12–128 ASCII characters.",
        L"Enter your 64-digit hexadecimal recovery key.", L"Unlocking…",
        L"Cannot unlock this device. Check encryption from recovery media.",
        L"Disk conversion is incomplete. Continue from external recovery media.",
        L"Back to boot menu", L"Tab: options · Enter: select · Esc: back · F2: language", L"English"
    },
    {
        L"デバイスのロックを解除", L"パスワードを入力して ReliefWin を起動します", L"回復キーを入力して ReliefWin を起動します", L"パスワード", L"回復キー",
        L"ロック解除", L"戻る", L"回復キーを使用", L"パスワードを使用",
        L"解除できません。パスワードまたは回復キーを確認してください。",
        L"12–128 文字の ASCII パスワードを入力してください。",
        L"64 桁の 16 進数の回復キーを入力してください。", L"ロックを解除しています…",
        L"解除できません。回復メディアから暗号化設定を確認してください。",
        L"ディスク変換が未完了です。外部の回復メディアから続行してください。",
        L"起動メニューに戻る", L"Tab: 移動 · Enter: 選択 · Esc: 戻る · F2: 言語", L"日本語"
    }
};
