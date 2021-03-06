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
/// \file TrackerHit.cc
/// \brief Implementation of the Tracker::TrackerHit class

#include "TrackerHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

namespace B1
{

G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerHit::TrackerHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackerHit::~TrackerHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TrackerHit::operator==(const TrackerHit& right) const
{
  return ( this == &right ) ? true : false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(fPostPosition);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackerHit::Print()
{
  bool debug = false;
  if(debug) {
  G4cout
     << std::fixed << std::setprecision(3)
     << " trackID: "   << std::setw(3) << fTrackID
     //<< " detetorNb: "   << fDetectorNb
     << " volume: "    << std::left << std::setw(16) << fVolumeName
     << " Edep: "      << std::right << std::setw(8) << fEdep/CLHEP::keV << "  " //G4BestUnit(fEdep,"Energy")
     << " IsForward: " << fFoward << ", "
     << " Pre-position: ("
     << std::setw(8) << fPrePosition.getX()/CLHEP::mm << ", "
     << std::setw(8) << fPrePosition.getY()/CLHEP::mm << ", "
     << std::setw(8) << fPrePosition.getZ()/CLHEP::mm << "), "
     << " Post-position: ("
     << std::setw(8) << fPostPosition.getX()/CLHEP::mm << ", "
     << std::setw(8) << fPostPosition.getY()/CLHEP::mm << ", "
     << std::setw(8) << fPostPosition.getZ()/CLHEP::mm << "), "
     << " DetPosition: ("
     << std::setw(8) << fDetPos.getX()/CLHEP::mm << ", " 
     << std::setw(8) << fDetPos.getY()/CLHEP::mm << ", "
     << std::setw(8) << fDetPos.getZ()/CLHEP::mm << ")"
     //<< " Position: "    << fPos/CLHEP::mm << "   " //G4BestUnit( fPos,"Length")
     //<< " DetPosition: " << fDetPos/CLHEP::mm //G4BestUnit( fDetPos,"Length")
     << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
