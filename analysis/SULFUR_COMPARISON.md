# Sulfur-44 migration comparison

Published reports, plots, and pooled ROOT histograms are available in the
[comparison archive](../docs/comparisons/README.md). Full event data remain local.

The three supplied `examples/inbeam/fit/s44_*.mac` cases were run in the
Geant4 10.7.4 container and in native WSL Geant4 11.4.1, with 100,000 beam
primaries per excitation state. Geometry, level data, beam/target parameters,
and electromagnetic selection match. Each version/state uses independent
seeds recorded in `inputs.json`.

The original results are in `container-results/sulfur-comparison-001/`.
The corrected comparison is in `container-results/sulfur-comparison-002/`:

- `sulfur_comparison.pdf` and `.png`: all three states, detector and emitted spectra.
- `summary.md` and `.json`: detector yields and broad-window statistical comparisons.
- `<state>/g4-10/spectrum.root` and `<state>/g4-11/spectrum.root`: individual ROOT outputs.
- `<state>/comparison.pdf` and `.root`: individual-state overlay and bin ratios.
- `output_fix_verification.json`: per-event verification of the output-filter fix.
- `tracking-output-fix.patch`: exact native source correction used in the repeat.

All crystal spectra sum deposits within each crystal/event. They are laboratory
energies, without Doppler correction, resolution smearing, or cross-crystal
addback. The range is 0–4000 keV in 2 keV bins; the combined overview rebins to
20 keV for readability. Spectra are normalized per simulated beam primary.
The three states are kept separate: no population weights are assumed.

## Defect found and corrected

The initial Geant4 11 runs recorded zero emitted gammas despite normal crystal
deposits. The installed `G4Radioactivation.hh` aliases `G4Radioactivation` to
`G4RadioactiveDecay`, whose default process name is `RadioactiveDecay`.
UCGretina's tracking filter only accepted `Radioactivation`.
`TrackingAction.cc` now accepts both names. This restores emitted-gamma
bookkeeping and the information used by derived output flags, without changing
the registered physics or reaction kinematics.

Only the native runs were repeated after this correction, with exactly the
same seeds; the original container outputs were reused. Every per-event
crystal energy record was identical before/after the correction across all
300,000 native events. Recorded emitted gamma counts were restored to
approximately 100,000 for each single-transition case and 200,000 for the
2457 keV cascade. Counts are the recorded truth output, not a separate proof
that all physical emission channels have been recorded.

## Repeat

From the repository root in WSL, with Geant4/ROOT sourced and Docker available:

```bash
cmake --build build -j 4
export SULFUR_DIR="$PWD/container-results/sulfur-comparison-003"
export UCGRETINA_NATIVE="$PWD/build/UCGretina"
python3 analysis/prepare_sulfur_comparison.py "$SULFUR_DIR" --events 100000

UCGRETINA_OUTPUT_DIR="$SULFUR_DIR" ./containers/geant4-10/run.sh bash -c '
  for state in 1329 2150 2457; do
    (cd /work/$state/g4-10 && UCGretina run.mac > simulation.log 2>&1) || exit
    python3 /work/gamma_spectra.py convert /work/$state/g4-10 --label "Geant4 10.7.4 container" || exit
  done'

for state in 1329 2150 2457; do
  (cd "$SULFUR_DIR/$state/g4-11" && "$UCGRETINA_NATIVE" run.mac > simulation.log 2>&1) || break
  python3 analysis/gamma_spectra.py convert "$SULFUR_DIR/$state/g4-11" --label 'Geant4 11.4.1 WSL' || break
  python3 analysis/gamma_spectra.py compare "$SULFUR_DIR/$state" || break
done
python3 analysis/compare_sulfur.py "$SULFUR_DIR"
```

The preparer refuses to overwrite an existing directory. It checks local input
files against the backup commit, then stages identical inputs except for seeds.
The ROOT converter validates unique complete event IDs and S800/detector/emitted
record consistency. S800 records are checked for event alignment and retained in
the ASCII output; their kinematic fields are not converted to ROOT trees by this
analysis. These tests cover the standard executable, not the polarized variant.

## Result

| Excitation (keV) | Crystal entries, Geant4 10 | Crystal entries, Geant4 11 | Difference / standard error |
|---|---:|---:|---:|
| 1329 | 37,904 | 38,349 | +1.53 |
| 2150 | 36,487 | 36,394 | −0.32 |
| 2457 | 75,295 | 75,752 | +1.13 |

The laboratory detector and emitted spectra overlap closely. Populated
500 keV detector windows differ by at most 1.56 estimated standard errors.
Sparse windows are reported as counts without a normal-approximation z value.
This is consistent with statistical variation in these tests, not proof of
full migration equivalence. No polarized decay, angular-correlation, lifetime
fit, or Doppler-reconstruction validation is implied.

## Independent-seed repeat

`container-results/sulfur-comparison-003/` repeats all six simulations with
100,000 primaries each, using the corrected native executable. It adds
2,000,000 to every seed in the previous pair, with each version/state retaining
a distinct seed pair. Physical inputs and the native tracking source hash
were checked against `sulfur-comparison-002`; only seeds changed.

The directory contains the same ROOT files and overview plots as above, plus
`seed_variation.md` and `.json`. These compare the new versions with each other
and each version's new run with its previous run, separately for all three
excitation states. Uncertainties use primary-event sample variances, including
within-event crystal correlations. Sparse windows do not receive a normal
approximation z value. The multiple window checks are exploratory.

For another repeat, prepare a fresh directory with a different offset, then
use the run/conversion commands above:

```bash
python3 analysis/prepare_sulfur_comparison.py container-results/sulfur-comparison-004 \
  --events 100000 --seed-offset 4000000
```

After the simulations and ROOT conversion:

```bash
python3 analysis/compare_sulfur.py container-results/sulfur-comparison-004
python3 analysis/check_sulfur_seed_variation.py \
  container-results/sulfur-comparison-003 container-results/sulfur-comparison-004
```

Results for the independent-seed repeat:

| Excitation (keV) | Geant4 10 crystal entries | Geant4 11 crystal entries | Difference / standard error |
|---|---:|---:|---:|
| 1329 | 37,648 | 37,640 | −0.03 |
| 2150 | 36,711 | 36,912 | +0.68 |
| 2457 | 75,798 | 75,521 | −0.68 |

Total yields agree closely between versions. The largest populated-window
version difference is +2.79 standard errors in the 1000–1500 keV laboratory
window for the 2150 keV state. The corresponding previous-pair difference was
−0.02 standard errors. A same-version comparison also shows a −2.44-standard-error
change in Geant4 11's total crystal yield for the 1329 keV state. Such excursions
can occur across multiple comparisons; this repeat alone cannot identify a
systematic effect. Higher statistics would be needed to resolve small spectral
differences. No global equivalence test or multiple-testing correction was
applied.

## Ten additional seed sets

See [SULFUR_TEN_SEED_COMPARISON.md](SULFUR_TEN_SEED_COMPARISON.md) for the
60-simulation follow-up and pooled analysis of twelve independent runs per
state/version.
