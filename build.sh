#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")"

joinpath() {
    local left="${1%/}"
    local right="${2#/}"
    printf '%s' "$left/$right"
}

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

wayland_source_destination="src/os/linux/wayland/src"
wayland_header_destination="src/os/linux/wayland/include"
mkdir -p "$wayland_source_destination" "$wayland_header_destination"

for protocol in "${wayland_protocols[@]}"; do
	protocol_basename=$(basename "$protocol")
	xml_path="$(joinpath "$system_wayland_prefix" "$protocol").xml"
	src_path="$(joinpath "$wayland_source_destination" "$protocol_basename").c"
	header_path="$(joinpath "$wayland_header_destination" "$protocol_basename").h"
	wayland-scanner public-code "$xml_path" "$src_path"
	wayland-scanner client-header "$xml_path" "$header_path"
done

wayland_source_files_header_path="src/os/linux/wayland/wayland_source_files.h"
printf "#pragma once\n" > "$wayland_source_files_header_path"
for file in "$wayland_source_destination"/*.c; do
	printf "#include \"%s\"\n" "$(basename "$file")" >> "$wayland_source_files_header_path"
done

build_destination="out"
mkdir -p "$build_destination"
cflags=(-g3 -Og -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
warnings=(-Weverything -Wno-assign-enum -Wno-declaration-after-statement -Wno-implicit-int-enum-cast -Wno-undef -Wno-unsafe-buffer-usage -Wno-unused-function -Wno-unused-macros -Wno-unused-parameter -Wno-unused-variable -Wno-static-in-inline -Wno-undefined-internal -Wno-implicit-void-ptr-cast -Wno-float-equal)
includes=(-I src -isystem src/thirdparty -isystem "$wayland_header_destination" -isystem "$wayland_source_destination")
libs=(-D RGFW_WAYLAND -lwayland-cursor -lwayland-client -lxkbcommon -lm)
clang "${cflags[@]}" "${warnings[@]}" "${includes[@]}" src/main.c -o "$(joinpath "$build_destination" "game")" "${libs[@]}"
