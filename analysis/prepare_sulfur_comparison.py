#!/usr/bin/env python3
"""Stage baseline sulfur-44 reaction examples with independent random seeds."""
import argparse
import hashlib
import json
import shutil
from prepare_gamma_comparison import BASELINE, REPO, git


def main():
    p = argparse.ArgumentParser()
    p.add_argument('output')
    p.add_argument('--events', type=int, default=100000)
    p.add_argument('--seed-offset', type=int, default=0,
                   help='Add this offset to each of the six distinct seed pairs')
    args = p.parse_args()
    from pathlib import Path
    base = Path(args.output)
    if args.events <= 0:
        p.error('events must be positive')
    if not 0 <= args.seed_offset <= 2146368353:
        p.error('seed offset is outside the supported positive seed range')
    base.mkdir(parents=True, exist_ok=False)
    example = REPO/'examples/inbeam/fit'
    common = {}
    for name in ['asolid','aslice','aclust','awalls','aeuler','crmat.LINUX','z16.a44.lvldata']:
        path = (example/name).resolve()
        data = git('show', BASELINE+':'+str(path.relative_to(REPO)))
        if data != path.read_bytes():
            raise RuntimeError('Input differs from baseline: '+name)
        common[name] = data
    for index, state in enumerate([1329,2150,2457]):
        directory = base/str(state)
        directory.mkdir()
        path = example/f's44_{state}.mac'
        original = git('show', BASELINE+':'+str(path.relative_to(REPO)))
        if original != path.read_bytes():
            raise RuntimeError('Macro differs from baseline')
        body = '\n'.join(line for line in original.decode().splitlines()
                         if not line.strip().startswith(('/run/beamOn','/Mode2/Filename')))
        seeds = {'g4-10':[513279+index*1009,713243+index*1013],
                 'g4-11':[913247+index*1019,1113251+index*1021]}
        seeds = {version: [s + args.seed_offset for s in pair]
                 for version, pair in seeds.items()}
        hashes = {}
        for version in seeds:
            target = directory/version
            target.mkdir()
            inputs = dict(common)
            inputs['run.mac'] = ('/run/numberOfThreads 2\n/random/setSeeds {} {}\n'.format(*seeds[version])+
                '/PhysicsList/SelectEmPhysics emstandard_opt4\n'+body+
                f'\n/Output/Filename events.out\n/run/beamOn {args.events}\n').encode()
            for name, data in inputs.items():
                (target/name).write_bytes(data)
            hashes[version] = {name:hashlib.sha256(data).hexdigest() for name,data in inputs.items()}
        manifest = dict(source=f'Sulfur-44 in-beam excitation {state} keV',
            plot_title=f'Sulfur-44, excitation {state} keV (laboratory energies)',
            excitation_keV=state, baseline_commit=BASELINE,
            migration_commit=git('rev-parse','HEAD').decode().strip(),
            native_tracking_action_sha256=hashlib.sha256((REPO/'src/TrackingAction.cc').read_bytes()).hexdigest(),
            native_worktree_patch='native-source.patch in parent directory',
            events_per_run=args.events, threads=2, seeds=seeds,
            bins=2000, range_keV=[0,4000], resolution_smearing=False,
            doppler_correction=False, geometry='NSCLstd12quads/G120C4',
            peak_windows_keV=[], sha256=hashes)
        (directory/'inputs.json').write_text(json.dumps(manifest,indent=2))
    shutil.copyfile(REPO/'analysis/gamma_spectra.py',base/'gamma_spectra.py')
    (base/'native-source.patch').write_bytes(git('diff', '--', 'src', 'include'))
    print(base.resolve())


if __name__ == '__main__':
    main()
