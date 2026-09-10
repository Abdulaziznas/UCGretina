#!/usr/bin/env python3
"""Summarize three sulfur excitation runs, using event-level count errors."""
import argparse
import json
import math
from pathlib import Path
import ROOT
import check_seed_variation as stats

ROOT.gROOT.SetBatch(True)
stats.WINDOWS = [(e,e+500) for e in range(0,4000,500)]


def main():
    p=argparse.ArgumentParser(); p.add_argument('directory', type=Path); args=p.parse_args()
    base=args.directory
    report=['# Sulfur-44 reaction comparison', '',
        '100,000 beam primaries per state and version; independent seeds. Original NSCL 12-quad geometry, 100 MeV/u sulfur-44 beam, Be target, emstandard_opt4, two workers.', '',
        'Crystal spectra use laboratory deposited energies without Doppler correction, resolution smearing, or cross-crystal addback. Ratios are normalized per beam primary. Thus the moving-source peaks are Doppler broadened. Excitation states are compared separately, without assuming population weights.', '',
        'The table reports event-level standard errors, including correlations between crystals hit in the same primary event. Normal-approximation z values are omitted when combined counts are below 20. Broad energy windows are exploratory checks, not a global equivalence test.', '']
    canvas=ROOT.TCanvas('sulfur_comparison','Sulfur-44 comparison',1200,1100); canvas.Divide(2,3)
    objects=[]; results={}
    for row,state in enumerate([1329,2150,2457]):
        directory=base/str(state)
        summaries=[json.loads((directory/v/'spectrum.json').read_text()) for v in ['g4-10','g4-11']]
        values=[stats.read(directory/v) for v in ['g4-10','g4-11']]
        report += [f'## Excitation {state} keV', '', '| Observable | Geant4 10 | Geant4 11 | Difference / standard error |','|---|---:|---:|---:|']
        rows={}
        for name in values[0]:
            a,b=values[0][name],values[1][name]
            delta=b['mean']-a['mean']; se=math.sqrt(a['variance_of_mean']+b['variance_of_mean'])
            z=delta/se if se and a['count']+b['count'] >= 20 else None
            rows[name]=dict(g4_10=a['count'],g4_11=b['count'],difference_per_primary=delta,standard_error=se,z=z)
            formatted=f'{z:+.2f}' if z is not None else 'n/a (sparse or no variance)'
            report.append(f"| {name} | {a['count']} | {b['count']} | {formatted} |")
        report += ['',f"Recorded emitted gammas: {summaries[0]['generated_gammas']} (Geant4 10), {summaries[1]['generated_gammas']} (Geant4 11).",'']
        results[str(state)]=dict(runs=summaries,checks=rows)
        files=[ROOT.TFile.Open(str(directory/v/'spectrum.root')) for v in ['g4-10','g4-11']]
        for col,key in enumerate(['crystal_energy','emitted_energy']):
            pad=canvas.cd(row*2+col+1);pad.SetLogy()
            legend=ROOT.TLegend(.57,.74,.89,.89)
            histograms=[]
            for i,f in enumerate(files):
                h=f.Get(key).Clone(f'{key}_{state}_{i}');h.SetDirectory(0);h.Rebin(10);h.Scale(1/summaries[i]['events'])
                h.SetStats(False);h.SetLineColor(ROOT.kBlue+1 if i==0 else ROOT.kRed+1)
                h.SetTitle(f'Ex = {state} keV: '+('crystal deposits' if col==0 else 'recorded emitted gammas')+';Laboratory energy [keV];Entries / primary / 20 keV')
                histograms.append(h);objects.append(h)
                legend.AddEntry(h,'Geant4 10.7.4' if i==0 else 'Geant4 11.4.1','l')
            histograms[0].SetMinimum(1e-6);histograms[0].SetMaximum(max(h.GetMaximum() for h in histograms)*3)
            histograms[0].Draw('HIST');histograms[1].Draw('HIST SAME');legend.Draw();objects.append(legend)
        for f in files:f.Close()
    canvas.SaveAs(str(base/'sulfur_comparison.pdf'));canvas.SaveAs(str(base/'sulfur_comparison.png'))
    f=ROOT.TFile(str(base/'sulfur_comparison.root'),'RECREATE');canvas.Write()
    for o in objects:
        if o.InheritsFrom('TH1'):o.Write()
    f.Close()
    (base/'summary.json').write_text(json.dumps(results,indent=2))
    (base/'summary.md').write_text('\n'.join(report)+'\n')
    print('\n'.join(report))


if __name__=='__main__':main()
