set ARG_BUILDDIR=%1
set ARG_EMSDK=%2
set ARG_RELPROJDIR=%3
set ARG_CMAKECMDS=%4

mkdir %ARG_BUILDDIR%
Pushd %ARG_BUILDDIR%

if %ARG_CMAKECMDS% EQU "" (
	%ARG_EMSDK%\emsdk_env.bat && emcmake cmake -G "MinGW Makefiles" -S %ARG_RELPROJDIR% && emmake make
) else (
	%ARG_EMSDK%\emsdk_env.bat && emcmake cmake %ARG_CMAKECMDS%=ON -G "MinGW Makefiles" -S %ARG_RELPROJDIR% && emmake make
)
