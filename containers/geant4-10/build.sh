#!/usr/bin/env bash
set -euo pipefail
container_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
if ! command -v docker >/dev/null 2>&1; then
    echo 'Docker is unavailable. Enable Docker Desktop WSL integration for Ubuntu; see containers/geant4-10/README.md.' >&2
    exit 1
fi
docker info >/dev/null
docker build --platform linux/amd64 \
    --build-arg "BUILD_JOBS=${BUILD_JOBS:-2}" \
    --build-arg "BUILD_VARIANTS=${BUILD_VARIANTS:-standard pol lh lh-pol scan scan-pol}" \
    --tag ucgretina:g4-10.7.4 "$container_dir"
