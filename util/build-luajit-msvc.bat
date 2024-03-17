call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
cd vendordeps/luajit/src
msvcbuild.bat amalg
msvcbuild.bat static
