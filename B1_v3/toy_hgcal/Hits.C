#define Hits_cxx
#include "Hits.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Hits::Loop()
{
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break; nb = fChain->GetEntry(jentry); nbytes += nb;

        reset_containers();
        h_nHits->Fill(Hits_DetX_mm->size());
        for(unsigned int ihit=0; ihit<Hits_DetX_mm->size(); ++ihit) {
            // condition for nominal / forward / backward hits
            if( !pass_hit_type(ihit) ) continue;

            // raw info
            double x = Hits_DetX_mm  -> at(ihit);
            double y = Hits_DetY_mm  -> at(ihit);
            double z = Hits_DetZ_mm  -> at(ihit);
            double e = Hits_DetE_keV -> at(ihit) / 1000.; // MeV

            h_Hits_DetX_mm  -> Fill( x );
            h_Hits_DetY_mm  -> Fill( y );
            h_Hits_DetZ_mm  -> Fill( z );
            h_Hits_DetE_MeV -> Fill( e );

            // derived info
            int layer = get_layer(Hits_DetZ_mm->at(ihit));
            int idx = layer - 1;
            vc_nHits[idx] += 1;
            vc_Edep[idx] += e;

            if(idx%2==0)
                vc_Edep_odd_even[0] += e;
            else
                vc_Edep_odd_even[1] += e;

        } // end of hit loop
        
        //if(jentry%100==0) print_containers();

        // store event information
        h_Edep_odd ->Fill(vc_Edep_odd_even[0]);
        h_Edep_even->Fill(vc_Edep_odd_even[1]);

        for(int i=0; i<26; ++i) {
            vh_nHits[i]->Fill(vc_nHits[i]);
            vh_Edep[i]->Fill(vc_Edep[i]);
        }

    } // end of event loop

}
