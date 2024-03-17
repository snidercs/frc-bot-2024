cd luajit
export PATH="$HOME/wpilib/2024/roborio/bin:$PATH"
make clean
make amalg HOST_CC="gcc -m32 -std=c99" \
    CROSS=arm-frc2024-linux-gnueabi- \
    XCFLAGS="-DLUAJIT_ENABLE_LUA52COMPAT=1" \
    BUILDMODE="static" \
    PREFIX="$HOME/SDKs/botlib/roborio"
make install PREFIX="$HOME/SDKs/botlib/roborio"
