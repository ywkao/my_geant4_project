#!/usr/bin/env python2
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptFit(1111)

print "Hello!"

def cosmetic(h):
    h.GetXaxis().SetLabelFont(42)
    h.GetYaxis().SetLabelFont(42)
    h.GetXaxis().SetTitleFont(42)
    h.GetYaxis().SetTitleFont(42)
    h.GetXaxis().SetLabelSize(0.03)
    h.GetYaxis().SetLabelSize(0.03)
    h.GetXaxis().SetTitleSize(0.04)
    h.GetYaxis().SetTitleSize(0.04)
    h.GetYaxis().SetTitleOffset(1.2)

def run(rootfile, tag):
    fin = ROOT.TFile(rootfile, 'R')
    
    c1 = ROOT.TCanvas("c1", "", 800, 600)
    c1.SetTicks()
    c1.SetGrid()
    
    h = fin.Get("Edep")
    h.SetTitle("")
    h.SetName("edep_perEvt")
    h.SetLineWidth(2)
    h.SetLineColor(ROOT.kBlue)
    h.GetXaxis().SetRangeUser(0, 10)
    h.GetXaxis().SetTitle("Energy deposit for each event (MeV)")
    h.GetYaxis().SetTitle("Entries")
    cosmetic(h)

    h.Fit("landau", "", "", 0, 4)
    f1 = h.GetFunction("landau")
    f1.SetLineColorAlpha(ROOT.kRed, 0.5)
    #f1.SetLineStyle(2)

    h.Draw("hists")
    f1.Draw("same")
    c1.SaveAs("edep_perEvt%s.pdf" % tag)

    t = fin.Get("Hits")
    t.Draw("Hits_DetE_keV >> edep_perHit")
    hist = ROOT.gDirectory.Get("edep_perHit")
    hist.SetTitle("")
    hist.SetLineWidth(2)
    hist.SetLineColor(ROOT.kBlue)
    hist.GetXaxis().SetRangeUser(0, 800)
    hist.GetXaxis().SetTitle("Energy deposit for each hit (keV)")
    hist.GetYaxis().SetTitle("Entries")
    cosmetic(hist)
    c1.SaveAs("edep_perHit%s.pdf" % tag)


if __name__ == "__main__":
    run("output_planar_tracker_n10000.root", "")
    run("output_barrel_tracker_n10000.root", "_barrel")
