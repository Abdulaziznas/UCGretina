#!/usr/bin/env python3
"""Run a bounded example against a CMake-built executable, retaining its log."""
import argparse
from pathlib import Path

import benchmark


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", required=True)
    parser.add_argument("--case", required=True,
                        choices=["standard", "lh", "pol", "scan", "sources",
                                 "background", "atima", "neutron"])
    parser.add_argument("--threads", type=int)
    args = parser.parse_args()
    binary = Path(args.binary).resolve()
    benchmark.TMP_DIR = str(binary.parent / "smoke")
    case_index = dict(standard=0, lh=1, pol=2, scan=3, sources=4,
                      background=5, atima=0, neutron=0)
    _, _, macro, example, files = benchmark.SMOKE_CASES[case_index[args.case]]
    workdir = Path(benchmark.setup_workdir(args.case, example, files))
    example = str(Path(benchmark.PROJECT_ROOT) / example)
    benchmark.write_base_macro(macro, example, "/Output/Filename output.out\n", workdir)
    base = workdir / macro
    text = base.read_text().replace(
        "../../../cadModels", str(Path(benchmark.PROJECT_ROOT) / "cadModels"))
    if args.case == "atima":
        text = text.replace("/run/initialize", "/PhysicsList/SelectEmPhysics "
                            "emstandard_opt4_Atima\n/run/initialize", 1)
    if args.case == "neutron":
        text = ("/run/initialize\n/Experiment/RunSource\n"
                "/Experiment/Source/Set neutron\n"
                "/Experiment/Source/setEnergy 1 MeV\n"
                "/Output/Filename output.out\n")
    base.write_text(text)
    wrapper = workdir / "run.mac"
    threads = f"/run/numberOfThreads {args.threads}\n" if args.threads else ""
    wrapper.write_text(threads + f"/control/execute {macro}\n/run/beamOn 100\n")
    stdout, stderr, code = benchmark.run_sim(str(binary), str(wrapper), str(workdir))
    log = stdout + "\n" + stderr
    (workdir / "run.log").write_text(log)
    ok, message = benchmark._check_run_criteria(args.case, stdout, stderr, code)
    errors = ("COMMAND NOT FOUND", "***** Illegal", "Batch is interrupted",
              "Not implemented for solid: CConvexPolyhedron")
    ok = ok and not any(error in log for error in errors)
    outputs = list(workdir.glob("output*.out"))
    counts = [benchmark.count_detected_and_simulated(str(path)) for path in outputs]
    simulated = sum(count[1] for count in counts)
    detected = sum(count[0] for count in counts)
    ok = ok and simulated == 100
    # A long-lived Eu-152 source must actually decay and produce detector hits.
    if args.case == "sources":
        ok = ok and detected > 0
    if args.case == "pol":
        ok = ok and all(process in log for process in ("pol-phot", "pol-compt", "pol-conv"))
    print(message)
    print(f"Simulated: {simulated}; detected: {detected}; log: {workdir / 'run.log'}")
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
