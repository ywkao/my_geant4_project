//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file EventAction.cc
/// \brief Implementation of the B1::EventAction class

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

#include "TrackerHit.hh"

using std::array;
using std::vector;

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction)
: fRunAction(runAction)
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  G4cout << "EventAction::Constructor" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event*)
{
  fEdep = 0.;
  fRunAction->ResetHitInfoContainer();

  G4cout << G4endl
  << "------------------------------------------------------------------------------------------------------------------------" << G4endl
  << "EventAction::BeginOfEventAction" << G4endl
  << "------------------------------------------------------------------------------------------------------------------------" << G4endl
  << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{

  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  auto primary = event->GetPrimaryVertex(0)->GetPrimary(0);
  G4int eventID = event->GetEventID();

  G4cout << G4endl
    << "EventAction::EndOfEventAction : Event " << event->GetEventID()
    << G4endl;

  G4cout
    << "EventAction::EndOfEventAction : Simulation truth : "
    << primary->GetG4code()->GetParticleName()
    << " " << primary->GetMomentum() << G4endl;

  G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
  auto nhit = hc->GetSize();

  // not sure how to do this   
  // accumulate statistics in run action
  fRunAction->AddEdep(fEdep);
  
  G4cout << "EventAction::EndOfEventAction : start to fill the hit vector " << G4endl;
  for (unsigned long i = 0; i < hc->GetSize(); ++i) {
    auto hit = static_cast<TrackerHit*>(hc->GetHit(i));
    
    bool debug = false;
    if(debug) {
        G4cout << "  hit " << std::setw(3) << i
        << "  DetId: " << std::setw(3) << hit->GetDetectorNb()
        << "  detector position ("
        << std::setw(8) << hit->GetDetPos().getX()/CLHEP::mm << ", " 
        << std::setw(8) << hit->GetDetPos().getY()/CLHEP::mm << ", "
        << std::setw(8) << hit->GetDetPos().getZ()/CLHEP::mm << ")   "
        << "  Edep: " << hit->GetEdep()/CLHEP::keV
        << G4endl;
    }

    // fEdep is summed up in src/SteppingAction.cc
    //fEdep += hit->GetEdep();

    // store hit if it is in active layers
    if(hit->IsGoodHit()) {
        fRunAction->RegisterHitInfo(
                                    hit->GetDetectorNb(),
                                    hit->IsFowardHit(),
                                    hit->GetDetPos().x()/CLHEP::mm,
                                    hit->GetDetPos().y()/CLHEP::mm,
                                    hit->GetDetPos().z()/CLHEP::mm,
                                    hit->GetEdep()/CLHEP::keV
                                   );
    }

  }

  G4cout << " fEdep " << fEdep/CLHEP::keV << " keV" << G4endl;

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  analysisManager->FillH1(0, fEdep/CLHEP::MeV);
  analysisManager->FillNtupleIColumn(0, eventID);
  analysisManager->FillNtupleIColumn(1, fEdep/CLHEP::keV);
  analysisManager->FillNtupleIColumn(2,nhit);
  analysisManager->FillNtupleDColumn(3,primary->GetMomentum().x());
  analysisManager->FillNtupleDColumn(4,primary->GetMomentum().y());
  analysisManager->FillNtupleDColumn(5,primary->GetMomentum().z());

  analysisManager->AddNtupleRow();

  G4cout << G4endl << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
