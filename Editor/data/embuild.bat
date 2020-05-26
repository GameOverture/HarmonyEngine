set ARG_BUILDDIR=%1
set ARG_EMSDK=%2
set ARG_RELPROJDIR=%3

mkdir %ARG_BUILDDIR%
Pushd %ARG_BUILDDIR%
%ARG_EMSDK%\emsdk_env.bat && emcmake cmake -DHYBUILD_Emscripten=ON -G "MinGW Makefiles" -S %ARG_RELPROJDIR% && emmake make
