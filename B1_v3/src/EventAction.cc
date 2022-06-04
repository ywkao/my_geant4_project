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


//namespace {
//
//// Utility function which finds a hit collection with the given Id
//// and print warnings if not found
//G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
//  auto hce = event->GetHCofThisEvent();
//  if (!hce) {
//      G4ExceptionDescription msg;
//      msg << "No hits collection of this event found." << G4endl;
//      G4Exception("EventAction::EndOfEventAction()",
//                  "Code001", JustWarning, msg);
//      return nullptr;
//  }
//
//  auto hc = hce->GetHC(collId);
//  if ( ! hc) {
//    G4ExceptionDescription msg;
//    msg << "Hits collection " << collId << " of this event not found." << G4endl;
//    G4Exception("EventAction::EndOfEventAction()",
//                "Code001", JustWarning, msg);
//  }
//  return hc;
//}
//
//}

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
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
  
  fSiHitsX.clear();
  fSiHitsY.clear();
  fSiHitsZ.clear();
  fSiHitsEdep.clear();
  fDetID.clear();

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

  // accumulate statistics in run action // Well, I'm not sure how to do
  //fRunAction->AddEdep(fEdep);
  
  G4cout << "EventAction::EndOfEventAction : start to fill the hit vector " << G4endl;
  for (unsigned long i = 0; i < hc->GetSize(); ++i) {
    auto hit = static_cast<TrackerHit*>(hc->GetHit(i));
    G4cout << "  hit " << std::setw(3) << i
    << "  DetId: " << std::setw(3) << hit->GetDetectorNb()
    << "  detector position ("
    << std::setw(8) << hit->GetDetPos().getX()/CLHEP::mm << ", " 
    << std::setw(8) << hit->GetDetPos().getY()/CLHEP::mm << ", "
    << std::setw(8) << hit->GetDetPos().getZ()/CLHEP::mm << ")   "
    << "  Edep: " << hit->GetEdep()/CLHEP::keV
    << G4endl;

    //fEdep += hit->GetEdep(); // sum up in src/SteppingAction.cc

    fSiHitsX.push_back(hit->GetDetPos().x()/CLHEP::mm);    
    fSiHitsY.push_back(hit->GetDetPos().y()/CLHEP::mm);
    fSiHitsZ.push_back(hit->GetDetPos().z()/CLHEP::mm);
    fSiHitsEdep.push_back(hit->GetEdep()/CLHEP::keV);
    fDetID.push_back(hit->GetDetectorNb());
  }

  G4cout << " fEdep " << fEdep/CLHEP::keV << G4endl;

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
