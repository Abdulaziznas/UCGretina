#!/usr/bin/env bash
set -euo pipefail
if ! command -v docker >/dev/null 2>&1; then
    echo 'Docker is unavailable. Enable Docker Desktop WSL integration for Ubuntu.' >&2
    exit 1
fi
output_dir=${UCGRETINA_OUTPUT_DIR:-"$PWD/container-results/geant4-10"}
mkdir -p "$output_dir"
output_dir=$(cd -- "$output_dir" && pwd)
terminal_args=(-i)
if [[ -t 0 && -t 1 ]]; then terminal_args+=(-t); fi
if [[ $# == 0 ]]; then set -- bash; fi
# No host source or Geant4/ROOT installation is mounted into the image.
docker run --rm "${terminal_args[@]}" --platform linux/amd64 \
    --user "$(id -u):$(id -g)" \
    --mount "type=bind,src=$output_dir,dst=/work" \
    ucgretina:g4-10.7.4 "$@"
