make -C libseth clean || exit 1
make -C desktop clean || exit 1

export LANG=C.UTF-8
export LANGUAGE=
export WINEDEBUG=-all
export WINEPATH='C:\mingw64\bin'
export PROJECT_WINECOMPILE=true
export MAKE='mingw32-make'

wine ${MAKE} -C libseth -j4 || exit 1
(cd desktop; make OS=Windows_NT {sethcli_conf.c,frontend-gtk-glade.h,frontend-gtk-menu.h})
wine ${MAKE} -C desktop EXTRA_FLAGS=-I../libseth -j4 || exit 1

unset MAKE

name='seth-dkt-and-sdk-windows'

rm libseth/*-sls.exe

rm -rf ${name} ${name}.7z

mkdir ${name}
mkdir ${name}/include
cp libseth/seth.h libseth/nkget.h ${name}/include/
cp libseth/libseth.dll ${name}/
cp -r libseth/example ${name}/
cp libseth/*.exe ${name}/
cp desktop/*.dll ${name}/
cp desktop/*.exe ${name}/
cp -r desktop/windist/. ${name}/

MINGW_DIR=`env WINEDEBUG=-all winepath -u 'C:\mingw64'`

(cd ${name}/gtk/ && ./update-mingw-libs.sh "$MINGW_DIR")
rm ${name}/gtk/filelist
rm ${name}/gtk/update-mingw-libs.sh

pandoc -f markdown -t html desktop/README.md > ${name}/README.html
pandoc -f markdown -t html README.md > ${name}/README-libseth.html

[ "$1" = "debug" ] && cp -r ${name} /tmp/tmp || 7za -t7z -m0=lzma2 -mx=9 -aoa -mfb=64 -md=32m -ms=on -mhe a ${name}.7z ${name}/.

exit 0
