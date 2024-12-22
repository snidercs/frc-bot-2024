:

# this is a fast build of the project without the overhead of watching the file system
./util/docker-run.sh ./util/build-luajit-linux64.sh && ./util/build-luajit-roborio.sh && ./gradlew --no-watch-fs build
