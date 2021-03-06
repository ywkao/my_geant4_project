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
/// \file TrackerSD.cc
/// \brief Implementation of the Tracker::TrackerSD class

#include "TrackerSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerSD::TrackerSD(const G4String& name,
                     const G4String& hitsCollectionName)
 : G4VSensitiveDetector(name)
{
  collectionName.insert(hitsCollectionName);
  G4cout << "TrackerSD::Constructor::hitsCollectionName: " << hitsCollectionName << G4endl << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerSD::~TrackerSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection
    = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce

  G4int hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TrackerSD::ProcessHits(G4Step* aStep,
                                     G4TouchableHistory*)
{
  // energy deposit
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
  G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

  if (edep==0.) return false;

  TrackerHit* newHit = new TrackerHit();

  newHit->SetEdep(edep);
  newHit->SetTrackID  (aStep->GetTrack()->GetTrackID());
  newHit->SetDetectorNb(preStepPoint->GetTouchableHandle()->GetCopyNumber());
  newHit->SetPrePosition (preStepPoint->GetPosition());
  newHit->SetPostPosition (postStepPoint->GetPosition());

  G4ThreeVector origin(0.,0.,0.);
  G4ThreeVector globalOrigin = preStepPoint->GetTouchableHandle()->GetHistory()->GetTopTransform().Inverse().TransformPoint(origin);  
  newHit->SetDetPos(globalOrigin);

  G4LogicalVolume* volume = preStepPoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4bool is_in_active_volume = volume->GetName() == "pixel_LV";
  newHit->SetVolumeName(volume->GetName());
  newHit->SetGoodHit(is_in_active_volume);

  bool debug = false;
  if(debug) {
    if(is_in_active_volume)
        G4cout << ">>>>> TrackerSD::volume->GetName = " << volume->GetName() << G4endl;
    else
        G4cout << ">>>>> TrackerSD::volume->GetName (else) = " << volume->GetName() << G4endl;
  }

  G4bool is_forward_direction = postStepPoint->GetPosition().getZ() - preStepPoint->GetPosition().getZ() > 0.;
  newHit->SetHitDirectiton(is_forward_direction);
  
  fHitsCollection->insert( newHit );

  //newHit->Print();

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerSD::EndOfEvent(G4HCofThisEvent*)
{
  // if ( verboseLevel>1 ) {
     G4int nofHits = fHitsCollection->entries();
     G4cout << "TrackerSD::EndOfEvent -------->Hits Collection: in this event they are " << nofHits
            << " hits in the tracker detectors: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

