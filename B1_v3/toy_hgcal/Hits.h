//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jun 22 17:30:55 2022 by ROOT version 6.18/04
// from TTree Hits/Hits
// found on file: output_equal_lead_plate.root
//////////////////////////////////////////////////////////

#ifndef Hits_h
#define Hits_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <stdlib.h>

using namespace std;

class Hits {
    public :
        TTree          *fChain;   //!pointer to the analyzed TTree or TChain
        Int_t           fCurrent; //!current Tree number in a TChain

        // Fixed size dimensions of array or collections stored in the TTree if any.

        // Declaration of leaf types
        Int_t           evtNo;
        Int_t           Edep_keV;
        Int_t           NHits;
        Double_t        GenPX_MeV;
        Double_t        GenPY_MeV;
        Double_t        GenPZ_MeV;
        vector<double>  *Hits_DetX_mm;
        vector<double>  *Hits_DetY_mm;
        vector<double>  *Hits_DetZ_mm;
        vector<double>  *Hits_DetE_keV;
        vector<int>     *Hits_DetID;
        vector<int>     *Hits_isForward;

        // List of branches
        TBranch        *b_evtNo;   //!
        TBranch        *b_Edep_keV;   //!
        TBranch        *b_NHits;   //!
        TBranch        *b_GenPX_MeV;   //!
        TBranch        *b_GenPY_MeV;   //!
        TBranch        *b_GenPZ_MeV;   //!
        TBranch        *b_Hits_DetX_mm;   //!
        TBranch        *b_Hits_DetY_mm;   //!
        TBranch        *b_Hits_DetZ_mm;   //!
        TBranch        *b_Hits_DetE_keV;   //!
        TBranch        *b_Hits_DetID;   //!
        TBranch        *b_Hits_isForward;   //!

        Hits(TTree *tree=0, TString type="nominal");
        virtual ~Hits();
        virtual Int_t    Cut(Long64_t entry);
        virtual Int_t    GetEntry(Long64_t entry);
        virtual Long64_t LoadTree(Long64_t entry);
        virtual void     Init(TTree *tree, TString type);
        virtual void     Loop();
        virtual Bool_t   Notify();
        virtual void     Show(Long64_t entry = -1);
        virtual void     FillHists(double x, double y, double z, double e);
        virtual void     MakePlots(TString dir);
        virtual void     Register();
        virtual int      get_layer(double z);
        virtual void     reset_containers();
        virtual void     print_containers();
        virtual bool     pass_hit_type(unsigned int ihit);

    private:
        TString myHitType;

        TCanvas *c1;
        TFile *fout;
        TH1D *h_Hits_DetX_mm;
        TH1D *h_Hits_DetY_mm;
        TH1D *h_Hits_DetZ_mm;
        TH1D *h_Hits_DetE_MeV;
        TH1D *h_nHits;
        TH1D *h_Edep_odd;
        TH1D *h_Edep_even;
        //TH1D *h_Edep_odd_forward;
        //TH1D *h_Edep_even_forward;
        //TH1D *h_Edep_odd_backward;
        //TH1D *h_Edep_even_backward;

        vector<int> vc_nHits;
        vector<double> vc_Edep; // MeV
        vector<double> vc_Edep_odd_even; // MeV

        vector<TH1D*> vh_nHits;
        vector<TH1D*> vh_Edep;
        //vector<TH1D*> vh_nHits_forward;
        //vector<TH1D*> vh_Edep_forward;
        //vector<TH1D*> vh_nHits_backward;
        //vector<TH1D*> vh_Edep_backward;
};

#endif

#ifdef Hits_cxx
Hits::Hits(TTree *tree, TString type) : fChain(0) 
{
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.
    if (tree == 0) {
        printf("[ERROR] Please check input file in examine.cpp\n");
        exit(1);
    }
    Init(tree, type);
}

Hits::~Hits()
{
    if (!fChain) return;
    delete fChain->GetCurrentFile();
}

Int_t Hits::GetEntry(Long64_t entry)
{
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}
Long64_t Hits::LoadTree(Long64_t entry)
{
    // Set the environment to read one entry
    if (!fChain) return -5;
    Long64_t centry = fChain->LoadTree(entry);
    if (centry < 0) return centry;
    if (fChain->GetTreeNumber() != fCurrent) {
        fCurrent = fChain->GetTreeNumber();
        Notify();
    }
    return centry;
}

