# Sulfur-44 reaction comparison

100,000 beam primaries per state and version; independent seeds. Original NSCL 12-quad geometry, 100 MeV/u sulfur-44 beam, Be target, emstandard_opt4, two workers.

Crystal spectra use laboratory deposited energies without Doppler correction, resolution smearing, or cross-crystal addback. Ratios are normalized per beam primary. Thus the moving-source peaks are Doppler broadened. Excitation states are compared separately, without assuming population weights.

The table reports event-level standard errors, including correlations between crystals hit in the same primary event. Normal-approximation z values are omitted when combined counts are below 20. Broad energy windows are exploratory checks, not a global equivalence test.

## Excitation 1329 keV

| Observable | Geant4 10 | Geant4 11 | Difference / standard error |
|---|---:|---:|---:|
| detected events | 29514 | 29465 | -0.24 |
| crystal entries | 37648 | 37640 | -0.03 |
| 0-500 keV | 15588 | 15647 | +0.31 |
| 500-1000 keV | 9115 | 9049 | -0.48 |
| 1000-1500 keV | 10836 | 10847 | +0.08 |
| 1500-2000 keV | 2109 | 2097 | -0.19 |
| 2000-2500 keV | 0 | 0 | n/a (sparse or no variance) |
| 2500-3000 keV | 0 | 0 | n/a (sparse or no variance) |
| 3000-3500 keV | 0 | 0 | n/a (sparse or no variance) |
| 3500-4000 keV | 0 | 0 | n/a (sparse or no variance) |

Recorded emitted gammas: 99997 (Geant4 10), 99999 (Geant4 11).

## Excitation 2150 keV

| Observable | Geant4 10 | Geant4 11 | Difference / standard error |
|---|---:|---:|---:|
| detected events | 27799 | 27809 | +0.05 |
| crystal entries | 36711 | 36912 | +0.68 |
| 0-500 keV | 13157 | 13185 | +0.16 |
| 500-1000 keV | 4716 | 4609 | -1.09 |
| 1000-1500 keV | 4650 | 4919 | +2.79 |
| 1500-2000 keV | 7364 | 7530 | +1.41 |
| 2000-2500 keV | 5504 | 5441 | -0.62 |
| 2500-3000 keV | 1320 | 1228 | -1.83 |
| 3000-3500 keV | 0 | 0 | n/a (sparse or no variance) |
| 3500-4000 keV | 0 | 0 | n/a (sparse or no variance) |

Recorded emitted gammas: 99999 (Geant4 10), 99999 (Geant4 11).

## Excitation 2457 keV

| Observable | Geant4 10 | Geant4 11 | Difference / standard error |
|---|---:|---:|---:|
| detected events | 50747 | 50589 | -0.71 |
| crystal entries | 75798 | 75521 | -0.68 |
| 0-500 keV | 32637 | 32628 | -0.03 |
| 500-1000 keV | 21807 | 21536 | -1.32 |
| 1000-1500 keV | 19206 | 19101 | -0.56 |
| 1500-2000 keV | 2092 | 2196 | +1.60 |
| 2000-2500 keV | 40 | 39 | -0.11 |
| 2500-3000 keV | 14 | 17 | +0.54 |
| 3000-3500 keV | 2 | 4 | n/a (sparse or no variance) |
| 3500-4000 keV | 0 | 0 | n/a (sparse or no variance) |

Recorded emitted gammas: 199994 (Geant4 10), 199992 (Geant4 11).

