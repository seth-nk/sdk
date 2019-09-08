#!/bin/bash

MINGW_DIR="$1"
for i in `cat filelist` ; do
	echo "copying $i ..."
	mkdir -p `dirname $i`
	cp -r "$MINGW_DIR/$i" "$i"
done

update-icon-caches ./share/icons/Adwaita/
find -name '*.a' -delete
