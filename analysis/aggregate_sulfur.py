#!/usr/bin/env python3
"""Pool independent sulfur runs, retaining primary-event count uncertainties."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import numpy as np
import ROOT

ROOT.gROOT.SetBatch(True)
STATES=[1329,2150,2457]
VERSIONS=['g4-10','g4-11']
NAMES=['detected events','crystal entries']+[f'{lo}-{lo+500} keV' for lo in range(0,4000,500)]


def read(path):
    events=ROOT.RDataFrame('events',str(path/'spectrum.root')).AsNumpy(['event_id','crystal_multiplicity'])
    n=len(events['event_id'])
    assert np.array_equal(np.sort(events['event_id']),np.arange(n))
    mult=np.asarray(events['crystal_multiplicity'],dtype=np.float64)
    hits=ROOT.RDataFrame('crystal_hits',str(path/'spectrum.root')).AsNumpy(['event_id','energy_keV'])
    ids=hits['event_id'];energy=hits['energy_keV']
    cols=[(mult>0).astype(float),mult]
    for lo in range(0,4000,500):
        cols.append(np.bincount(ids[(energy>=lo)&(energy<lo+500)],minlength=n).astype(float))
    return dict(n=n,observables={name:dict(count=float(x.sum()),squares=float(np.dot(x,x)))
                                 for name,x in zip(NAMES,cols)})


def pooled(runs):
    n=sum(r['n'] for r in runs)
    result={}
    for name in NAMES:
        total=sum(r['observables'][name]['count'] for r in runs)
        squares=sum(r['observables'][name]['squares'] for r in runs)
        result[name]=dict(n=n,count=total,mean=total/n,variance_of_mean=(squares-total*total/n)/(n-1)/n)
    return result


def difference(a,b):
    delta=b['mean']-a['mean'];se=math.sqrt(a['variance_of_mean']+b['variance_of_mean'])
    z=delta/se if se and a['count']+b['count']>=20 else None
    return dict(g4_10_count=int(a['count']),g4_11_count=int(b['count']),
                difference_per_primary=delta,standard_error=se,z=z,
                p_two_sided=math.erfc(abs(z)/math.sqrt(2)) if z is not None else None,
                interval_95=[delta-1.96*se,delta+1.96*se],
                relative_difference_percent=100*delta/a['mean'] if a['mean'] else None)


def main():
    p=argparse.ArgumentParser();p.add_argument('results',type=Path);p.add_argument('output',type=Path)
    args=p.parse_args();args.output.mkdir(parents=True,exist_ok=False)
    indices=list(range(2,14));cache={};manifests={};seedpairs=set();reference={}
    for index in indices:
        run=f'sulfur-comparison-{index:03d}'
        cache[run]={};manifests[run]={}
        for state in STATES:
            folder=args.results/run/str(state)
            manifest=json.loads((folder/'inputs.json').read_text());manifests[run][str(state)]=manifest
            if state not in reference:reference[state]=manifest
            for key in ['baseline_commit','migration_commit','native_tracking_action_sha256','events_per_run','threads','bins','range_keV']:
                assert manifest[key]==reference[state][key], (run,state,key)
            cache[run][str(state)]={}
            for version in VERSIONS:
                seed=tuple(manifest['seeds'][version]);assert seed not in seedpairs;seedpairs.add(seed)
                for name,digest in manifest['sha256'][version].items():
                    assert hashlib.sha256((folder/version/name).read_bytes()).hexdigest()==digest
                    if name!='run.mac':assert digest==reference[state]['sha256']['g4-10'][name]
                log=(folder/version/'simulation.log').read_text()
                assert 'events/s' in log and not any(x in log for x in ['G4Exception','COMMAND NOT FOUND','Batch is interrupted'])
                cache[run][str(state)][version]=read(folder/version)
                assert cache[run][str(state)][version]['n']==100000
        print('Read '+run,flush=True)
    (args.output/'run_moments.json').write_text(json.dumps(cache,indent=2))
    (args.output/'inputs.json').write_text(json.dumps(manifests,indent=2))
    final={};report=['# Sulfur-44: ten additional seed sets', '',
        'Ten new independent runs per state/version, 100,000 primaries each: six million new events. The all-runs pool adds the two previous independent runs, giving 1,200,000 primaries per state/version. The pre-fix native runs in sulfur-comparison-001 are excluded.', '',
        'Errors use primary-event sample variances, including correlations between crystals hit in one event. Spectra are normalized per beam primary, in laboratory energies without Doppler correction or resolution smearing. The 2150-keV excitation / 1000–1500-keV deposited-energy window was selected before these ten new runs. Its new-only result is the independent follow-up; the all-runs pool includes the data that motivated that selection.', '']
    objects=[]
    for group,selected in [('new_ten',indices[2:]),('all_twelve',indices)]:
        groupdir=args.output/group;groupdir.mkdir()
        canvas=ROOT.TCanvas(group,group,1200,1100);canvas.Divide(2,3)
        root=ROOT.TFile(str(groupdir/'spectra.root'),'RECREATE')
        data={};report += ['## '+group.replace('_',' '),'']
        for row,state in enumerate(STATES):
            stats={v:pooled([cache[f'sulfur-comparison-{i:03d}'][str(state)][v] for i in selected]) for v in VERSIONS}
            checks={name:difference(stats['g4-10'][name],stats['g4-11'][name]) for name in NAMES}
            data[str(state)]=dict(events_per_version=stats['g4-10']['crystal entries']['n'],checks=checks)
            report += [f'### Excitation {state} keV','','| Observable | Geant4 10 | Geant4 11 | Difference / standard error |','|---|---:|---:|---:|']
            for name,d in checks.items():
                value=f"{d['z']:+.2f}" if d['z'] is not None else 'n/a (sparse)'
                report.append(f"| {name} | {d['g4_10_count']} | {d['g4_11_count']} | {value} |")
            report.append('')
            for col,key in enumerate(['crystal_energy','emitted_energy']):
                canvas.cd(row*2+col+1).SetLogy();histograms=[]
                legend=ROOT.TLegend(.57,.74,.89,.89)
                for version,color in zip(VERSIONS,[ROOT.kBlue+1,ROOT.kRed+1]):
                    summed=None
                    for i in selected:
                        f=ROOT.TFile.Open(str(args.results/f'sulfur-comparison-{i:03d}'/str(state)/version/'spectrum.root'))
                        h=f.Get(key)
                        if summed is None:summed=h.Clone(f'{key}_{state}_{version}');summed.SetDirectory(0)
                        else:summed.Add(h)
                        f.Close()
                    root.cd();summed.Write() # Raw 2-keV counts; preserve normalization information separately.
                    h=summed.Clone(summed.GetName()+'_per_primary');h.SetDirectory(0);h.Rebin(10);h.Scale(1/stats[version]['crystal entries']['n'])
                    h.SetLineColor(color);h.SetStats(False)
                    h.SetTitle(f'Ex = {state} keV: '+('crystal deposits' if col==0 else 'emitted gammas')+';Laboratory energy [keV];Entries / primary / 20 keV')
                    histograms.append(h);objects.extend([h,summed]);legend.AddEntry(h,'Geant4 10.7.4' if version=='g4-10' else 'Geant4 11.4.1','l')
                histograms[0].SetMinimum(1e-7);histograms[0].SetMaximum(max(h.GetMaximum() for h in histograms)*3)
                histograms[0].Draw('HIST');histograms[1].Draw('HIST SAME');legend.Draw();objects.append(legend)
        # Exploratory energy-window family: Holm adjustment across all states, excluding sparse windows.
        family=sorted([(d['p_two_sided'],s,name) for s,item in data.items() for name,d in item['checks'].items()
                       if name.endswith('keV') and d['p_two_sided'] is not None])
        adjusted=0
        for rank,(prob,s,name) in enumerate(family):
            adjusted=max(adjusted,min(1.,prob*(len(family)-rank)))
            data[s]['checks'][name]['holm_adjusted_p_energy_windows']=adjusted
        final[group]=data;root.cd();canvas.Write();ROOT.TNamed('metadata',json.dumps(data)).Write();root.Close()
        canvas.SaveAs(str(groupdir/'comparison.pdf'));canvas.SaveAs(str(groupdir/'comparison.png'));objects.append(canvas)
    # Independent seed-by-seed follow-up of the previously highlighted window.
    target=[]
    for i in indices:
        run=f'sulfur-comparison-{i:03d}';item=cache[run]['2150']
        d=difference(pooled([item['g4-10']])['1000-1500 keV'],pooled([item['g4-11']])['1000-1500 keV'])
        target.append(dict(run=run,**d))
    final['target_by_seed']=target
    followup=ROOT.TCanvas('target_seed_followup','Previously highlighted window',1100,650)
    graph=ROOT.TGraphErrors()
    for point,d in enumerate(target[2:]):
        graph.SetPoint(point,point+1,d['difference_per_primary']*100000)
        graph.SetPointError(point,0,d['standard_error']*100000)
    graph.SetTitle('2150-keV state: 1000-1500 keV crystal deposits;Additional independent seed set;Geant4 11 minus 10: entries / 100,000 primaries')
    graph.SetMarkerStyle(20);graph.Draw('AP');graph.GetXaxis().SetLimits(0.5,10.5)
    zero=ROOT.TLine(.5,0,10.5,0);zero.SetLineStyle(2);zero.Draw()
    followup.SaveAs(str(args.output/'target_seed_followup.pdf'));followup.SaveAs(str(args.output/'target_seed_followup.png'))
    report += ['## Previously highlighted window: 2150-keV state, 1000–1500 keV deposits','','| Run | Geant4 10 | Geant4 11 | Difference / standard error |','|---|---:|---:|---:|']
    for d in target:report.append(f"| {d['run']} | {d['g4_10_count']} | {d['g4_11_count']} | {d['z']:+.2f} |")
    report += ['','summary.json includes two-sided normal-approximation p values and pointwise 95% intervals. Exploratory energy-window p values also have a Holm correction across populated windows in each pool. These are not proof of physical equivalence; polarized configurations, Doppler reconstruction, and other reaction models remain outside this test.']
    (args.output/'summary.json').write_text(json.dumps(final,indent=2));(args.output/'summary.md').write_text('\n'.join(report)+'\n')
    print('Complete: '+str(args.output),flush=True)


if __name__=='__main__':main()
