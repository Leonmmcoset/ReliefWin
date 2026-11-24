> ReliefWin has no public support endpoint configured. Contact the maintainer of this checkout.

# ReliefWin のインストール

> [English](../../INSTALL.md) | [中文](../zh-CN/INSTALL.md) | [Français](../fr-FR/INSTALL.md) | [Español](../es-ES/INSTALL.md) | **日本語** | [العربية](../ar-MA/INSTALL.md) | [Українська](../uk-UA/INSTALL.md)

## イメージをビルドする

[ReliefWin を自分でビルドする](README.md#reliefwin-を自分でビルドする)の説明に従ってビルド環境を整え、ビルドを構成します。次に、出力フォルダー（たとえば `output-Clang-amd64-debug`）で必要なイメージをビルドします。

| コマンド | 作成されるもの | 用途 |
|---|---|---|
| `ninja bootcd` | `bootcd.iso` | CD イメージから ReliefWin を実行する、または ReliefWin セットアップを起動してディスクに ReliefWin をインストールする。 |
| `ninja reliefwinimg` | `ReliefWin.img` | ReliefWin がインストール済みのディスクを起動する。UEFI と BIOS のどちらでも起動します。 |
| `ninja reliefwinvhd` | `ReliefWin.vhd`、`ReliefWin.vhdx` | 同じプリインストール済みディスクを仮想マシンで使う。VHD は VirtualBox と Hyper-V 第 1 世代で、VHDX は VHD ディスクを受け付けない Hyper-V 第 2 世代で使います。 |

各ターゲットは必要なものをすべて先にビルドし、ファイルは出力フォルダーに書き出されます。

## セットアップ CD からインストールする

仮想マシンまたは PC で `bootcd.iso` を起動し、ReliefWin セットアップの指示に従います。セットアップは Windows セットアップと同じようにドライブを一覧表示し、パーティションの作成、削除、フォーマット、縮小、拡張ができます。ReliefWin を NTFS にインストールし、必要な EFI システムパーティションを作成します。

## プリインストール済みディスクを使う

`ReliefWin.img` はセットアップ不要です。QEMU などの仮想マシンで起動するか、USB ドライブや SD カードに書き込んでそこからマシンを起動します。VHD ディスクを受け付ける仮想マシンには `ReliefWin.vhd` を、Hyper-V 第 2 世代の仮想マシンにはセキュアブートを無効にしたうえで `ReliefWin.vhdx` を使います。

プリインストール済みディスクにも ReliefWin セットアップが入っています。実行中のシステムから別のディスクに ReliefWin をインストールするには、`C:\<アーキテクチャ>\reliefwin.exe`（たとえば `C:\arm64\reliefwin.exe`）を実行します。

## ヘルプ

問題が起きたら、アーキテクチャ、ビルドの種類、ビルドしたコミット、問題に至る手順を添えて Issues で報告してください。ガイドやメモは Wiki にあり、Discord または Matrix でチームに質問できます。
