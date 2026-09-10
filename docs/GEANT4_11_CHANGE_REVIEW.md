# Geant4 11 migration: change summary and human review

Review date: 10 September 2026. Baseline: commit
`1271388e03ec9b3f8e055a988aeb4c2e4d2efedf` on `geant4.10`, after synchronization
with the fork. Scope: the current uncommitted migration, CMake build, tests,
and terminal/Qt visualization changes, including newly created files.
This is a review of the final working tree, not a list of intermediate edits.

**There are real behavior changes in this migration.** Many edits are equivalent
API replacements, including the `contains()` changes shown in the IDE. However,
the complete migration should not be approved as a syntax-only patch.

“Approval” below means a recommended project-owner or physics-expert decision
before production use. It is not an approval rule imposed by Git or Geant4.
The changes are already implemented locally; this document does not record
scientific approval, commit them, or publish them.

**1. Equivalent API and compilation changes**

| Files | Change | Effect and review level |
| --- | --- | --- |
| [SteppingAction.cc](../src/SteppingAction.cc), [TrackerGammaSD.cc](../src/TrackerGammaSD.cc), [TrackingAction.cc](../src/TrackingAction.cc), [EventAction.cc](../src/EventAction.cc), string predicates in [Reaction.cc](../src/Reaction.cc) | `text.contains(value)` → `G4StrUtil::contains(text, value)` | Equivalent substring/character test. Normal code review; no new physics decision. |
| [Experimental_Hall.cc](../src/Experimental_Hall.cc) | `G4VisAttributes::Invisible` → `G4VisAttributes::GetInvisible()` | Keeps the hall invisible. Does not remove material or disable particle transport. |
| [Outgoing_Beam.hh](../include/Outgoing_Beam.hh) | Removes an unused, unavailable radioactive-decay messenger include | Compilation fix; does not remove the radioactive-decay process. |
| [ScanningTable.hh](../include/ScanningTable.hh), [ScanningTable.cc](../src/ScanningTable.cc) | Moves `CADMesh.hh` from the public header to the implementation | Prevents CADMesh macros such as `Next()` from interfering with other headers. Mesh-loading calls are unchanged. |
| All six `UCGretina*.cc` entry points | Includes `globals.hh` before testing `G4MULTITHREADED` | Makes the installed Geant4 configuration visible before choosing run-manager headers. The existing choice of an MT run manager and core-count default is retained. |

For the specific screenshot:

```cpp
// Before
volume2->GetName().contains("BeamTube")

// After
G4StrUtil::contains(volume2->GetName(), "BeamTube")
```

Both ask whether the name contains `BeamTube`. The searched names, case
sensitivity, `||`/`&&` operators, negations, and bodies of the conditions were
preserved. Thus these edits do not change when a track is killed, how Clover
segments are selected, or when beta/cache information is recorded. The local
Geant4 11.4.1 `G4String.icc` explicitly implements the deprecated member functions
by calling `G4StrUtil::contains`, providing direct evidence for this equivalence.
This does not imply that the surrounding pre-existing algorithms have been fully
validated.

**2. Small defensive behavior change**

In [Gretina_Array.cc](../src/Gretina_Array.cc), five geometry-file setters now use
`!nome.empty() && nome.front() == '/'` instead of `nome(0) == '/'`.
For nonempty paths, the intended absolute-versus-relative-path decision is the
same. Empty input is explicitly checked before reading the first character.
The rest of path handling is unchanged; this does not add a full empty-path
validation rule. Normal maintainer review is sufficient.

**3. Changes needing physics or geometry review**

