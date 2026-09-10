#!/usr/bin/env python3
"""Convert UCGretina ASCII D/C/E records to ROOT and compare crystal spectra."""
import argparse
from array import array
import csv
import json
import math
from pathlib import Path
import re
import ROOT

ROOT.gROOT.SetBatch(True)


def records(path):
    """Return event id, emitted energies, and (crystal id, summed energy) pairs."""
    with path.open() as source:
        lines = iter(source)
        pending = None
        spectrometer_event = None
        for line in lines:
            fields = line.split()
            if not fields:
                continue
            if fields[0] == 'S':
                if len(fields) != 6 or spectrometer_event is not None:
                    raise ValueError('Invalid or duplicate S800 record')
                spectrometer_event = int(fields[5])
            elif fields[0] == 'D':
                if pending is not None:
                    raise ValueError('D record without subsequent E in ' + str(path))
                crystals = []
                for _ in range(int(fields[1])):
                    header = next(lines).split()
                    if header[0] != 'C':
                        raise ValueError('Expected crystal header: ' + str(header))
                    energy = 0.0
                    for _ in range(int(header[2])):
                        point = next(lines).split()
                        if len(point) != 6:
                            raise ValueError('Expected six interaction-point columns')
                        value = float(point[1])
                        if not math.isfinite(value) or value < 0:
                            raise ValueError('Invalid deposited energy')
                        energy += value
                    crystals.append((int(header[1]), energy))
                pending = (int(fields[2]), crystals)
            elif fields[0] == 'E':
                event = int(fields[4])
                if spectrometer_event is not None and spectrometer_event != event:
                    raise ValueError('S/E event ids do not match')
                spectrometer_event = None
                emitted = [float(next(lines).split()[0]) for _ in range(int(fields[1]))]
                crystals = []
                if pending is not None:
                    if pending[0] != event:
                        raise ValueError('D/E event ids do not match')
                    crystals = pending[1]
                pending = None
                yield event, emitted, crystals
            else:
                raise ValueError('Unexpected record: ' + line)
        if pending is not None or spectrometer_event is not None:
            raise ValueError('Unterminated D record')


def convert(args):
    directory = args.directory
    log = (directory / 'simulation.log').read_text()
    if any(marker in log for marker in ('FatalException', 'Fatal Exception',
                                       'COMMAND NOT FOUND', 'Batch is interrupted')):
        raise RuntimeError('Simulation log contains an error')
    if 'events/s' not in log:
        raise RuntimeError('Simulation log has no completed run')
    inputs = json.loads((directory.parent / 'inputs.json').read_text())
    bins = inputs.get('bins', 800)
    lower, upper = inputs.get('range_keV', [0, 1600])
    files = sorted(directory.glob('events_t*.out'))
    if not files:
        files = [directory / 'events.out']
    output = ROOT.TFile(str(directory / 'spectrum.root'), 'RECREATE')
    hist = ROOT.TH1D('crystal_energy', 'Crystal energy;Deposited energy [keV];Crystal entries / 2 keV', bins, lower, upper)
    generated = ROOT.TH1D('emitted_energy', 'Emitted gamma energy;Energy [keV];Gammas / 2 keV', bins, lower, upper)
    summed = ROOT.TH1D('event_energy', 'Array energy sum;Deposited energy [keV];Detected events / 2 keV', bins, lower, upper)
    for h in (hist, generated, summed):
        h.Sumw2()
    events = ROOT.TTree('events', 'All primary events, including zero-deposit events')
    hits = ROOT.TTree('crystal_hits', 'Energy sum per crystal per event')
    event_id, worker, multiplicity, detector = (array('i', [0]) for _ in range(4))
    total, energy = array('d', [0]), array('d', [0])
    for tree in (events, hits):
        tree.Branch('event_id', event_id, 'event_id/I')
        tree.Branch('worker', worker, 'worker/I')
    events.Branch('crystal_multiplicity', multiplicity, 'crystal_multiplicity/I')
    events.Branch('array_energy_keV', total, 'array_energy_keV/D')
    hits.Branch('crystal_id', detector, 'crystal_id/I')
    hits.Branch('energy_keV', energy, 'energy_keV/D')
    seen, detected, generated_count = set(), 0, 0
    for path in files:
        match = re.search(r'_t(\d+)\.out$', path.name)
        worker[0] = int(match.group(1)) if match else -1
        for event, emitted, crystals in records(path):
            if event in seen:
                raise ValueError('Duplicate global event id: {}'.format(event))
            seen.add(event)
            event_id[0] = event
            multiplicity[0] = 0
            total[0] = 0.0
            for value in emitted:
                generated.Fill(value)
                generated_count += 1
            for crystal, value in crystals:
                if value <= 0:
                    continue
                detector[0], energy[0] = crystal, value
                total[0] += value
                multiplicity[0] += 1
                hist.Fill(value)
                hits.Fill()
            if total[0] > 0:
                summed.Fill(total[0])
                detected += 1
            events.Fill()
    if len(seen) != inputs['events_per_run'] or seen != set(range(inputs['events_per_run'])):
        raise ValueError('Missing events: expected {}, read {}'.format(inputs['events_per_run'], len(seen)))
    summary = dict(label=args.label, events=len(seen), detected_events=detected,
                   generated_gammas=generated_count, crystal_entries=int(hist.GetEntries()),
                   root_version=ROOT.gROOT.GetVersion(),
                   geant4_banner=next((s.strip() for s in log.splitlines() if 'Geant4 version' in s), 'unknown'),
                   underflow=hist.GetBinContent(0), overflow=hist.GetBinContent(bins + 1))
    ROOT.TNamed('metadata', json.dumps(dict(inputs=inputs, summary=summary))).Write()
    output.Write()
    output.Close()
    (directory / 'spectrum.json').write_text(json.dumps(summary, indent=2))
    print(json.dumps(summary, indent=2))


