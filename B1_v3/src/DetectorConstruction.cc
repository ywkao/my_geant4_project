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
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
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
  G4Material* air  = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_WATER");
  G4Material* material_silicon = nist->FindOrBuildMaterial("G4_Si");

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  G4bool fCheckOverlaps = true;

  //==================================================
  // start of my detector
  //==================================================
  G4double worldXSize  = 200*cm;
  G4double worldYZSize = 1.25*worldXSize;
  G4double occupied_fraction = 0.96;

  G4double yt = cm; // unit in y coordinate, cm
  std::vector<G4double> locations = { 5*yt, 15*yt, 25*yt, 35*yt, 45*yt, 55*yt, 65*yt, 75*yt, 85*yt, 100*yt };

  G4int n_pixels = 10; // 100
  G4double pixel_unit   = 50*micrometer; // for quick test
  G4double default_pixel_length = 200*pixel_unit; // z direction
  G4double default_pixel_width  = 100*pixel_unit; // x direction
  G4double default_pixel_thick  = 300*pixel_unit; // y direction

  G4Material* targetMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

  // world
  G4Box* worldSolid = new G4Box("World", 0.5*worldXSize, 0.5*worldYZSize, 0.5*worldYZSize);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid, air, "World");
  G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), worldLV, "World", nullptr, 0, fCheckOverlaps);

  // detector
  G4double detXSize  = occupied_fraction*worldXSize;
  G4double detYZSize = occupied_fraction*worldYZSize;
  G4Box *tracker = new G4Box("tracker", 0.5*detXSize, 0.5*detYZSize, 0.5*detYZSize);
  G4LogicalVolume *tracker_LV = new G4LogicalVolume(tracker, air, "tracker_LV");
  G4VPhysicalVolume* tracker_PV = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), tracker_LV, "tracker_PV", worldLV, 1, fCheckOverlaps);

  //----------------------------------------------------------------------------------------------------

  for(unsigned int i=0; i<locations.size(); ++i)
  {
      //if(i>0) continue; // test one layer

      G4String idx = std::to_string(i);
      G4String tag = "-layer" + idx;
      G4String name_obj;
      G4String name_log;
      G4String name_vol;

      G4double scale = locations[i] / locations[0];
      G4cout << ">>> mycheck: " << i << ", scale = " << scale << G4endl;
      G4double pixel_length = scale*default_pixel_length; // z direction
      G4double pixel_width  = scale*default_pixel_width ; // x direction
      G4double pixel_thick  = default_pixel_thick ; // y direction
      G4double space_z = 1.2*pixel_length;
      G4double space_x = 1.2*pixel_width;
      G4double space_y = 3.0*pixel_thick;

      //+++++++++++++++++++++++++
      // pixel
      //+++++++++++++++++++++++++
      //name_obj = "pixel" + tag;
      //name_log = "logic-pixel" + tag;
      name_obj = "pixel_box";
      name_log = "pixel_LV";
      G4Box *pixel = new G4Box(name_obj, 0.5*pixel_width, 0.5*pixel_thick, 0.5*pixel_length);
      G4LogicalVolume *pixel_logic = new G4LogicalVolume(pixel, targetMaterial, name_log);

      //+++++++++++++++++++++++++
      // 2D array
      //+++++++++++++++++++++++++
      //name_obj = "box_two_dim_array" + tag;
      //name_log = "logic-two_dim_array" + tag;
      //name_vol = "two_dim_array" + tag;
      name_obj = "two_dim_array_box";
      name_log = "two_dim_array_LV";
      name_vol = "two_dim_array_PV";
      G4double two_dim_array_size = occupied_fraction*worldYZSize;
      G4double two_dim_array_thick = pixel_thick;
      G4Box *two_dim_array = new G4Box(name_obj, 0.5*two_dim_array_size, 0.5*two_dim_array_thick, 0.5*two_dim_array_size);
      G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, air, name_log);
      //new G4PVReplica(name_vol, one_dim_array_logic, two_dim_array_logic, kXAxis, n_pixels, space_x);
      G4int copyNo = 0;
      G4double firstZPosition = -0.5*n_pixels*space_z + 0.5*space_z; // center of left-most pixel
      G4double firstXPosition = -0.5*n_pixels*space_x + 0.5*space_x; // center of bottom 1d array
      for(G4int z=0; z<n_pixels; ++z)
      {
          G4double ZPosition = firstZPosition + z*space_z;
          for(G4int x=0; x<n_pixels; ++x)
          {
              //name_vol = "pixel" + tag + "-z" + std::to_string(z) + "-x" + std::to_string(x);
              name_vol = "pixel";
              G4double XPosition = firstXPosition + x*space_x;
              G4ThreeVector position = G4ThreeVector(XPosition, 0., ZPosition);
              new G4PVPlacement(nullptr, position, pixel_logic, name_vol, two_dim_array_logic, copyNo, fCheckOverlaps);
              copyNo++;
          }
      }

      //++++++++++++++++++++++++++++++
      // place element in detector
      //++++++++++++++++++++++++++++++
      //G4String layer_name = "layer" + idx;
      G4String layer_name = "layer";
      G4cout << ">>> layer_name = " << layer_name << G4endl;
      new G4PVPlacement(nullptr, G4ThreeVector(0.,locations[i],0.), two_dim_array_logic, layer_name, tracker_LV, i, fCheckOverlaps);
  }

  fScoringVolume = tracker_LV;

  //----------------------------------------------------------------------------------------------------
  /*
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

  //++++++++++++++++++++
  // box 
  //++++++++++++++++++++
  G4double box_sizeXY = 1*cm;
  G4double box_sizeZ  = 1*cm;
  G4Box* box = new G4Box("box", 0.5*box_sizeXY, 0.5*box_sizeXY, 0.5*box_sizeZ);
  G4LogicalVolume* box_log = new G4LogicalVolume(box, material_silicon, "boxL");
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), box_log, "boxP", logicEnv, false, 2, true);
  */

  //++++++++++++++++++++++++++++++
  //always return the physical World
  //++++++++++++++++++++++++++++++
  //return physWorld;
  return worldPV;
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
  // SetSensitiveDetector("Shape2", aTrackerSD, true);
  //SetSensitiveDetector("logic-Disc", aTrackerSD, true);
  //SetSensitiveDetector("boxL", aTrackerSD, true);
  SetSensitiveDetector("pixel_LV", aTrackerSD, true);
  //SetSensitiveDetector("logic-Target", aTrackerSD, true);
  
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector(0, 0, 2*tesla);
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}


  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
