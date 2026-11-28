#!/bin/sh
# PROJECT:     ReliefWin build tools
# LICENSE:     GPL-3.0-or-later (https://spdx.org/licenses/GPL-3.0-or-later)
# PURPOSE:     Install and validate host build dependencies
# COPYRIGHT:   Copyright 2026 ReliefWin contributors

set -eu

build_deps_root=$(CDPATH='' cd -- "$(dirname -- "$0")/.." && pwd -P)
if [ "${ROSBE_DOCKER_ACTIVE:-0}" = 1 ]; then
    cmake -P "$build_deps_root/sdk/cmake/host-dependencies.cmake"
    exit
fi
build_deps_manager=manual
if command -v pacman >/dev/null 2>&1; then
    build_deps_manager=pacman
elif command -v apt-get >/dev/null 2>&1; then
    build_deps_manager=apt-get
fi
build_deps_packages=
build_deps_missing=

need_package() {
    build_deps_missing="$build_deps_missing $1"
    case "$build_deps_manager" in
        pacman) build_deps_package=$3 ;;
        *) build_deps_package=$2 ;;
    esac
    case " $build_deps_packages " in
        *" $build_deps_package "*) ;;
        *) build_deps_packages="$build_deps_packages $build_deps_package" ;;
    esac
}

check_packages() {
    build_deps_packages=
    build_deps_missing=
    for build_deps_tool in git:git:git cmake:cmake:cmake ninja:ninja-build:ninja \
        make:make:make flex:flex:flex bison:bison:bison m4:m4:m4 \
        ccache:ccache:ccache curl:curl:curl wget:wget:wget xz:xz-utils:xz tar:tar:tar patch:patch:patch \
        python3:python3:python pkg-config:pkg-config:pkgconf \
        glslangValidator:glslang-tools:glslang msidump:msitools:msitools msiextract:msitools:msitools; do
        build_deps_command=${build_deps_tool%%:*}
        build_deps_names=${build_deps_tool#*:}
        command -v "$build_deps_command" >/dev/null 2>&1 ||
            need_package "$build_deps_command" "${build_deps_names%%:*}" "${build_deps_names#*:}"
    done
    if ! command -v cc >/dev/null 2>&1 && ! command -v gcc >/dev/null 2>&1 && ! command -v clang >/dev/null 2>&1; then
        need_package 'host-C-compiler' build-essential base-devel
    fi
    if ! command -v c++ >/dev/null 2>&1 && ! command -v g++ >/dev/null 2>&1 && ! command -v clang++ >/dev/null 2>&1; then
        need_package 'host-C++-compiler' build-essential base-devel
    fi
    for build_deps_module in mako:python3-mako:python-mako yaml:python3-yaml:python-yaml packaging:python3-packaging:python-packaging; do
        build_deps_names=${build_deps_module#*:}
        python3 -c "import ${build_deps_module%%:*}" >/dev/null 2>&1 ||
            need_package "Python:${build_deps_module%%:*}" "${build_deps_names%%:*}" "${build_deps_names#*:}"
    done
}

check_packages
if [ -n "$build_deps_packages" ]; then
    if [ "$build_deps_manager" = manual ]; then
        echo "Missing build dependencies:$build_deps_missing" >&2
        echo "Install the host tools and Python mako, PyYAML and packaging modules; see README.md." >&2
        exit 1
    fi
    build_deps_sudo=
    if [ "$(id -u)" != 0 ]; then
        command -v sudo >/dev/null 2>&1 || {
            echo "Install these packages with administrator privileges:$build_deps_packages" >&2
            exit 1
        }
        build_deps_sudo=sudo
    fi
    echo "Installing build dependencies:$build_deps_packages"
    if [ "$build_deps_manager" = pacman ]; then
        $build_deps_sudo pacman -S --needed --noconfirm $build_deps_packages
    else
        $build_deps_sudo apt-get update
        $build_deps_sudo apt-get install -y $build_deps_packages
    fi
    check_packages
    if [ -n "$build_deps_missing" ]; then
        echo "Build dependencies are still unavailable:$build_deps_missing" >&2
        echo "Ensure python3 uses the interpreter with the installed modules." >&2
        exit 1
    fi
fi

cmake -P "$build_deps_root/sdk/cmake/host-dependencies.cmake"
