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
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"

#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4SystemOfUnits.hh"
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
  G4Material* air  = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* silicon = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

  // Option to switch on/off checking of volumes overlaps
  G4bool fCheckOverlaps = true;

  //==================================================
  // start of my detector
  //==================================================
  G4double worldXSize  = 400*cm;
  G4double worldYZSize = 1.25*worldXSize;
  G4double occupied_fraction = 0.96;

  G4double yt = cm; // unit in y coordinate, cm
  std::vector<G4double> locations = { 5*yt, 15*yt, 25*yt, 35*yt, 45*yt, 55*yt, 65*yt, 75*yt, 85*yt, 100*yt };

  // z direction parameters
  G4double z_spacing_factor = 1.05;

  //G4int n_sections = 100;
  //G4int n_pixels = 1500;
  //G4double pixel_unit = micrometer;

  G4int n_sections = 10;
  G4int n_pixels = 30; // quick test
  G4double pixel_unit   = 50*micrometer; // quick test
  G4double default_pixel_length = 200*pixel_unit; // z direction
  G4double default_pixel_width  = 100*pixel_unit; // x direction
  G4double default_pixel_thick  = 300*pixel_unit; // y direction

  // world
  G4Box* worldSolid = new G4Box("World", 0.5*worldXSize, 0.5*worldYZSize, 0.5*worldYZSize);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid, air, "World");
  G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), worldLV, "World", nullptr, 0, fCheckOverlaps);

  // detector
  G4double detXSize  = occupied_fraction*worldXSize;
  G4double detYZSize = occupied_fraction*worldYZSize;
  G4Box *tracker = new G4Box("tracker", 0.5*detXSize, 0.5*detYZSize, 0.5*detYZSize);
  G4LogicalVolume *tracker_LV = new G4LogicalVolume(tracker, air, "tracker_LV");
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), tracker_LV, "tracker_PV", worldLV, 1, fCheckOverlaps);

  //----------------------------------------------------------------------------------------------------

  G4LogicalVolume *pixel_logic[10];

  for(unsigned int i=0; i<locations.size(); ++i)
  {
      //if(i>0) continue; // test one layer

      G4String idx = std::to_string(i);
      G4String tag = "-layer" + idx;
      G4String name_obj;
      G4String name_log;
      G4String name_vol;
      G4String layer_name = "layer";
      //G4String layer_name = "layer" + idx;

      G4double scale = locations[i] / locations[0];
      G4cout << ">>> mycheck: " << i << ", scale = " << scale << ", layer_name = " << layer_name << G4endl;
      G4double pixel_length = scale*default_pixel_length; // x direction
      G4double pixel_width  = scale*default_pixel_width ; // z direction
      G4double pixel_thick  = default_pixel_thick ; // y direction
      G4double space_z = 1.01*pixel_length;
      G4double space_x = 1.01*pixel_width;

      //+++++++++++++++++++++++++
      // pixel
      //+++++++++++++++++++++++++
      //name_obj = "pixel" + tag;
      //name_log = "logic-pixel" + tag;
      name_obj = "pixel_box";
      name_log = "pixel_LV";
      G4Box *pixel = new G4Box(name_obj, 0.5*pixel_width, 0.5*pixel_thick, 0.5*pixel_length);
      //G4LogicalVolume *pixel_logic = new G4LogicalVolume(pixel, silicon, name_log);
      pixel_logic[i] = new G4LogicalVolume(pixel, silicon, name_log);

      //+++++++++++++++++++++++++
      // 2D array
      //+++++++++++++++++++++++++

      G4double dPhi = twopi/n_pixels, half_dPhi = 0.5*dPhi;
      G4double cosdPhi = std::cos(half_dPhi);
      G4double tandPhi = std::tan(half_dPhi);
      //G4double ring_R1 = 0.5*cryst_dY/tandPhi;
      //G4double ring_R2 = (ring_R1+cryst_dZ)/cosdPhi;

      // ring
      G4double ring_R1 = locations[i] - 0.5*pixel_thick;
      G4double ring_R2 = std::sqrt( (locations[i]+0.5*pixel_thick)*(locations[i]+0.5*pixel_thick) + (0.5*pixel_width)*(0.5*pixel_width) );
      G4double detector_dZ = n_sections*pixel_width*z_spacing_factor;

      G4Tubs* solidRing = new G4Tubs("Ring", ring_R1, ring_R2, 0.5*pixel_width, 0., twopi);
      G4LogicalVolume* logicRing = new G4LogicalVolume(solidRing, air, "Ring_LV");

      // put pixelx in a ring
      for (G4int icrys = 0; icrys < n_pixels ; icrys++) {
        G4double phi = icrys*dPhi;
        G4RotationMatrix rotm  = G4RotationMatrix();
        rotm.rotateY(90*deg);
        rotm.rotateZ(90*deg);
        rotm.rotateZ(phi);
        G4ThreeVector uz = G4ThreeVector(std::cos(phi),  std::sin(phi),0.);
        G4ThreeVector position = (ring_R1+0.5*pixel_thick)*uz;
        G4Transform3D transform = G4Transform3D(rotm,position);

        new G4PVPlacement(transform,             //rotation,position
                          pixel_logic[i],           //its logical volume
                          "pixel",               //its name
                          logicRing,             //its mother  volume
                          false,                 //no boolean operation
                          icrys,                 //copy number
                          fCheckOverlaps);       // checking overlaps
      }

      name_obj = "two_dim_array_box" + tag;
      name_log = "two_dim_array_LV" + tag;
      name_vol = "two_dim_array_PV" + tag;
      G4double two_dim_array_size = occupied_fraction*worldYZSize;
      G4double two_dim_array_thick = pixel_thick;
      G4Tubs *two_dim_array = new G4Tubs(name_obj, ring_R1, ring_R2, z_spacing_factor*0.5*n_pixels*pixel_width, 0., twopi);
      G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, air, name_log);
      //new G4PVReplica(name_vol, one_dim_array_logic, two_dim_array_logic, kXAxis, n_pixels, space_x);

      // place rings within detector
      G4double OG = -0.5*detector_dZ;
      for (G4int iring = 0; iring < n_sections ; iring++) {
        OG += pixel_width*z_spacing_factor;
        new G4PVPlacement(0,                     //no rotation
                          G4ThreeVector(0,0,OG), //position
                          logicRing,             //its logical volume
                          "ring",                //its name
                          tracker_LV,            //its mother  volume
                          false,                 //no boolean operation
                          iring,                 //copy number
                          fCheckOverlaps);       // checking overlaps
      }

      //++++++++++++++++++++++++++++++
      // place element in detector
      //++++++++++++++++++++++++++++++
      new G4PVPlacement(nullptr, G4ThreeVector(0.,locations[i],0.), two_dim_array_logic, layer_name, tracker_LV, i, fCheckOverlaps);
      //break;
  }

  //fScoringVolume = tracker_LV;
  fScoringVolume = pixel_logic[0];

  //----------------------------------------------------------------------------------------------------

  return worldPV;
}

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  G4cout << G4endl << "DetectorConstruction::ConstructSDandField" << G4endl;

  G4String trackerChamberSDname = "/TrackerChamberSD";
  TrackerSD* aTrackerSD = new TrackerSD(trackerChamberSDname,"TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
  // Setting aTrackerSD to all logical volumes with the same name
  SetSensitiveDetector("pixel_LV", aTrackerSD, true);
  
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector(0, 0, 2*tesla);
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}


  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
