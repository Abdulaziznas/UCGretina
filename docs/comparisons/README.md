# Geant4 11 migration comparison results

This archive accompanies the emitted-gamma recording fix in `TrackingAction.cc`.
It compares the original Geant4 10.7.4 container with the Geant4 11.4.1 WSL
migration. The native tracking filter now recognizes both `Radioactivation`
and `RadioactiveDecay`. Identical-seed checks confirmed unchanged per-event
crystal energies after this bookkeeping correction.

## Main sulfur result

Ten additional independent seed sets completed 60 simulations and six million
new events. Including two previous independent sets gives 1.2 million beam
primaries per excitation state and Geant4 version.

- [Combined spectra, all twelve runs](sulfur-ten-seed-summary/all_twelve/comparison.pdf)
- [ROOT histograms and canvas, all twelve runs](sulfur-ten-seed-summary/all_twelve/spectra.root)
- [Independent ten-run spectra](sulfur-ten-seed-summary/new_ten/comparison.pdf)
- [ROOT histograms and canvas, ten new runs](sulfur-ten-seed-summary/new_ten/spectra.root)
- [Statistical report](sulfur-ten-seed-summary/summary.md)
- [Previously highlighted window across seeds](sulfur-ten-seed-summary/target_seed_followup.pdf)
- [Input provenance](sulfur-ten-seed-summary/inputs.json)
- [Per-run moments for reproducing uncertainties](sulfur-ten-seed-summary/run_moments.json)

The previously highlighted 2150-keV / 1000–1500-keV window has 47,248 entries
in Geant4 10 and 47,251 in Geant4 11 in the ten new runs (0.01 standard errors).
Combined crystal yields differ by 1.40, 1.09, and 1.23 standard errors for the
1329, 2150, and 2457 keV states. These tests do not resolve a systematic version
difference; they do not establish equivalence for all physics configurations.

## Earlier comparisons

- [Eu-152 initial comparison](gamma-comparison-001/comparison.pdf)
- [Eu-152 independent-seed comparison](gamma-comparison-002/seed_variation.md)
- [Sulfur comparison after the recording fix](sulfur-comparison-002/summary.md)
- [Sulfur independent-seed repeat](sulfur-comparison-003/seed_variation.md)
- [Per-event verification of the recording fix](sulfur-comparison-002/output_fix_verification.json)

`sulfur-comparison-001` preserves results before the recording fix, including
the zero emitted-gamma counts. Its native runs must not be treated as extra
independent statistics. The later pooled report describes which runs were used.

## Archive scope and reproduction

Reports, figures, input manifests, per-run summary counts, and pooled ROOT
histograms are committed. Large ASCII event files, individual event-tree ROOT
files, and the Docker image remain local under `container-results/` or Docker
storage. Paths beginning with `container-results/` inside preserved reports
refer to the original local run layout. `archive_manifest.json` maps each
published artifact to its original path and SHA256 hash.

Reproduction instructions and analysis code:

- [Geant4 10 container](../../containers/geant4-10/README.md)
- [Eu-152 comparison](../../analysis/README.md)
- [Sulfur comparison](../../analysis/SULFUR_COMPARISON.md)
- [Ten-seed follow-up](../../analysis/SULFUR_TEN_SEED_COMPARISON.md)

The source commit IDs in manifests identify the original tested checkout;
the saved patch and tracking-source hash identify the then-uncommitted fix.

Archive files retain their original bytes, including CSV line endings and patch
context whitespace, so published SHA256 values match the local evidence.
