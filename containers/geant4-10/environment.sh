# Source this file in bash. Legacy Geant4 environment scripts expect unset vars.
source /opt/root/bin/thisroot.sh
source /opt/geant4/bin/geant4.sh
source /opt/geant4/share/Geant4-10.7.4/geant4make/geant4make.sh
export PATH="/opt/ucgretina-bin:${PATH}"
export LD_LIBRARY_PATH="/opt/ucgretina-lib:${LD_LIBRARY_PATH:-}"
