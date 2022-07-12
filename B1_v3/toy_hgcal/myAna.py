#!/usr/bin/env python2
import os, subprocess
import ROOT
ROOT.gROOT.SetBatch(True)
#ROOT.gStyle.SetOptStat("emr")
ROOT.gStyle.SetOptStat(2210)
ROOT.gStyle.SetOptFit(1111)

my_stat_pos = [0.89, 0.87, 0.24, 0.08]
ROOT.gStyle.SetStatX(my_stat_pos[0])
ROOT.gStyle.SetStatY(my_stat_pos[1])
ROOT.gStyle.SetStatW(my_stat_pos[2])
ROOT.gStyle.SetStatH(my_stat_pos[3])
ROOT.gStyle.SetTextSize(1.2)

import toolbox.plot_utils as pu
import toolbox.MetaData as m

flag_add_reference = True
flag_add_reference = False

eos = "./plots"

c1 = ROOT.TCanvas("c1", "", 800, 600)
c1.SetGrid()
c1.SetTicks(1,1)
c1.SetLeftMargin(0.12)
c1.SetRightMargin(0.08)

c2 = ROOT.TCanvas("c2", "", 5600, 2400)
c2.SetLeftMargin(0.)
c2.SetRightMargin(0.)
c2.Divide(7,4)

X0 = {
"uniform" : [1.,2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,16.,17.,18.,19.,20.,21.,22.,23.,24.,25.,26.],
"alternative" : [0.564,1.567,2.547,3.549,4.528,5.531,6.509,7.512,8.49,9.493,10.472,11.474,12.453,13.455,14.434,15.437,16.415,17.418,18.975,19.978,21.536,22.538,24.096,25.099,26.656,27.659],
"PCB_by_Lead_before": [
1.2851,2.2851,3.5702,4.5702,5.8553,6.8553,8.1404,9.1404,10.4255,11.4255,12.7106,13.7106,14.9957,15.9957,17.2808,18.2808,19.5659,20.5659,21.851,22.851,24.1361,25.1361,26.4212,27.4212,28.7063,29.7063
],
"PCB_by_Lead_after": [
1.0,2.2851,3.2851,4.5702,5.5702,6.8553,7.8553,9.1404,10.1404,11.4255,12.4255,13.7106,14.7106,15.9957,16.9957,18.2808,19.2808,20.5659,21.5659,22.851,23.851,25.1361,26.1361,27.4212,28.4212,29.7063
],
}

#--------------------------------------------------
def create_directory(dir_output):
    if not os.path.isdir(dir_output):
        subprocess.call("mkdir -p %s" % dir_output, shell=True)
        #subprocess.call("cp -p %s/index.php %s" % (eos, dir_output), shell=True)

def annotate(rshift=0):
    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextFont(43)
    latex.SetTextAlign(11)
    #latex.SetTextSize(24)
    latex.SetTextSize(20)
    latex.DrawLatex( 0.12, 0.912, "#bf{Simulation} #it{work in progress}" )
    latex.DrawLatex( 0.62+rshift, 0.912, "1,000 events in toy detector" )

def draw_2D_ntuple(hname, v_hists, selection, color, is_first_plot=False):
    if is_first_plot: v_hists[0].Draw("r:z>>%s(2500, 300, 550, 3000, 0, 300)" % hname, selection)
    else:             v_hists[0].Draw("r:z>>%s(2500, 300, 550, 3000, 0, 300)" % hname, selection, "same")

    hnew = ROOT.gDirectory.Get(hname)
    hnew.SetTitle("")
    hnew.GetXaxis().SetTitle("Z [cm]")
    hnew.GetYaxis().SetTitle("Rxy [cm]")
    hnew.SetMarkerColor(color)
    if is_first_plot: hnew.Draw()
    else:             hnew.Draw("same")

