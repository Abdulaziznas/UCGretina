#!/usr/bin/env python3
"""Compare independent runs using the primary event as the sampling unit."""
import argparse
from collections import defaultdict
import json
import math
from pathlib import Path
import ROOT

WINDOWS = [(120,124), (242,248), (342,348), (776,782), (962,968), (1110,1116), (1406,1412)]


def moments(values, n):
    # Unrepresented events have zero counts. Retain within-event correlations.
    total = sum(values)
    squares = sum(v*v for v in values)
    variance = (squares - total*total/n)/(n-1)
    return dict(count=total, mean=total/n, variance_of_mean=variance/n)


def read(directory):
    f = ROOT.TFile.Open(str(directory / 'spectrum.root'))
    if not f or f.IsZombie():
        raise RuntimeError('Cannot read ' + str(directory))
    n = int(f.Get('events').GetEntries())
    multiplicities = [int(e.crystal_multiplicity) for e in f.Get('events')]
    output = {'detected events': moments([int(v > 0) for v in multiplicities], n),
              'crystal entries': moments(multiplicities, n)}
    counts = [defaultdict(int) for _ in WINDOWS]
    for hit in f.Get('crystal_hits'):
        energy, event_id = float(hit.energy_keV), int(hit.event_id)
        for window, (lo, hi) in zip(counts, WINDOWS):
            if lo <= energy < hi:
                window[event_id] += 1
    for window, (lo, hi) in zip(counts, WINDOWS):
        output[f'{lo}-{hi} keV'] = moments(list(window.values()), n)
    f.Close()
    return output


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('previous', type=Path)
    parser.add_argument('current', type=Path)
    args = parser.parse_args()
    runs = {f'{tag}/{v}': read(base / v)
            for tag, base in [('previous', args.previous), ('current', args.current)]
            for v in ['g4-10', 'g4-11']}
    pairs = [('New Geant4 11 vs new Geant4 10', 'current/g4-10', 'current/g4-11'),
             ('Geant4 10: new vs previous seed', 'previous/g4-10', 'current/g4-10'),
             ('Geant4 11: new vs previous seed', 'previous/g4-11', 'current/g4-11')]
    comparisons = {}
    text = ['# Independent-seed comparison', '',
            'Each run contains 100,000 Eu-152 single-gamma primaries. All settings except seeds match the original comparison.', '',
            'The difference is divided by its estimated standard error (z). Errors use event-level sample variances, including correlations between crystals hit in the same primary event. Approximately ±1.96 standard errors is an individual 95% interval under a normal approximation. These correlated checks are exploratory, not a global equivalence test. Peak windows include continuum.', '']
    for title, left, right in pairs:
        text += ['## ' + title, '', '| Observable | Reference count | New count | Difference / standard error |', '|---|---:|---:|---:|']
        rows = {}
        for name in runs[left]:
            a, b = runs[left][name], runs[right][name]
            se = math.sqrt(a['variance_of_mean'] + b['variance_of_mean'])
            delta = b['mean'] - a['mean']
            z = delta/se if se else None
            rows[name] = dict(reference=a['count'], new=b['count'], difference_per_primary=delta,
                              standard_error=se, z=z, interval_95=[delta-1.96*se, delta+1.96*se])
            text.append(f"| {name} | {a['count']} | {b['count']} | {z:+.2f} |")
        text.append('')
        comparisons[title] = rows
    result = dict(previous=str(args.previous.resolve()), current=str(args.current.resolve()),
                  runs=runs, comparisons=comparisons,
                  method='Independent primary events; sample variances include within-event crystal correlations; normal-approximation intervals; no multiple-testing correction.')
    (args.current/'seed_variation.json').write_text(json.dumps(result, indent=2))
    (args.current/'seed_variation.md').write_text('\n'.join(text)+'\n')
    print('\n'.join(text))


if __name__ == '__main__':
    main()
