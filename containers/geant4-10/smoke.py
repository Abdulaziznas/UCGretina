#!/usr/bin/env python3
"""Exercise the immutable Geant4 10 source using its original example macros."""
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import sys

SOURCE = Path('/opt/UCGretina')
OUTPUT = Path('/work/smoke')


def main():
    OUTPUT.mkdir(parents=True, exist_ok=True)
    versions = {}
    for tool in ('geant4-config', 'root-config'):
        versions[tool] = subprocess.check_output([tool, '--version'], text=True).strip()
    assert versions['geant4-config'] == '10.7.4', versions
    assert versions['root-config'] == '6.28/12', versions
    subprocess.run(['python3', '-c',
                    'import ROOT; h = ROOT.TH1D("check", "check", 10, 0, 10); '
                    'h.Fill(5); assert h.GetEntries() == 1'], check=True)
    spec = importlib.util.spec_from_file_location('baseline_benchmark', SOURCE / 'tests/benchmark.py')
    benchmark = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(benchmark)
    benchmark.TMP_DIR = str(OUTPUT)
    results = []
    for name, binary_name, macro, example, support_files in benchmark.SMOKE_CASES:
        binary = shutil.which(binary_name)
        if binary is None:
            print('SKIP {}: {} was not built'.format(name, binary_name), flush=True)
            continue
        workdir = Path(benchmark.setup_workdir(name, example, support_files))
        benchmark.write_base_macro(macro, str(SOURCE / example),
                                   '/Output/Filename output.out\n', str(workdir))
        base = workdir / macro
        base.write_text(base.read_text().replace('../../../cadModels', str(SOURCE / 'cadModels')))
        wrapper = workdir / 'run.mac'
        wrapper.write_text('/run/numberOfThreads 2\n/random/setSeeds 12345 67890\n'
                           '/control/execute {}\n/run/beamOn 100\n'.format(macro))
        try:
            run = subprocess.run([binary, str(wrapper)], cwd=workdir,
                                 capture_output=True, text=True, timeout=180)
        except subprocess.TimeoutExpired:
            print('FAIL {}: exceeded 180 seconds'.format(name), flush=True)
            results.append(dict(case=name, passed=False, reason='timeout'))
            continue
        log = run.stdout + '\n' + run.stderr
        (workdir / 'run.log').write_text(log)
        ok, message = benchmark._check_run_criteria(name, run.stdout, run.stderr, run.returncode)
        ok = ok and not any(error in log for error in
                            ('COMMAND NOT FOUND', 'Batch is interrupted', '***** Illegal'))
        counts = [benchmark.count_detected_and_simulated(str(path))
                  for path in workdir.glob('output*.out')]
        detected = sum(pair[0] for pair in counts)
        simulated = sum(pair[1] for pair in counts)
        ok = ok and simulated == 100
        if name == 'smoke_sources':
            ok = ok and detected > 0
        results.append(dict(case=name, binary=binary_name, passed=ok,
                            simulated=simulated, detected=detected))
        print('{} {}: {} simulated, {} detected; {}'.format(
            'PASS' if ok else 'FAIL', name, simulated, detected, message), flush=True)
    (OUTPUT / 'summary.json').write_text(json.dumps(dict(versions=versions, results=results), indent=2))
    return 0 if results and all(result['passed'] for result in results) else 1


if __name__ == '__main__':
    sys.exit(main())