def make_plot(varName, bool_make_logitudinal_profile):
    global myRootfiles, specified_directory, flag_add_reference
    is_number_of_hits = "h_nHits" in varName
    
    #++++++++++++++++++++++++++++++
    # Initiate
    #++++++++++++++++++++++++++++++
    bool_ntuple = "nt_" in varName
    bool_this_is_eta_phi = "Eta" in varName or "Phi" in varName
    bool_single_figures = bool_ntuple or bool_this_is_eta_phi
    #dir_output = specified_directory + "/" + pu.sub_directory[varName]
    #create_directory(dir_output)
    processes = [str(i) for i in range(1,27)]

    #++++++++++++++++++++++++++++++
    # Load histograms
    #++++++++++++++++++++++++++++++
    vf = []
    v_v_hists = []
    for rootfile in myRootfiles:
        print ">>> rootfile", rootfile
        fin = ROOT.TFile.Open(rootfile, "R")
        vf.append(fin)

        v_hists = []
        if bool_single_figures:
            v_hists = pu.load_single_histogram(fin, varName)
            v_v_hists.append(v_hists)
        else:
            v_hists = pu.load_histograms(fin, varName, processes)
            v_v_hists.append(v_hists)

    #++++++++++++++++++++++++++++++
    # Longitdinal profile
    #++++++++++++++++++++++++++++++
    if bool_make_logitudinal_profile:
        v_gr = []

        #------------------------------
        # loop over energy
        #------------------------------
        max_value = 0.
        for i, v_hists in enumerate(v_v_hists):
            #gr = pu.get_graph(varName, v_hists, is_number_of_hits)
            gr, max_value_idx = pu.get_graph(X0[layer_depth_types[i]], varName, v_hists, False)
            
            if max_value_idx > max_value:
                max_value = max_value_idx

            gr.SetLineStyle(2)
            if not flag_add_reference:
                gr.SetLineColor(colors[i])
                gr.SetMarkerColor(colors[i])
            else:
                gr.SetMarkerColor(ROOT.kBlue)
                gr.SetLineColor(ROOT.kBlue)
            v_gr.append(gr)

            # 26 plots in one
            c2.cd()
            for j, h in enumerate(v_hists):
                c2.cd(j+1)
                ROOT.gPad.SetLeftMargin(0.0)
                ROOT.gPad.SetRightMargin(0.0)
                ROOT.gPad.SetTopMargin(0.0)
                ROOT.gPad.SetBottomMargin(0.1)
                ROOT.gPad.SetTicks(1,1)
                h.GetXaxis().SetNdivisions(-5)
                h.GetXaxis().SetLabelSize(0.06)
                h.SetStats(0)
                h.Draw()

            output = specified_directory + "/" + varName + "_all_plots_" + tags[i]
            c2.SaveAs(output + ".pdf")

            continue

            ## individual plots
            #c1.cd()
            #for j, h in enumerate(v_hists):
            #    h.Draw()
            #    output = dir_output + "/" + varName + "_" + processes[j] + "_" + tags[i]
            #    c1.SaveAs(output + ".pdf")

        #------------------------------
        # logitudinal profile
        #------------------------------
        c1.cd()
        legend = ROOT.TLegend(0.60, 0.72, 0.85, 0.85)
        legend.SetLineColor(0)
        legend.SetTextSize(0.03)
        for i, gr in enumerate(v_gr):

            if not flag_add_reference:

                if i==0:
                    gr.Draw("alp")
                    gr.SetMaximum(max_value *1.5)
                else:
                    gr.Draw('lp;same')

                legend.AddEntry(gr, label[tags[i]], "lp")

                if i+1 == len(v_gr):
                    annotate()
                    legend.Draw("same")

                    output = specified_directory + "/" + varName
                    c1.SaveAs(output + ".pdf")

            else:
                gr.Draw("alp")
                v_gr_ref_mc[i].Draw('lp;same')
                v_gr_ref_data[i].Draw('lp;same')

                legend.Clear()
                legend.AddEntry(v_gr_ref_data[i], "2018 TB Data", "lp")
                legend.AddEntry(v_gr_ref_mc[i], "2018 TB MC", "lp")
                legend.AddEntry(gr, "D86 geometry", "lp")

                annotate()
                legend.Draw("same")

                output = specified_directory + "/" + varName + "_" + tags[i]
                c1.SaveAs(output + ".pdf")

