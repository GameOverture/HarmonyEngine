set ARG_BUILDDIR=%1
set ARG_EMSDK=%2
set ARG_RELPROJDIR=%3
set ARG_CMAKECMDS=%4

mkdir %ARG_BUILDDIR%
Pushd %ARG_BUILDDIR%
%ARG_EMSDK%\emsdk_env.bat && emcmake cmake %ARG_CMAKECMDS% -G "MinGW Makefiles" -S %ARG_RELPROJDIR% && emmake make
