
prefix="$1"
if [ -z "$1" ]; then
    prefix="$HOME/SDKs/botlib/linux64"
fi

cd luajit
make clean
make amalg HOST_CC="gcc -m64 -std=c99" \
    BUILDMODE="static" \
    XCFLAGS="-DLUAJIT_ENABLE_LUA52COMPAT=1" \
    PREFIX="${prefix}"
make install PREFIX="${prefix}"
