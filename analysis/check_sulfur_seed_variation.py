#!/usr/bin/env python3
"""Compare sulfur runs across versions and independent random seeds."""
import argparse
import json
import math
from pathlib import Path
import check_seed_variation as stats

stats.WINDOWS = [(e,e+500) for e in range(0,4000,500)]


def main():
    p=argparse.ArgumentParser()
    p.add_argument('previous', type=Path);p.add_argument('current', type=Path)
    args=p.parse_args()
    report=['# Sulfur-44 seed variation', '',
            '100,000 primaries per run. All physical inputs match; seed pairs are independent. Differences are divided by the standard error computed from primary-event sample variances, retaining within-event crystal correlations. These are exploratory, correlated checks, not a global equivalence test. Normal-approximation z values are omitted for combined counts below 20.', '']
    results={}
    for state in [1329,2150,2457]:
        runs={f'{tag}/{v}':stats.read(base/str(state)/v)
              for tag,base in [('previous',args.previous),('current',args.current)]
              for v in ['g4-10','g4-11']}
        manifests={tag:json.loads((base/str(state)/'inputs.json').read_text())
                   for tag,base in [('previous',args.previous),('current',args.current)]}
        report += [f'## Excitation {state} keV', '',
                   '| Run | Geant4 10 seeds | Geant4 11 seeds |','|---|---|---|']
        for tag,m in manifests.items():report.append(f"| {tag} | {m['seeds']['g4-10']} | {m['seeds']['g4-11']} |")
        pairs=[('New Geant4 11 vs new Geant4 10','current/g4-10','current/g4-11'),
               ('Geant4 10: new vs previous','previous/g4-10','current/g4-10'),
               ('Geant4 11: new vs previous','previous/g4-11','current/g4-11')]
        checks={}
        for title,left,right in pairs:
            report += ['', '### '+title, '', '| Observable | Reference | New | Difference / standard error |','|---|---:|---:|---:|']
            rows={}
            for name in runs[left]:
                a,b=runs[left][name],runs[right][name]
                delta=b['mean']-a['mean']; se=math.sqrt(a['variance_of_mean']+b['variance_of_mean'])
                z=delta/se if se and a['count']+b['count']>=20 else None
                rows[name]=dict(reference=a['count'],new=b['count'],difference_per_primary=delta,standard_error=se,z=z)
                value=f'{z:+.2f}' if z is not None else 'n/a (sparse or no variance)'
                report.append(f"| {name} | {a['count']} | {b['count']} | {value} |")
            checks[title]=rows
        results[str(state)]=dict(seeds={k:m['seeds'] for k,m in manifests.items()},comparisons=checks)
        report.append('')
    (args.current/'seed_variation.json').write_text(json.dumps(results,indent=2))
    (args.current/'seed_variation.md').write_text('\n'.join(report)+'\n')
    print(json.dumps(results,indent=2))


if __name__=='__main__':main()
