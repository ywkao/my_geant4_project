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
    gr.GetXaxis().SetTitleSize(0.042)
    gr.GetXaxis().SetTitleOffset(1.0)
    gr.GetYaxis().SetTitle(ytitle)
    gr.GetYaxis().SetTitleSize(0.042)

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


sigmaEoverE, error_sigmaEoverE = [], []

def reset_containers():
    global sigmaEoverE , error_sigmaEoverE
    sigmaEoverE, error_sigmaEoverE = [], []

def record_fit_result(func):
    global sigmaEoverE

    fit_const = func.GetParameter(0)
    fit_mean  = func.GetParameter(1)
    fit_sigma = func.GetParameter(2)
    fitError_const = func.GetParError(0)
    fitError_mean  = func.GetParError(1)
    fitError_sigma = func.GetParError(2)

    ratio = fit_sigma/fit_mean
    uncertainty = ratio * math.sqrt( math.pow(fitError_mean/fit_mean, 2) + math.pow(fitError_sigma/fit_sigma, 2) )
    
    sigmaEoverE.append(ratio)
    error_sigmaEoverE.append(uncertainty)

    #return fit_mean, fit_sigma

    #print ">>> result:", fit_const, fit_mean, fit_sigma 
    #print ">>> fit error:", fitError_const, fitError_mean, fitError_sigma 
    
    #d_fit_const["central"].append( func.GetParameter(0) )
    #d_fit_mean ["central"].append( func.GetParameter(1) )
    #d_fit_sigma["central"].append( func.GetParameter(2) )
    #d_fit_const[ "error" ].append( func.GetParError(0)  )
    #d_fit_mean [ "error" ].append( func.GetParError(1)  )
    #d_fit_sigma[ "error" ].append( func.GetParError(2)  )

def set_stat_pad(stat, positions, color):
    if stat:
        #print ">>>>> check:", stat.GetName()
        stat.GetName()
        stat.SetX1NDC(positions[0])
        stat.SetY1NDC(positions[1])
        stat.SetX2NDC(positions[2])
        stat.SetY2NDC(positions[3])
        stat.SetTextSize(0.02)
        stat.SetTextColor(color)
        stat.SetLineColor(color)
    else:
        #print ">>>>> stat is null"
        return

