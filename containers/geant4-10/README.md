# Original Geant4 10 comparison container

This image builds the original UCGretina backup, independently of the host's
Geant4 11 and ROOT installations. Container files live on the migration branch;
the application compiled inside the image is the original code.

**Status (10 September 2026):** built successfully with Docker Desktop through
WSL. All six application variants compiled; batch ROOT, PyROOT, and all six
100-event smoke cases passed as the host user's UID/GID. The local image is
`ucgretina:g4-10.7.4`. This validates basic execution, not physics equivalence
or graphical display forwarding.

The initial image build failed after ROOT printed its version because the
interpreted stream expression returned exit status 255. The check now explicitly
returns success only for ROOT version 6.28.12. Compilation and validation use
separate build layers. The application shared libraries produced by GNUmake
are retained in `/opt/ucgretina-lib`, which is added to `LD_LIBRARY_PATH`, before
the temporary object directories are removed.

| Component | Selected version |
| --- | --- |
| Ubuntu user space | 20.04, linux/amd64 |
| Compiler | GCC/G++ 9 from Ubuntu 20.04 packages |
| Geant4 | 10.7.4, built from source, multithreaded, C++17 |
| ROOT | 6.28.12, official Ubuntu 20 / GCC 9.4 binary, including PyROOT |
| Python | Ubuntu's Python 3.8 |
| UCGretina | `1271388e03ec9b3f8e055a988aeb4c2e4d2efedf` |
| Source branch reference | `backup/geant4.10-container-comparison` |
| Application build | Original GNUmakefile, no migration patches |
| Visualization | Original terminal and OpenGL/X11 drivers; Geant4 Qt disabled |

A container supplies Ubuntu libraries and tools; it shares the Docker/WSL Linux
kernel. It does not install a separate kernel or change the host Ubuntu version.
The legacy compiler/OS choice is for reproducing the old simulation environment.
ROOT 6.28.12 is a selected compatible analysis environment, not a claim about the
exact ROOT version used by the original authors. GRUTinizer and the separate
GrROOT analysis project are not installed; example analysis depending on them
needs those packages in addition to ROOT.

Geant4 and ROOT archives have checked SHA256 values in the Dockerfile.
Geant4 downloads the datasets specified by its own 10.7.4 CMake definitions,
including G4NDL 4.6, G4EMLOW 7.13, PhotonEvaporation 5.7, RadioactiveDecay 5.6,
and G4PARTICLEXS 3.1.1, with the upstream dataset checksums.
The application is pinned to a full commit, not a moving branch tip.
Ubuntu's image tag and package repositories are not pinned to immutable snapshots,
so these inputs alone do not promise byte-for-byte identical future builds.
The image records installed package versions in `/opt/container/ubuntu-packages.txt`.

**One-time Docker setup on Windows/WSL**

Install and start Docker Desktop for Windows, using Linux containers. In its
settings, enable **General → Use the WSL 2 based engine**, then
**Resources → WSL Integration → Ubuntu** (your distribution name may differ).
Apply the settings and reopen your WSL terminal. Check:

```bash
docker version
docker info
```

Both client and server must be available. Docker Desktop's WSL integration
provides the command and daemon connection; a second Docker Engine installation
inside Ubuntu is unnecessary. Once these checks work, the agent can use the same
commands to build and test the image.

**Build**

Run from the UCGretina repository root:

```bash
./containers/geant4-10/build.sh
```

The tag is `ucgretina:g4-10.7.4`. The first build downloads Geant4, ROOT and physics
datasets and compiles Geant4 and six application variants. Allow several GB of
disk space and substantial build time. Compilation uses two jobs by default to
limit memory demand. To change it, or initially build just the standard executable:

```bash
BUILD_JOBS=4 BUILD_VARIANTS=standard ./containers/geant4-10/build.sh
```

