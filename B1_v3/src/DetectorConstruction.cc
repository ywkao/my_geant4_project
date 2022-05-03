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
/// \file DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"


#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "TrackerSD.hh"
#include "G4SDManager.hh"
#include "G4PVReplica.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  G4double env_sizeXY = 20*cm, env_sizeZ = 30*cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_WATER");
  G4Material* material_silicon = nist->FindOrBuildMaterial("G4_Si");

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //++++++++++++++++++++++++++++++
  // World
  //++++++++++++++++++++++++++++++
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

  G4Box* solidWorld = new G4Box("World", 0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, checkOverlaps);

  //++++++++++++++++++++++++++++++
  // Envelope
  //++++++++++++++++++++++++++++++
  G4Box* solidEnv = new G4Box("Envelope", 0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ);
  G4LogicalVolume* logicEnv = new G4LogicalVolume(solidEnv, env_mat, "Envelope");
  new G4PVPlacement(0, G4ThreeVector(), logicEnv, "Envelope", logicWorld, false, 0, checkOverlaps);

  fScoringVolume = logicEnv;

  // //++++++++++++++++++++++++++++++
  // // Shape 1
  // //++++++++++++++++++++++++++++++
  // //G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");
  // G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_Si");
  // G4ThreeVector pos1 = G4ThreeVector(0, 0, -7*cm);

  // // Conical section shape
  // G4double shape1_rmina =  0.*cm, shape1_rmaxa = 2.*cm;
  // G4double shape1_rminb =  0.*cm, shape1_rmaxb = 4.*cm;
  // G4double shape1_hz = 3.*cm;
  // G4double shape1_phimin = 0.*deg, shape1_phimax = 360.*deg;
  // G4Cons* solidShape1 = new G4Cons("Shape1", shape1_rmina, shape1_rmaxa, shape1_rminb, shape1_rmaxb, shape1_hz, shape1_phimin, shape1_phimax);
  // G4LogicalVolume* logicShape1 = new G4LogicalVolume(solidShape1, shape1_mat, "Shape1");
  // new G4PVPlacement(0, pos1, logicShape1, "Shape1", logicEnv, false, 0, checkOverlaps);

  // //++++++++++++++++++++++++++++++
  // // Shape 2
  // //++++++++++++++++++++++++++++++
  // G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_BONE_COMPACT_ICRU");
  // G4ThreeVector pos2 = G4ThreeVector(0, 0, 7*cm);

  // // Trapezoid shape
  // G4double shape2_dxa = 12*cm, shape2_dxb = 12*cm;
  // G4double shape2_dya = 10*cm, shape2_dyb = 16*cm;
  // G4double shape2_dz  = 6*cm;
  // G4Trd* solidShape2 = new G4Trd("Shape2", 0.5*shape2_dxa, 0.5*shape2_dxb, 0.5*shape2_dya, 0.5*shape2_dyb, 0.5*shape2_dz);
  // G4LogicalVolume* logicShape2 = new G4LogicalVolume(solidShape2, shape2_mat, "Shape2");
  // new G4PVPlacement(0, pos2, logicShape2, "Shape2", logicEnv, false, 1, checkOverlaps);

  // // Set Shape2 as scoring volume
  // fScoringVolume = logicShape2;

  //++++++++++++++++++++
  // disc
  //++++++++++++++++++++
  // c. Create a target disc (a full tube), defining first its dimensions:
   G4double discZLength = 0.1*mm;
   G4double discRadius  = 3*cm;

   // tube = name, min radius, max radius, z half length, start phi, delta phi
   G4VSolid* tube 
     = new G4Tubs("tube", 0, 2*discRadius, 0.5*env_sizeZ, 0, twopi*rad);
   G4LogicalVolume* tube_log
     = new G4LogicalVolume(tube, env_mat, "tubeL");   // name
   //G4VPhysicalVolume* tube_phy =
   //    new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), tube_log, "tubeP", logicWorld, false, 2);
  
   G4VSolid* disc 
     = new G4Tubs("disc-Target", 0, discRadius, discZLength, 0, twopi*rad);
   G4LogicalVolume* disc_log
     = new G4LogicalVolume(disc, material_silicon, "logic-Disc");
   //G4VPhysicalVolume* discPhysical =
   //    new G4PVReplica("detector", disc_log, tube_log, kZAxis, 4, discZLength*1.1);

   G4double init_position = -2.25*cm;
   for(int i=0; i<10; ++i) {
       G4double increment = (double)i * 0.5*cm;
       G4double zposition = init_position + increment;
       new G4PVPlacement(nullptr, G4ThreeVector(0.5*cm, 0.5*cm, zposition), disc_log, "Disc", logicEnv, false, i+2, true);
   }

    // //new G4PVPlacement(nullptr,                 // (no) rotation
    // //		     G4ThreeVector(0.5*cm, 0.5*cm, 0.5*cm), // translation
    // //		     disc_log,             // its logical volume
    // //		     "Disc",                  // its name
    // //		     logicEnv,            // its mother volume
    // //		     false,                   // not used
    // //		     2,                       // copy number
    // //		     true);                   // enable overlap check

    // //new G4PVPlacement(nullptr,                 // (no) rotation
    // //		     G4ThreeVector(0.5*cm, 0.5*cm, -0.5*cm), // translation
    // //		     disc_log,             // its logical volume
    // //		     "Disc",                  // its name
    // //		     logicEnv,            // its mother volume
    // //		     false,                   // not used
    // //		     3,                       // copy number
    // //		     true);                   // enable overlap check


  //++++++++++++++++++++++++++++++
  //always return the physical World
  //++++++++++++++++++++++++++++++
  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  G4String trackerChamberSDname = "/TrackerChamberSD";
  TrackerSD* aTrackerSD = new TrackerSD(trackerChamberSDname,"TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
  // Setting aTrackerSD to all logical volumes with the same name
  // of "Chamber_LV".
  // SetSensitiveDetector("Shape1", aTrackerSD, true);
  SetSensitiveDetector("logic-Disc", aTrackerSD, true);
  // SetSensitiveDetector("Shape2", aTrackerSD, true);
  
  // // Create global magnetic field messenger.
  // // Uniform magnetic field is then created automatically if
  // // the field value is not zero.
  // G4ThreeVector fieldValue = G4ThreeVector();
  // fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  // fMagFieldMessenger->SetVerboseLevel(1);

  // // Register the field messenger for deleting
  // G4AutoDelete::Register(fMagFieldMessenger);
}


  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