def make_simple_plot():
    global myRootfiles, specified_directory
    #++++++++++++++++++++++++++++++
    # Load histograms
    #++++++++++++++++++++++++++++++
    vf, v_v_hists = [], []
    for rootfile in myRootfiles:
        print ">>> rootfile", rootfile
        fin = ROOT.TFile.Open(rootfile, "R")
        vf.append(fin)

        v_hists = []
        v_hists.append( pu.load_single_histogram(fin, "h_Edep_odd")  )
        v_hists.append( pu.load_single_histogram(fin, "h_Edep_even") )
        v_v_hists.append(v_hists)

    c1.cd()
    for i, v_hists in enumerate(v_v_hists):
        max_values = []
        max_values.append(v_hists[0].GetMaximum())
        max_values.append(v_hists[1].GetMaximum())
        max_value = max(max_values)

        # Edep odd layers
        v_hists[0].GetXaxis().SetTitleOffset(1.1)
        v_hists[0].GetXaxis().SetTitle("Deposited energy [MeV]")
        v_hists[0].GetYaxis().SetTitle("Entries / 10 MeV")
        v_hists[0].SetMaximum(max_value*1.2)
        v_hists[0].SetStats(0)
        v_hists[0].SetLineWidth(2)
        v_hists[0].SetLineColor(ROOT.kBlue)
        v_hists[0].Draw()

        # Edep even layers
        v_hists[1].SetStats(0)
        v_hists[1].SetLineWidth(2)
        v_hists[1].SetLineColor(ROOT.kRed)
        v_hists[1].Draw("same")

        if v_hists[0].GetMean() > 0. and v_hists[1].GetMean() > 0.:
            latex = ROOT.TLatex()
            latex.SetNDC()
            latex.SetTextFont(43)
            latex.SetTextAlign(11)
            latex.SetTextSize(24)

            sigmaEoverE = []
            sigmaEoverE.append( v_hists[0].GetStdDev() / v_hists[0].GetMean() )
            sigmaEoverE.append( v_hists[1].GetStdDev() / v_hists[1].GetMean() )

            latex.SetTextColor(ROOT.kBlue)
            latex.DrawLatex( 0.60, 0.70, "#sigma#left(E_{odd}#right) / #bar{E}_{odd} = %.4f" % sigmaEoverE[0] )
            latex.SetTextColor(ROOT.kRed)
            latex.DrawLatex( 0.60, 0.60, "#sigma#left(E_{even}#right) / #bar{E}_{even} = %.4f" % sigmaEoverE[1] )

        annotate()
        output = specified_directory + "/" + "h_Edep_odd_even_" + tags[i]
        c1.SaveAs(output + ".pdf")

#--------------------------------------------------

def run(myfin, mydin):
    global flag_add_reference, myRootfiles, specified_directory
    myRootfiles = myfin
    specified_directory = mydin

    create_directory( specified_directory )

    make_simple_plot()
    make_plot( "h_nHits", True )
    make_plot( "h_Edep" , True )

#--------------------------------------------------

if __name__ == "__main__":
    myRootfiles, specified_directory, label = [], "", {}

    colors = [ROOT.kBlack, ROOT.kRed, ROOT.kGreen+2, ROOT.kMagenta, ROOT.kBlue-7, ROOT.kRed-7]
    tags = ["all_hits", "forward_hits", "backward_hits"]
    layer_depth_types = ["alternative", "alternative", "alternative"]
    for tag in tags: label[tag] = tag
    run( m.input_files["backwardHits_v1"]    , eos + "/" + "longitudinal_profile_backward_hit_study" )

    layer_depth_types = ["uniform", "uniform", "uniform"]
    run( m.input_files["backwardHits_v2"]    , eos + "/" + "longitudinal_profile_backward_hit_study_v2" )

    exit()

    colors = [ROOT.kBlack, ROOT.kBlue, ROOT.kRed, ROOT.kGreen+2, ROOT.kMagenta, ROOT.kBlue-7, ROOT.kRed-7]
    tags = ["uniform_layers", "alternative_thickness", "with_PCB_before", "with_PCB_after"]
    layer_depth_types = ["uniform", "alternative", "uniform", "uniform"]
    layer_depth_types = ["uniform", "alternative", "PCB_by_Lead_before", "PCB_by_Lead_after"]
    for tag in tags: label[tag] = tag
    run( m.input_files["replace_PCB_by_Lead"], eos + "/" + "longitudinal_profile_replace_PCB_by_lead_v2" )

    colors = [ROOT.kBlack, ROOT.kBlue, ROOT.kRed, ROOT.kGreen+2, ROOT.kMagenta, ROOT.kBlue-7, ROOT.kRed-7]
    tags = ["uniform_layers", "alternative_thickness", "with_PCB_before", "with_PCB_after"]
    layer_depth_types = ["uniform", "alternative", "uniform", "uniform"]
    for tag in tags: label[tag] = tag
    run( m.input_files["toy_detector"], eos + "/" + "longitudinal_profile" )