Supported variant names are `standard pol lh lh-pol scan scan-pol`.
Each has a separate object directory to prevent reuse of objects compiled with
different flags. The defaults do not enable `NEUTRONS`, `HIGHMULT` or `CACHETEXT`;
neutron physics is a separate comparison configuration that is not provided here.

No files from the active migration source tree are copied into the application.
Only the small container-definition directory is sent to Docker. The build fetches
the original public commit from GitHub, so it needs internet access but no GitHub
login. It does not check out another branch in your host repository.

**Run checks**

```bash
./containers/geant4-10/run.sh cat /opt/container/versions.txt
./containers/geant4-10/run.sh geant4-config --version
./containers/geant4-10/run.sh root-config --version
./containers/geant4-10/run.sh python3 /opt/container/smoke.py
```

The image build also checks that batch ROOT starts. The smoke command checks
PyROOT, then the original standard, polarization, LH, scanning, Eu-152, and
background examples for whichever relevant binaries were built. It uses two
workers, fixed seeds, and 100 events per case. Combined LH/Pol and Scan/Pol
executables are built by default but are not separately covered by these cases.
Failures return a nonzero status. Smoke tests are execution checks, not a
high-statistics physics comparison.

Results persist in `container-results/geant4-10/smoke/`, including per-case
`run.log`, generated macros, ASCII output, and `summary.json`. Repeating the smoke
command replaces the corresponding smoke-case directories. Use a different
output directory to retain previous runs:

```bash
UCGRETINA_OUTPUT_DIR="$PWD/container-results/run-02" \
  ./containers/geant4-10/run.sh python3 /opt/container/smoke.py
```

**Use the original terminal workflow**

```bash
./containers/geant4-10/run.sh
```

Inside the container, the Geant4, legacy make and ROOT environments are already
loaded. The original source and examples are at `/opt/UCGretina`, executables are
on `PATH`, and `/work` is the persistent host results directory. For an example:

```bash
mkdir -p /work/eu152
cp -aL /opt/UCGretina/examples/sources/eu152/. /work/eu152/
cd /work/eu152
UCGretina eu152.mac
```

The original macro requests one million events; edit the copied macro to change
that before running. To use the command prompt instead, run `UCGretina` without
arguments and enter `/control/execute eu152.mac`. The executable is the unchanged
Geant4 10 program; it has no migration `--vis` option. Use `root -l` for ROOT and
`exit` to leave the container shell. The container is removed on exit; files
written under `/work` remain on the host. Keep important output under `/work`.

The runner maps your host UID/GID so results belong to you. It mounts only the
results directory. Do not mount your Geant4 11 libraries over `/opt/geant4`, and do
not overwrite `/opt/UCGretina` with the migration branch when measuring the baseline.
The runner does not forward a graphical display. Original visualization needs
additional X11/WSLg display forwarding, which has not been configured or tested.

**Comparison procedure**

Use matching geometry, beam/source inputs, output settings, event counts, and
thread counts for both versions. Save the exact macros, seeds, datasets, and code
commit with each run. Equal random seeds do not guarantee identical event sequences
across Geant4 versions. Compare spectra, efficiencies, and angular distributions
statistically using sufficient events; do not use the 100-event smoke results as
physics validation. The existing migration review remains applicable.

To preserve the built image itself for a later comparison machine:

```bash
docker image inspect ucgretina:g4-10.7.4 --format '{{.Id}}'
docker save --output ucgretina-g4-10.7.4.tar ucgretina:g4-10.7.4
# On the other machine:
docker load --input ucgretina-g4-10.7.4.tar
```

Store the archive outside Git. The container files remain uncommitted;
no image has been pushed to a registry.

Sources: [Geant4 10.7.4](https://geant4.web.cern.ch/download/10.7.4.html),
[Geant4 10.7 tested compilers](https://www.geant4.org/download/release-notes/notes-v10.7.0.html),
[ROOT 6.28.12 distributions](https://root.cern.ch/releases/release-62812/),
[Docker Desktop WSL integration](https://docs.docker.com/desktop/features/wsl/).
