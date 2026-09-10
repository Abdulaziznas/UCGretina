# Ten additional sulfur-44 seed sets

Published reports, plots, and pooled ROOT histograms are available in the
[comparison archive](../docs/comparisons/README.md). Full event data remain local.

This follow-up runs ten additional 100,000-event simulations for each of the
1329, 2150, and 2457 keV excitation states, in both Geant4 10.7.4 (container)
and Geant4 11.4.1 (WSL). There are 60 new simulations and six million new beam
primaries. The native executable includes the previously verified emitted-gamma
process-name filter correction; no new physics settings were introduced.

Run directories are `container-results/sulfur-comparison-004/` through `013/`.
Each directory contains three state directories, each with separate `g4-10`
and `g4-11` macros, logs, ASCII outputs, and `spectrum.root` files. The seed
offsets are 4,000,000 through 22,000,000, in steps of 2,000,000. Actual seed pairs,
input hashes, source hashes, and the uncommitted native source patch are saved.

## Pooled analysis

`analysis/aggregate_sulfur.py` produces
`container-results/sulfur-ten-seed-summary/` with:

- `new_ten/comparison.pdf`, `.png`, and `spectra.root`: ten new runs only,
  1,000,000 primaries per state/version.
- `all_twelve/comparison.pdf`, `.png`, and `spectra.root`: ten new runs plus
  `sulfur-comparison-002` and `003`, totaling 1,200,000 primaries per state/version.
- `target_seed_followup.pdf`: differences in the previously highlighted window
  across the ten new seed sets, with one-standard-error bars.
- `summary.md` and `.json`: pooled yields, broad energy-window comparisons,
  statistical uncertainties, and the target window's seed-by-seed counts.
- `run_moments.json`: per-run event count, sum of counts, and sum of squared
  per-event counts used to compute pooled uncertainties.
- `inputs.json`: manifests for all included state/version/run combinations.

The original pre-fix native runs in `sulfur-comparison-001` are excluded, as
are all Eu-152 runs. Container runs in `002` are reused from `001` and are
counted only once. The earlier Geant4 11 rerun used the same seeds and is also
counted only once. Every included run has unique seeds relative to its peers.

Raw histogram counts are summed before normalization per beam primary.
The ROOT overview stores raw 2-keV histograms; its canvas shows 20-keV bins.
Individual ROOT event trees remain separate, so repeated event IDs across
independent runs cannot be mistaken for unique IDs in a merged tree.
Spectra use laboratory energies, without Doppler correction, resolution
smearing, or cross-crystal addback.

Uncertainty estimates treat the primary event as the sampling unit: all
crystals hit by the same event are counted together when estimating variance.
The 2150-keV excitation / 1000–1500-keV deposited-energy window was identified
before these ten new runs. Its new-only result provides an independent check;
the all-twelve result also includes the data that motivated the check.
Pointwise intervals and normal-approximation two-sided p values are reported.
Sparse windows with fewer than 20 combined counts omit the z value. Exploratory
energy-window p values additionally receive a Holm correction across all
populated windows in the three states, separately for each pool. These checks
do not establish equivalence for all configurations or physics processes.

## Reproduce aggregation

After all simulations and ROOT conversions have completed, source WSL ROOT and
run from the repository root, using a new output directory:

```bash
python3 analysis/aggregate_sulfur.py container-results \
  container-results/sulfur-ten-seed-summary
```

The aggregator refuses to overwrite an existing directory. It verifies event
IDs, event totals, unique seeds, input-file hashes, source-hash consistency,
and simulation logs before pooling. It reads exactly runs `002` through `013`.

## Interpretation

All 60 additional simulations completed, totaling six million new events.
The previously highlighted 2150-keV / 1000–1500-keV window has 47,248 entries
in Geant4 10 and 47,251 in Geant4 11 across the ten new runs: z = +0.010.
Its differences alternate in sign across the ten seeds (five positive and
five negative). This independent follow-up does not reproduce the earlier
2.79-standard-error excursion and supports a sampling-fluctuation explanation.

Across all twelve runs (1.2 million primaries per state/version):

| Excitation (keV) | Geant4 10 crystal entries | Geant4 11 crystal entries | Difference / standard error |
|---|---:|---:|---:|
| 1329 | 452,188 | 453,592 | +1.40 |
| 2150 | 438,905 | 440,017 | +1.09 |
| 2457 | 905,740 | 907,461 | +1.23 |

The largest populated energy-window difference is 1.68 standard errors in
the new-only pool and 1.57 in the all-twelve pool. None of the tested broad
energy windows is statistically significant at 0.05 after the reported Holm
correction. There is no statistically resolved discrepancy in these checks;
small differences below their sensitivity and untested physics remain possible.
