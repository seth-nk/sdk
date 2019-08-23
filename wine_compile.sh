make -C libseth clean || exit 1
make -C desktop clean || exit 1

export LANG=C
export LANGUAGE=
export WINEDEBUG=-all
export WINEPATH='C:\mingw64\bin'
export PROJECT_WINECOMPILE=true
export MAKE='mingw32-make'

wine ${MAKE} -C libseth -j4 || exit 1
(cd desktop; make OS=Windows_NT sethcli_conf.h)
wine ${MAKE} -C desktop EXTRA_FLAGS=-I../libseth -j4 || exit 1

unset MAKE

rm libseth/*-sls.exe

rm -rf seth-sdk-windows seth-sdk-windows.7z

mkdir seth-sdk-windows
mkdir seth-sdk-windows/include
cp libseth/seth.h libseth/nkget.h seth-sdk-windows/include/
cp libseth/libseth.dll seth-sdk-windows/
cp -r libseth/example seth-sdk-windows/
cp libseth/*.exe seth-sdk-windows/
cp desktop/*.exe seth-sdk-windows/
cp -r desktop/windist/. seth-sdk-windows/

[ "$1" = "debug" ] && cp -r seth-sdk-windows /tmp/tmp || 7za -t7z -m0=lzma2 -mx=9 -aoa -mfb=64 -md=32m -ms=on -mhe a seth-sdk-windows.7z seth-sdk-windows/.

exit 0
