# Build run by GitHub actions.
set -ex
chmod +x gradlew
./util/build-luajit-linux64.sh
./util/build-luajit-roborio.sh
./gradlew --no-watch-fs build