| ID | Change and location | Why it matters | Recommended approval before production |
| --- | --- | --- | --- |
| P1 | `PhysicsList` constructor sets the radioactive-decay time threshold to `1e60 year` for Geant4 ≥11.2 | An explicit physics configuration, intended to preserve long-lived calibration-source decays. It overrides the newer short default cutoff, rather than merely renaming an API. | Physics owner should accept this default for the intended experiments. |
| P2 | `PhysicsList::ConstructProcess` disables the combined gamma process when `usePolar` is true; it replaces individual gamma processes using backward iteration | Changes process construction and traversal. Backward traversal avoids skipping entries when removing processes. Polarized photoelectric, Compton, and conversion processes are registered individually. | Physics owner should validate polarized interactions and angular distributions. |
| P3 | [ITDecay.cc](../src/ITDecay.cc) keeps the custom `G4ITDecay::DecayIt` but removes locally supplied constructors, destructor, and `DumpNuclearInfo` | Initialization and bookkeeping now come from the installed Geant4 library. For ≥11.2, decay uses `theParent`; earlier versions use `CheckAndFillParent()` and `G4MT_parent`. | Expert review of the custom decay override and its compatibility with each supported Geant4 release. |
| P4 | `ITDecay::DecayIt` searches `level->NumberOfTransitions()` instead of `lman->NumberOfTransitions()` | An algorithmic correction: search the selected level's transitions. This is a genuine loop-bound change, not syntax. | Physics/code reviewer should check transition selection and failure handling. |
| P5 | `SpinTwo(index)` → `std::abs(TwoSpinParity(index))` in `Reaction` and `ITDecay` | Intended to extract spin magnitude from the signed spin/parity representation. It affects the required number of magnetic substates and coefficients used for Stokes parameters. | Validate positive/negative parity, spin-zero states, and any unknown-spin conventions in the level data. |
| P6 | Generic `G4HadronInelasticProcess("inelastic", particle)` and `G4NeutronCaptureProcess` in `PhysicsList::ConstructHad` | Required API migration with intended equivalent particle associations. Existing model registrations, energy ranges, and cross-section registrations were retained in the application code. Library/data behavior can still differ. | Hadronic-physics reviewer should validate neutron spectra, capture response, and other enabled hadrons. Applies when `NEUTRONS` is enabled. |
| G1 | New `CConvexPolyhedron::BoundingLimits` in [CConvexPolyhedron.cc](../src/CConvexPolyhedron.cc) and its [header](../include/CConvexPolyhedron.hh) | Computes local axis-aligned minima/maxima from the current vertices, replacing an inherited unimplemented/infinite-bounds fallback. Bounds can affect Boolean geometry and spatial calculations as well as visualization. | Geometry owner should check rotated/translated Boolean solids, moved planes, overlaps, and crystal/dead-layer volumes. |