def compare(args):
    base = args.directory
    inputs = json.loads((base / 'inputs.json').read_text())
    lower, upper = inputs.get('range_keV', [0, 1600])
    files = [ROOT.TFile.Open(str(base / label / 'spectrum.root')) for label in ('g4-10', 'g4-11')]
    if any(not f or f.IsZombie() for f in files):
        raise RuntimeError('Cannot open input ROOT files')
    summaries = [json.loads((base / label / 'spectrum.json').read_text()) for label in ('g4-10', 'g4-11')]
    histograms = []
    for index, f in enumerate(files):
        h = f.Get('crystal_energy').Clone('g4_{}_per_event'.format(10 + index))
        h.SetDirectory(0)
        h.Scale(1.0 / summaries[index]['events'])
        histograms.append(h)
    old, new = histograms
    result = ROOT.TFile(str(base / 'comparison.root'), 'RECREATE')
    old.SetLineColor(ROOT.kBlue + 1)
    new.SetLineColor(ROOT.kRed + 1)
    old.SetStats(False)
    new.SetStats(False)
    canvas = ROOT.TCanvas('comparison', 'Geant4 gamma spectrum comparison', 1100, 850)
    canvas.Divide(1, 2)
    canvas.cd(1).SetLogy()
    old.SetTitle(inputs.get('plot_title', 'Eu-152 single-gamma source') + ';Crystal deposited energy [keV];Crystal entries / primary / 2 keV')
    old.SetMinimum(1e-6)
    old.SetMaximum(max(old.GetMaximum(), new.GetMaximum()) * 2)
    old.Draw('HIST')
    new.Draw('HIST SAME')
    legend = ROOT.TLegend(0.60, 0.73, 0.89, 0.89)
    legend.AddEntry(old, 'Geant4 10.7.4 (container)', 'l')
    legend.AddEntry(new, 'Geant4 11.4.1 (WSL)', 'l')
    legend.Draw()
    canvas.cd(2)
    ratio = ROOT.TGraphErrors()
    ratio.SetName('ratio_g4_11_over_g4_10')
    rows = []
    for b in range(1, old.GetNbinsX() + 1):
        a, c = old.GetBinContent(b), new.GetBinContent(b)
        ea, ec = old.GetBinError(b), new.GetBinError(b)
        r = c / a if a else None
        er = math.sqrt((ec/a)**2 + (c*ea/a**2)**2) if a else None
        rows.append([old.GetBinCenter(b), a, c, r, er])
        if r is not None:
            idx = ratio.GetN()
            ratio.SetPoint(idx, old.GetBinCenter(b), r)
            ratio.SetPointError(idx, 0, er)
    ratio.SetTitle('Ratio (bins with a nonzero Geant4 10 denominator);Crystal deposited energy [keV];Geant4 11 / Geant4 10')
    ratio.SetMarkerStyle(20)
    ratio.SetMarkerSize(0.35)
    ratio.Draw('AP')
    ratio.GetXaxis().SetLimits(lower, upper)
    unity = ROOT.TLine(lower, 1, upper, 1)
    unity.SetLineStyle(2)
    unity.Draw()
    for obj in (old, new, ratio, canvas):
        obj.Write()
    canvas.SaveAs(str(base / 'comparison.png'))
    canvas.SaveAs(str(base / 'comparison.pdf'))
    with (base / 'spectrum_bins.csv').open('w', newline='') as target:
        writer = csv.writer(target)
        writer.writerow(['energy_keV', 'g4_10_per_primary', 'g4_11_per_primary', 'ratio', 'approx_ratio_error'])
        writer.writerows(rows)
    peak_windows = []
    for low, high in inputs.get('peak_windows_keV', [(120, 124), (242, 248), (342, 348), (776, 782),
                      (962, 968), (1110, 1116), (1406, 1412)]):
        counts = []
        for source in files:
            h = source.Get('crystal_energy')
            counts.append(int(h.Integral(h.FindBin(low + 0.001), h.FindBin(high - 0.001))))
        peak_windows.append(dict(window_keV=[low, high], g4_10_counts=counts[0],
                                 g4_11_counts=counts[1], background_subtracted=False))
    metrics = dict(runs=summaries, peak_windows=peak_windows,
                   crystal_yield_ratio=(new.Integral() / old.Integral()),
                   notes=['Normalized per simulated primary, not to equal spectrum area.',
                          'No resolution smearing or cross-crystal addback in the main spectrum.',
                          'Ratio errors use approximate independent Poisson counts; crystal entries within an event can be correlated.',
                          inputs['source'],
                          'This is an initial comparison, not a physics validation sign-off.'])
    ROOT.TNamed('comparison_metadata', json.dumps(metrics)).Write()
    result.Close()
    (base / 'comparison.json').write_text(json.dumps(metrics, indent=2))
    print(json.dumps(metrics, indent=2))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('action', choices=['convert', 'compare'])
    parser.add_argument('directory', type=Path)
    parser.add_argument('--label', default='')
    args = parser.parse_args()
    convert(args) if args.action == 'convert' else compare(args)
