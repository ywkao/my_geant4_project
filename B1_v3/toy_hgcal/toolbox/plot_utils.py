#!/usr/bin/env python
import math
import copy
import array
import ROOT
ROOT.gROOT.SetBatch(True)

import MetaData as m

def load_single_histogram(fin, varName):
    histName = varName
    h = fin.Get(histName)
    return h

def load_histograms(fin, varName, processes):
    v_hists = []
    for p in processes:
        histName = varName + "_" + str(p)
        h = fin.Get(histName)
        v_hists.append(h)

    return v_hists

def set_graph(gr, ytitle, xtitle, color):
    gr.SetTitle("")
    gr.SetLineColor(color)
    gr.SetLineWidth(2)
    gr.SetMarkerStyle(20)
    gr.SetMarkerColor(color)
    gr.SetMinimum(0)
    gr.GetXaxis().SetTitle(xtitle)
    gr.GetYaxis().SetTitle(ytitle)
    gr.GetXaxis().SetTitleOffset(1.2)

sub_directory = {
    "h_nHits" : "h_nHits",
    "h_Edep"  : "h_Edep",
}

ytitles = copy.deepcopy( sub_directory )
ytitles["h_nHits"] = "Multiplicity of hits / steps"
ytitles["h_Edep"] = "Energy [MeV]"

def get_graph(lx, varName, v_hists, is_number_of_hits = False):
    ly = [h.GetMean() for h in v_hists]
    ley = [h.GetMeanError() for h in v_hists]
    gr = get_graph_from_list(varName, lx, ly, ley, is_number_of_hits)
    return gr, max(ly)

def get_graph_from_list(varName, lx, ly, ley, normalize_to_unity = False):
    n = len(lx) 
    x, ex = array.array('d'), array.array('d')
    y, ey = array.array('d'), array.array('d')

    total = sum(ly) 
    scale = 1./total if normalize_to_unity else 1.
    ly = [ele*scale for ele in ly]
    ley = [ele*scale for ele in ley]

    for i, ele in enumerate(lx):
        x.append(ele)
        y.append(ly[i])
        ex.append(0.)
        ey.append(ley[i])

    gr = ROOT.TGraphErrors(n, x, y, ex, ey)
    set_graph(gr, ytitles[varName], "Layer depth [ X_{0} ]", ROOT.kBlue)
    if normalize_to_unity: gr.SetMaximum(0.12)

    #print ">>> get_graph_from_list::total =", total
    #print ">>> n, total, ly, ley =", n, total, ly, ley
    return gr


