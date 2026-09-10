# Comparing the Geant4 10 and 11 gamma spectra

Published reports, plots, and pooled ROOT histograms are available in the
[comparison archive](../docs/comparisons/README.md). Full event data remain local.

Inside the comparison container, UCGretina source is in `/opt/UCGretina`;
executables are in `/opt/ucgretina-bin` and already on `PATH`:

```bash
command -v UCGretina
ls /opt/UCGretina/examples
```

`/work` is the output directory shared with WSL. The container's home directory
is not the source checkout. The WSL executable is `build/UCGretina` in this repository.

## Completed comparison

The initial run is saved in `container-results/gamma-comparison-001/`:

- `g4-10/spectrum.root`: Geant4 10.7.4, container ROOT 6.28.12.
- `g4-11/spectrum.root`: Geant4 11.4.1, WSL ROOT 6.40.00.
- `comparison.png`, `comparison.pdf`: overlaid spectra and their ratio.
- `comparison.root`: normalized histograms, ratio graph, and canvas.
- `spectrum_bins.csv`, `comparison.json`: bin values and run totals.
- Each run retains its macro, geometry, ASCII output, and `simulation.log`.
- `inputs.json` records input hashes, source commits, seeds, and settings.

UCGretina writes its original ASCII output; `gamma_spectra.py` converts it to
ROOT without changing the simulation. Both runs use the same converter.
The ROOT files contain `crystal_energy`, `emitted_energy`, `event_energy`,
an `events` tree including zero-deposit events, and a `crystal_hits` tree.
`metadata` records run provenance. Crystal energies sum the interaction points
within each crystal and event. The primary comparison uses 2 keV bins from
0 to 1600 keV, normalized per simulated primary. It applies no detector
resolution smearing and no cross-crystal addback. The separate `event_energy`
histogram sums all crystal deposits in a detected event.

The input is the supplied `examples/sources/eu152/eu152_gammas.mac` source,
with the NSCL G120C4 geometry, 100,000 primaries, two workers, seeds 12345/67890,
and explicit `emstandard_opt4` in both versions. This is an empirical
single-gamma source, not a full Eu-152 decay cascade. Thus it checks gamma
transport and detector response; it does not validate the migrated radioactive
decay or reaction logic. Identical seeds do not guarantee identical events
between Geant4 versions. Differences also include the container/native compiler
and dependency environments. Ratio error bars are approximate Poisson errors;
crystals hit in the same event can be correlated. Sparse bins have large errors.

## Repeat with a new output directory

Run these commands from the repository root in WSL, after Docker integration is
working. Use a new directory for each run; preparation refuses to overwrite one.
The Geant4 10 image must already have been built (see
[container instructions](../containers/geant4-10/README.md)).

```bash
source /home/eren511/geant4/install/bin/geant4.sh
source /home/eren511/root/install/bin/thisroot.sh
cmake -S . -B build
cmake --build build -j 4

export COMPARISON_DIR="$PWD/container-results/gamma-comparison-002"
python3 analysis/prepare_gamma_comparison.py "$COMPARISON_DIR" --events 100000

UCGRETINA_OUTPUT_DIR="$COMPARISON_DIR" ./containers/geant4-10/run.sh \
  bash -c 'cd /work/g4-10 && UCGretina run.mac > simulation.log 2>&1'

export UCGRETINA_NATIVE="$PWD/build/UCGretina"
(cd "$COMPARISON_DIR/g4-11" && "$UCGRETINA_NATIVE" run.mac > simulation.log 2>&1)

UCGRETINA_OUTPUT_DIR="$COMPARISON_DIR" ./containers/geant4-10/run.sh \
  python3 /work/gamma_spectra.py convert /work/g4-10 --label 'Geant4 10.7.4 container'
python3 analysis/gamma_spectra.py convert "$COMPARISON_DIR/g4-11" \
  --label 'Geant4 11.4.1 WSL'
python3 analysis/gamma_spectra.py compare "$COMPARISON_DIR"
```

The converter checks that every expected event is present exactly once and
that detected/emitted record IDs agree. Failed or incomplete runs must be
resolved before interpreting spectra.

To open the completed overlay interactively in WSL:

```bash
root -l container-results/gamma-comparison-001/comparison.root
```

Then at the ROOT prompt:

```cpp
((TCanvas*)_file0->Get("comparison"))->Draw();
```

Generated data are ignored by Git. Keep the output directory separately if you
need to archive the comparison.

## Initial results (100,000 primaries each)

| Quantity | Geant4 10.7.4 | Geant4 11.4.1 |
|---|---:|---:|
| Generated gammas | 100,000 | 100,000 |
| Events with detected energy | 17,636 | 17,653 |
| Crystal entries | 20,566 | 20,542 |
| 120–124 keV window | 2,170 | 2,183 |
| 242–248 keV window | 645 | 647 |
| 342–348 keV window | 1,607 | 1,619 |
| 776–782 keV window | 526 | 492 |
| 962–968 keV window | 467 | 499 |
| 1110–1116 keV window | 413 | 419 |
| 1406–1412 keV window | 487 | 523 |

Windows include the lower edge and exclude the upper edge. They are raw window
counts including continuum, not fitted/background-subtracted photopeak areas.
The integrated crystal yield changes by −0.117%. The spectra visually overlap
closely; the selected window differences are compatible with ordinary counting
fluctuations at this sample size. This does not establish equivalence across
all energies, source models, or physics processes. Increase statistics and test
decay/reaction-specific cases before approving the migration's physics changes.

## Different-seed repeat

The second pair is in `container-results/gamma-comparison-002/`, with 100,000
primaries per version and the same geometry/physics settings:

| Version | Seeds | Detected events | Crystal entries |
|---|---|---:|---:|
| Geant4 10.7.4 | 314159, 271828 | 17,428 | 20,290 |
| Geant4 11.4.1 | 8675309, 1618033 | 17,446 | 20,250 |

`comparison.png`/`.pdf` overlay the new spectra. `seed_variation.md` and
`seed_variation.json` compare the versions and each version's new run with its
previous run. Standard errors use primary-event sample variances, retaining
correlations among crystal counts from the same event.

The new versions differ by 0.19 standard errors in total crystal yield; all
seven selected energy-window differences are below 1.8 standard errors.
Within-version total yields move by 1.30 (Geant4 10) and 1.38 (Geant4 11)
standard errors between seeds. One same-version window, 1406–1412 keV in
Geant4 10, moves by 2.44 standard errors. An isolated excursion across multiple
checks can occur through sampling variation; it is not evidence of a
version-specific change. These results are consistent with ordinary Monte
Carlo variation, but two runs per version do not establish physics equivalence.

To prepare a further repeat with explicit seeds:

```bash
python3 analysis/prepare_gamma_comparison.py container-results/gamma-comparison-003 \
  --events 100000 --seeds-g4-10 112233 445566 --seeds-g4-11 778899 101112
```

Use the simulation/conversion commands above with that new directory, then:

```bash
python3 analysis/check_seed_variation.py \
  container-results/gamma-comparison-001 container-results/gamma-comparison-003
```

## Sulfur-44 reaction validation

See [SULFUR_COMPARISON.md](SULFUR_COMPARISON.md) for the three-state in-beam
comparison, the emitted-gamma output defect it exposed, and repeat commands.
The corrected results are in `container-results/sulfur-comparison-002/`.