void Hits::Init(TTree *tree, TString type)
{
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the branch addresses and branch
    // pointers of the tree will be set.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).

    myHitType = type;

    // Set object pointer
    Hits_DetX_mm = 0;
    Hits_DetY_mm = 0;
    Hits_DetZ_mm = 0;
    Hits_DetE_keV = 0;
    Hits_DetID = 0;
    Hits_isForward = 0;
    // Set branch addresses and branch pointers
    if (!tree) return;
    fChain = tree;
    fCurrent = -1;
    fChain->SetMakeClass(1);

    fChain->SetBranchAddress("evtNo", &evtNo, &b_evtNo);
    fChain->SetBranchAddress("Edep_keV", &Edep_keV, &b_Edep_keV);
    fChain->SetBranchAddress("NHits", &NHits, &b_NHits);
    fChain->SetBranchAddress("GenPX_MeV", &GenPX_MeV, &b_GenPX_MeV);
    fChain->SetBranchAddress("GenPY_MeV", &GenPY_MeV, &b_GenPY_MeV);
    fChain->SetBranchAddress("GenPZ_MeV", &GenPZ_MeV, &b_GenPZ_MeV);
    fChain->SetBranchAddress("Hits_DetX_mm", &Hits_DetX_mm, &b_Hits_DetX_mm);
    fChain->SetBranchAddress("Hits_DetY_mm", &Hits_DetY_mm, &b_Hits_DetY_mm);
    fChain->SetBranchAddress("Hits_DetZ_mm", &Hits_DetZ_mm, &b_Hits_DetZ_mm);
    fChain->SetBranchAddress("Hits_DetE_keV", &Hits_DetE_keV, &b_Hits_DetE_keV);
    fChain->SetBranchAddress("Hits_DetID", &Hits_DetID, &b_Hits_DetID);
    fChain->SetBranchAddress("Hits_isForward", &Hits_isForward, &b_Hits_isForward);

    c1 = new TCanvas("c1", "", 800, 600);
    c1->SetTicks(1,1);
    c1->SetGrid(1);

    h_Hits_DetX_mm  = new TH1D("h_Hits_DetX_mm" , "", 50, 0, 100);
    h_Hits_DetY_mm  = new TH1D("h_Hits_DetY_mm" , "", 50, 0, 100);
    h_Hits_DetZ_mm  = new TH1D("h_Hits_DetZ_mm" , "", 100, 0, 1000);
    h_Hits_DetE_MeV = new TH1D("h_Hits_DetE_MeV", "", 50, 0, 1);
    h_nHits         = new TH1D("h_nHits", "", 100, 16000, 32000);

    // for evaluating sigma_E/E
    h_Edep_odd      = new TH1D("h_Edep_odd" , "", 200, 0, 2000);
    h_Edep_even     = new TH1D("h_Edep_even", "", 200, 0, 2000);

    // two elements
    vc_Edep_odd_even.push_back(0.);
    vc_Edep_odd_even.push_back(0.);

    // prepare containers for 26 layers
    TString name;
    for(int i=0; i<26; ++i) {
        vc_nHits.push_back(0);
        vc_Edep.push_back(0.);

        name = Form("h_nHits_%d", i+1);
        TH1D* h_nhits = new TH1D(name, "", 50, 0, 3500);
        vh_nHits.push_back(h_nhits);

        name = Form("h_Edep_%d", i+1);
        TH1D* h_Edep = new TH1D(name, "", 50, 0, 500);
        vh_Edep.push_back(h_Edep);
    }

    Notify();
}

Bool_t Hits::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

void Hits::Show(Long64_t entry)
{
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
}
Int_t Hits::Cut(Long64_t entry)
{
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    return 1;
}

void Hits::FillHists(double x, double y, double z, double e)
{
    h_Hits_DetX_mm  ->Fill(x);
    h_Hits_DetY_mm  ->Fill(y);
    h_Hits_DetZ_mm  ->Fill(z);
    h_Hits_DetE_MeV ->Fill(e);
}

void Hits::MakePlots(TString dir)
{
    h_Hits_DetX_mm  ->Draw(); c1->SaveAs(dir + "/h_Hits_DetX_mm.pdf");
    h_Hits_DetY_mm  ->Draw(); c1->SaveAs(dir + "/h_Hits_DetY_mm.pdf");
    h_Hits_DetZ_mm  ->Draw(); c1->SaveAs(dir + "/h_Hits_DetZ_mm.pdf");
    h_Hits_DetE_MeV ->Draw(); c1->SaveAs(dir + "/h_Hits_DetE_MeV.pdf");
    h_nHits ->Draw(); c1->SaveAs(dir + "/h_nHits.pdf");
    h_Edep_odd->Draw(); c1->SaveAs(dir + "/h_Edep_odd.pdf");
    h_Edep_even->Draw(); c1->SaveAs(dir + "/h_Edep_even.pdf");
}

void Hits::Register()
{
    fout = new TFile("./rootfiles/result.root", "RECREATE");
    fout->cd();

    h_Hits_DetX_mm->Write();
    h_Hits_DetY_mm->Write();
    h_Hits_DetZ_mm->Write();
    h_Hits_DetE_MeV->Write();
    h_nHits->Write();
    h_Edep_odd->Write();
    h_Edep_even->Write();
    
    for(int i=0; i<26; ++i) vh_nHits[i]->Write();
    for(int i=0; i<26; ++i) vh_Edep[i]->Write();

    fout->Close();
}

int Hits::get_layer(double z)
{
    int layer = 1 + (int)((z-50.)/25.);
    return layer;
}

void Hits::reset_containers()
{
    for(int i=0; i<26; ++i) {
        vc_nHits[i] = 0;
        vc_Edep[i] = 0.;
    }

    vc_Edep_odd_even[0] = 0.;
    vc_Edep_odd_even[1] = 0.;
}

void Hits::print_containers()
{
    for(int i=0; i<26; ++i) {
        if(i==0) printf("nHits = ");
        if(i+1==26) printf("%d\n", vc_nHits[i]);
        else printf("%d, ", vc_nHits[i]);
    }

    for(int i=0; i<26; ++i) {
        if(i==0) printf("Edep = ");
        if(i+1==26) printf("%.2f\n", vc_Edep[i]);
        else printf("%.2f, ", vc_Edep[i]);
    }
}

bool Hits::pass_hit_type(unsigned int ihit)
{
    bool pass = false;

    if(myHitType == "nominal" || myHitType == "forward_hits" || myHitType == "backward_hits") {
        // pass basic check of myHitType
    } else {
        printf("[ERROR] myHitType is not an exepcted TString.\n");
        exit(1);
    }

    if(myHitType == "forward_hits" && Hits_isForward->at(ihit)) {
        pass = true;
    } else if(myHitType == "backward_hits" && !Hits_isForward->at(ihit)) {
        pass = true;
    } else if(myHitType == "nominal") {
        pass = true;
    } else {
        pass = false;
    }

    return pass;
}

#endif // #ifdef Hits_cxx
