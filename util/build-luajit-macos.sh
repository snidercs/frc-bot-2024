#!/bin/bash

export MACOSX_DEPLOYMENT_TARGET=10.12

host_arch="$1"
if [ -z "$host_arch" ]; then
    host_arch=$(arch)
fi

ljdir="$(pwd)/vendordeps/luajit"
sdkdir="$(pwd)/vendordeps/sdk"

cd "$ljdir"
set -ex

### Build for host machine
make clean
make amalg CC="clang -arch $host_arch -std=c99" \
    BUILDMODE="static" \
    XCFLAGS="-DLUAJIT_ENABLE_LUA52COMPAT=1" \
    PREFIX="${sdkdir}/macos"
make install PREFIX="${sdkdir}/macos"
make clean
