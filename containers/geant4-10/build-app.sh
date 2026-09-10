#!/usr/bin/env bash
set -eo pipefail
source /opt/container/environment.sh
jobs=${1:-2}
variants=${2:-standard}
[[ "$jobs" =~ ^[1-9][0-9]*$ ]] || { echo 'BUILD_JOBS must be a positive integer' >&2; exit 2; }
mkdir -p /opt/ucgretina-bin /opt/ucgretina-lib
cd /opt/UCGretina
for variant in $variants; do
    flags=()
    case "$variant" in
        standard) binary=UCGretina ;;
        pol)      binary=UCGretina_Pol; flags=(POL=1) ;;
        lh)       binary=UCGretina_LH; flags=(LHTARGET=1) ;;
        lh-pol)   binary=UCGretina_LH_Pol; flags=(LHTARGET=1 POL=1) ;;
        scan)     binary=UCGretina_Scan; flags=(SCANNING=1) ;;
        scan-pol) binary=UCGretina_Scan_Pol; flags=(SCANNING=1 POL=1) ;;
        *) echo "Unknown variant: $variant" >&2; exit 2 ;;
    esac
    # The original makefile does not track changed compiler flags. Isolate objects.
    export G4WORKDIR="/tmp/g4work-${variant}"
    make -j "$jobs" "${flags[@]}"
    install -m 755 "$G4WORKDIR/bin/$G4SYSTEM/$binary" /opt/ucgretina-bin/
    # The legacy makefile builds an application shared library as well.
    # Preserve it before removing the temporary object directory.
    install -m 755 "$G4WORKDIR/tmp/$G4SYSTEM/$binary/lib${binary}.so" /opt/ucgretina-lib/
    rm -rf -- "$G4WORKDIR"
done