P1 can be overridden after `/run/initialize` using
`/process/had/rdm/thresholdForVeryLongDecayTime <value> <unit>`.
The motivation is documented in the
[Geant4 11.2 release notes](https://geant4.web.cern.ch/download/release-notes/notes-v11.2.0.html).

P2 is controlled by `/PhysicsList/SetGammaPolarization`; it is distinct from the
compile-time `POL` option that enables the custom nuclear-polarization code.
Checking that polarized process names appear in a log does not establish correct
polarization transport.

P3 preserves the existing Stokes-parameter formulas, but changes their supporting
initialization. The project still defines a method belonging to a Geant4 class
inside the application. That dependence on library internals and symbol linkage
requires more care than an ordinary application subclass.

**Unresolved issue in P4:** if no transition matches `dIndex`, the code reaches
`NumberOfTransitions()` and still uses `tIndex` to access transition data.
There is no explicit no-match check. The surrounding unchecked-access risk
predates this migration and remains unresolved. A reviewer should decide on
handling before approving the polarized-decay path; the passing example does
not exercise all possible level schemes. This documentation review has not
changed the implementation to conceal or resolve that issue.

**4. Build and user-workflow changes**

| Files | Final behavior | Review needed |
| --- | --- | --- |
| [CMakeLists.txt](../CMakeLists.txt) | Adds CMake ≥3.16, C++17, and Geant4 ≥11 discovery. Builds one selected variant per build directory; exposes geometry, polarization, neutron, visualization, cache, and multiplicity options. Rejects simultaneous `LHTARGET` and `SCANNING` (the old makefile gave LHTARGET precedence). | Maintainer acceptance of the supported build/dependency policy and option semantics. |
| [cmake/Git_Hash.hh.in](../cmake/Git_Hash.hh.in) | Generates build metadata in the build directory at CMake configuration time. Does not overwrite the tracked `include/Git_Hash.hh`. | Reconfigure after changing commits/branches. The printed hash does not identify uncommitted source edits. |
| All six entry points: [standard](../UCGretina.cc), [LH](../UCGretina_LH.cc), [Pol](../UCGretina_Pol.cc), [LH Pol](../UCGretina_LH_Pol.cc), [Scan](../UCGretina_Scan.cc), [Scan Pol](../UCGretina_Scan_Pol.cc) | No arguments explicitly selects the `csh` terminal session. `--vis macro.mac` creates Qt before visualization, executes the macro, then stays interactive. One ordinary macro argument remains batch mode. Adds help/argument validation and returns failure when the top-level `ApplyCommand` reports failure. | Maintainer approval of CLI behavior. Explicit `csh` may lose the old optional `tcsh` editing/history behavior. Existing batch scripts should be checked for argument and exit-code assumptions. |
| [VisManager.hh](../include/VisManager.hh), [VisManager.cc](../src/VisManager.cc) | Replaces hard-coded driver registration with `G4VisExecutive`; the source file now only includes the header. | Accept installed-driver selection. This changes available/default viewers, not physics processes. `OGLSX` is unavailable in the user's observed driver list; ToolsSG is available. |
| [vis/visQt.mac](../vis/visQt.mac) | Enables capsules, cryostats, target, sled, and beam tube; initializes the run; displays current geometry; does not call `beamOn`. | Geometry/viewing choices need user acceptance. These commands construct material if this setup is later used to simulate events. |
| [vis/gui.mac](../vis/gui.mac) | Minimal initialize/display macro for an existing interactive session. | Its opening comment is stale: no-argument startup now selects the terminal, not Qt. Use `--vis vis/gui.mac` to request Qt directly. |
| [tests/geant4_smoke.py](../tests/geant4_smoke.py), CMake test registration | Adds bounded smoke tests, copies example inputs, uses 100 events and two workers on MT builds, and retains output under each build directory. | Accept as execution checks, not physics certification. |
| [README.md](../README.md), [.gitignore](../.gitignore) | Documents the migration and adds `/build*/` to ignored artifacts. | Documentation/housekeeping review. |

The GNUmakefile itself was not changed or validated for this migration. CMake
is the tested route. ROOT analysis utilities were not modified or tested;
the simulation does not add a ROOT link dependency. No geometry data files,
material definitions, production cuts, reaction kinematics formulas, or output
record formats were edited as part of this migration. This statement concerns
the application diff; Geant4 and its datasets can change numerical results even
where the application source is unchanged.

**5. Evidence and its limits**

Builds and initial smoke tests used Geant4 11.4.1 on Linux:

| Build directory | Configuration | Smoke checks passed |
| --- | --- | --- |
| `build` | Standard, visualization enabled | standard in-beam, Eu-152 source, background, ATIMA: 4 |
| `build-pol` | `POL=ON` | Same four plus polarized in-beam: 5 |
| `build-lh-pol` | `LHTARGET=ON`, `POL=ON`, `WITH_VIS=OFF` | LH in-beam: 1 |
| `build-scan-pol` | `SCANNING=ON`, `POL=ON`, `NEUTRONS=ON` | scanning and 1 MeV neutron source: 2 |

All 12 checks passed after the physics/API migration. Following the final
terminal/`--vis` changes, all four configurations were rebuilt and the four
standard checks were rerun successfully. The optional eight checks were not
rerun after those CLI-only edits. CLI help/missing-argument handling and terminal
startup were checked separately. The new visualization macro was exercised
with a substituted VRML file driver for a headless check; that does not verify
the Qt viewer. The user demonstrated an X11 viewer, but final Qt interaction
has not been visually verified by the agent.

The automated checks require successful completion, exactly 100 recorded events,
and absence of selected error messages. Eu-152 also requires at least one detected
event; the polarized case requires the three polarized process names. They do
not compare spectra, efficiencies, Stokes values, branching ratios, energy
conservation, or angular distributions against reference measurements or
Geant4 10.7.4. The source-hit test alone is not a quantitative validation of P1.
No high-statistics baseline comparison, exhaustive overlap test, concurrency
validation, or cache replay validation was performed. Other Geant4 11.x releases,
a serial Geant4 installation, and every build-option combination remain untested.

**6. Suggested sign-off record**

The project owner can use the following decisions to approve a concrete scope.
These are recommendations, not claims that approval has already been obtained.

| Decision | Suggested reviewer | Status |
| --- | --- | --- |
| Accept equivalent API edits and defensive path handling | C++ maintainer | Pending explicit review |
| Accept CMake, metadata, terminal/Qt behavior, and macros | Project owner | Pending explicit review |
| Accept decay cutoff and validate calibration sources (P1) | Physics owner | Pending |
| Review polarized process registration, decay internals, spin conversion, and unresolved transition handling (P2–P5) | Nuclear-polarization expert | Pending |
| Accept hadronic migration for neutron-enabled production (P6) | Hadronic-physics expert | Pending if used |
| Validate bounds and detector geometry (G1) | Geometry owner | Pending |
| Accept Geant4 11.4.1 production results after comparisons to established spectra/efficiencies/angular distributions | Simulation owner | Pending |

Recommended disposition: keep the migration available for development and
review, and obtain physics/geometry sign-off for the configurations that will
be used for production. Approval of compilation or GUI behavior alone does
not approve the changed physics configuration.
