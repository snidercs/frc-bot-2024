#!/bin/bash
set -e
here="$(pwd)"
prefix="$HOME/SDKs/botlib/msvc"
ljpath="vendordeps/luajit"

mkdir -p "$prefix/include" "$prefix/lib"

# "clean" msvc luajit build
cd "${ljpath}" && \
    rm -rf src && \
    git checkout src
cd "${here}"

