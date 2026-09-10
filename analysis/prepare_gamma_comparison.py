#!/usr/bin/env python3
"""Prepare identical Eu-152 gamma-source inputs from the comparison baseline."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess

BASELINE = '1271388e03ec9b3f8e055a988aeb4c2e4d2efedf'
REPO = Path(__file__).resolve().parents[1]


def git(*args):
    return subprocess.check_output(['git', '-C', str(REPO), *args])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output', type=Path)
    parser.add_argument('--events', type=int, default=100000)
    parser.add_argument('--seeds-g4-10', type=int, nargs=2, default=[12345, 67890])
    parser.add_argument('--seeds-g4-11', type=int, nargs=2, default=[12345, 67890])
    args = parser.parse_args()
    if args.events <= 0:
        parser.error('--events must be positive')
    seeds = {'g4-10': args.seeds_g4_10, 'g4-11': args.seeds_g4_11}
    if any(s <= 0 or s >= 2147483647 for pair in seeds.values() for s in pair):
        parser.error('seeds must be between 1 and 2147483646')
    args.output.mkdir(parents=True, exist_ok=False)
    example = REPO / 'examples/sources/eu152'
    inputs = {}
    for name in ['asolid', 'aslice', 'aclust', 'awalls', 'aeuler', 'crmat.LINUX']:
        resolved = (example / name).resolve()
        content = git('show', '{}:{}'.format(BASELINE, resolved.relative_to(REPO)))
        if content != resolved.read_bytes():
            raise RuntimeError('Local input differs from baseline: ' + name)
        inputs[name] = content
    macro = git('show', BASELINE + ':examples/sources/eu152/eu152_gammas.mac').decode()
    macro = '\n'.join(line for line in macro.splitlines()
                      if not line.strip().startswith(('/run/beamOn', '/Mode2/Filename')))
    macro = ('/PhysicsList/SelectEmPhysics emstandard_opt4\n' + macro +
             '\n/Output/Filename events.out\n/run/beamOn {}\n'.format(args.events))
    hashes = {}
    for label in ['g4-10', 'g4-11']:
        directory = args.output / label
        directory.mkdir()
        inputs['run.mac'] = ('/run/numberOfThreads 2\n/random/setSeeds {} {}\n'.format(*seeds[label]) + macro).encode()
        for name, content in inputs.items():
            (directory / name).write_bytes(content)
        hashes[label] = {name: hashlib.sha256(value).hexdigest() for name, value in inputs.items()}
    manifest = dict(source='Eu-152 empirical single-gamma source (not radioactive-decay cascades)',
                    baseline_commit=BASELINE, migration_commit=git('rev-parse', 'HEAD').decode().strip(),
                    events_per_run=args.events, threads=2, seeds=seeds,
                    spectrum='sum interaction-point energy per crystal per event, keV',
                    bins=800, range_keV=[0, 1600], resolution_smearing=False,
                    geometry='examples/sources/eu152: GretinaNSCL/G120C4',
                    sha256=hashes)
    (args.output / 'inputs.json').write_text(json.dumps(manifest, indent=2))
    shutil.copyfile(REPO / 'analysis/gamma_spectra.py', args.output / 'gamma_spectra.py')
    print(args.output.resolve())


if __name__ == '__main__':
    main()
