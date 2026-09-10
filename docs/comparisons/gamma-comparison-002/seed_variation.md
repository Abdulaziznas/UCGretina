# Independent-seed comparison

Each run contains 100,000 Eu-152 single-gamma primaries. All settings except seeds match the original comparison.

The difference is divided by its estimated standard error (z). Errors use event-level sample variances, including correlations between crystals hit in the same primary event. Approximately ±1.96 standard errors is an individual 95% interval under a normal approximation. These correlated checks are exploratory, not a global equivalence test. Peak windows include continuum.

## New Geant4 11 vs new Geant4 10

| Observable | Reference count | New count | Difference / standard error |
|---|---:|---:|---:|
| detected events | 17428 | 17446 | +0.11 |
| crystal entries | 20290 | 20250 | -0.19 |
| 120-124 keV | 2117 | 2234 | +1.79 |
| 242-248 keV | 638 | 650 | +0.34 |
| 342-348 keV | 1598 | 1521 | -1.39 |
| 776-782 keV | 515 | 504 | -0.35 |
| 962-968 keV | 491 | 516 | +0.79 |
| 1110-1116 keV | 398 | 428 | +1.05 |
| 1406-1412 keV | 566 | 539 | -0.81 |

## Geant4 10: new vs previous seed

| Observable | Reference count | New count | Difference / standard error |
|---|---:|---:|---:|
| detected events | 17636 | 17428 | -1.22 |
| crystal entries | 20566 | 20290 | -1.30 |
| 120-124 keV | 2170 | 2117 | -0.82 |
| 242-248 keV | 645 | 638 | -0.20 |
| 342-348 keV | 1607 | 1598 | -0.16 |
| 776-782 keV | 526 | 515 | -0.34 |
| 962-968 keV | 467 | 491 | +0.78 |
| 1110-1116 keV | 413 | 398 | -0.53 |
| 1406-1412 keV | 487 | 566 | +2.44 |

## Geant4 11: new vs previous seed

| Observable | Reference count | New count | Difference / standard error |
|---|---:|---:|---:|
| detected events | 17653 | 17446 | -1.22 |
| crystal entries | 20542 | 20250 | -1.38 |
| 120-124 keV | 2183 | 2234 | +0.78 |
| 242-248 keV | 647 | 650 | +0.08 |
| 342-348 keV | 1619 | 1521 | -1.76 |
| 776-782 keV | 492 | 504 | +0.38 |
| 962-968 keV | 499 | 516 | +0.53 |
| 1110-1116 keV | 419 | 428 | +0.31 |
| 1406-1412 keV | 523 | 539 | +0.49 |

