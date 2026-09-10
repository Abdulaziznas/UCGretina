# Sulfur-44 reaction comparison

100,000 beam primaries per state and version; independent seeds. Original NSCL 12-quad geometry, 100 MeV/u sulfur-44 beam, Be target, emstandard_opt4, two workers.

Crystal spectra use laboratory deposited energies without Doppler correction, resolution smearing, or cross-crystal addback. Ratios are normalized per beam primary. Thus the moving-source peaks are Doppler broadened. Excitation states are compared separately, without assuming population weights.

The table reports event-level standard errors, including correlations between crystals hit in the same primary event. Normal-approximation z values are omitted when combined counts are below 20. Broad energy windows are exploratory checks, not a global equivalence test.

## Excitation 1329 keV

| Observable | Geant4 10 | Geant4 11 | Difference / standard error |
|---|---:|---:|---:|
| detected events | 29529 | 29797 | +1.31 |
| crystal entries | 37904 | 38349 | +1.53 |
| 0-500 keV | 15578 | 15849 | +1.44 |
| 500-1000 keV | 9434 | 9333 | -0.73 |
| 1000-1500 keV | 10835 | 11010 | +1.25 |
| 1500-2000 keV | 2057 | 2157 | +1.56 |
| 2000-2500 keV | 0 | 0 | n/a (sparse or no variance) |
| 2500-3000 keV | 0 | 0 | n/a (sparse or no variance) |
| 3000-3500 keV | 0 | 0 | n/a (sparse or no variance) |
| 3500-4000 keV | 0 | 0 | n/a (sparse or no variance) |

Recorded emitted gammas: 99997 (Geant4 10), 99998 (Geant4 11).

## Excitation 2150 keV

| Observable | Geant4 10 | Geant4 11 | Difference / standard error |
|---|---:|---:|---:|
| detected events | 27710 | 27531 | -0.90 |
| crystal entries | 36487 | 36394 | -0.32 |
| 0-500 keV | 12909 | 12991 | +0.47 |
| 500-1000 keV | 4768 | 4642 | -1.28 |
| 1000-1500 keV | 4730 | 4728 | -0.02 |
| 1500-2000 keV | 7470 | 7570 | +0.85 |
| 2000-2500 keV | 5340 | 5198 | -1.42 |
| 2500-3000 keV | 1270 | 1265 | -0.10 |
| 3000-3500 keV | 0 | 0 | n/a (sparse or no variance) |
| 3500-4000 keV | 0 | 0 | n/a (sparse or no variance) |

Recorded emitted gammas: 100000 (Geant4 10), 100000 (Geant4 11).

## Excitation 2457 keV

| Observable | Geant4 10 | Geant4 11 | Difference / standard error |
|---|---:|---:|---:|
| detected events | 50511 | 50726 | +0.96 |
| crystal entries | 75295 | 75752 | +1.13 |
| 0-500 keV | 32375 | 32791 | +1.53 |
| 500-1000 keV | 21490 | 21764 | +1.34 |
| 1000-1500 keV | 19177 | 18974 | -1.09 |
| 1500-2000 keV | 2190 | 2153 | -0.57 |
| 2000-2500 keV | 38 | 45 | +0.77 |
| 2500-3000 keV | 25 | 23 | -0.29 |
| 3000-3500 keV | 0 | 2 | n/a (sparse or no variance) |
| 3500-4000 keV | 0 | 0 | n/a (sparse or no variance) |

Recorded emitted gammas: 199991 (Geant4 10), 199988 (Geant4 11).

