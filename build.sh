#!/usr/bin/env bash
system_wayland_prefix="/usr/share/wayland-protocols"
wayland_protocols=(
	"stable/xdg-shell/xdg-shell"
	"unstable/xdg-decoration/xdg-decoration-unstable-v1"
	"staging/xdg-toplevel-icon/xdg-toplevel-icon-v1"
	"unstable/relative-pointer/relative-pointer-unstable-v1"
	"unstable/pointer-constraints/pointer-constraints-unstable-v1"
	"unstable/xdg-output/xdg-output-unstable-v1"
	"staging/pointer-warp/pointer-warp-v1"
)

wayland_source_destination="./wayland/src"
wayland_header_destination="./wayland/include"
mkdir -p $wayland_source_destination $wayland_header_destination

for protocol in ${wayland_protocols[@]}; do
	xml_path="${system_wayland_prefix%/}/${protocol}.xml"
	src_path="${wayland_source_destination%/}/$(basename $protocol).c"
	header_path="${wayland_header_destination%/}/$(basename $protocol).h"
	wayland-scanner public-code "$xml_path" "$src_path"
	wayland-scanner client-header "$xml_path" "$header_path"
done

printf "#pragma once\n" > ./wayland_source_files.h
for file in "$wayland_source_destination"/*.c; do
	printf "#include \"%s\"\n" "$file" >> ./wayland_source_files.h
done

cflags="-Wall -Wextra -Wpedantic -Wconversion -Woverflow"
includes="-I$wayland_header_destination"
libs="-D RGFW_WAYLAND -lwayland-cursor -lwayland-client -lxkbcommon  -lwayland-egl -lEGL -lm"
gcc $cflags $includes main.c -o game $libs
