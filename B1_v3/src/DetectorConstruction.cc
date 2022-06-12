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
  G4double worldXSize  = 200*cm;
  G4double worldYZSize = 1.25*worldXSize;
  G4double occupied_fraction = 0.96;

  G4double yt = cm; // unit in y coordinate, cm
  std::vector<G4double> locations = { 5*yt, 15*yt, 25*yt, 35*yt, 45*yt, 55*yt, 65*yt, 75*yt, 85*yt, 95*yt };

  //G4int n_pixels = 500; // 10
  //G4double pixel_unit   = micrometer; // 50*micrometer
  G4int n_pixels = 10; // quick test
  G4double pixel_unit   = 200*micrometer; // quick test
  G4double default_pixel_length = 100*pixel_unit; // y direction
  G4double default_pixel_width  = 100*pixel_unit; // x direction
  G4double default_pixel_thick  = 300*micrometer; // z direction

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
      G4String idx = std::to_string(i);
      G4String tag = "-layer" + idx;
      G4String name_obj;
      G4String name_log;
      G4String name_vol;
      G4String layer_name = "layer";
      //G4String layer_name = "layer" + idx;

      G4double scale = locations[i] / locations[0]; scale = 1.0;
      G4cout << ">>> mycheck: " << i << ", scale = " << scale << ", layer_name = " << layer_name << G4endl;
      G4double pixel_length = scale*default_pixel_length; // z direction
      G4double pixel_width  = scale*default_pixel_width ; // x direction
      G4double pixel_thick  = default_pixel_thick ; // y direction
      G4double space_z = 1.01*pixel_length;
      G4double space_x = 1.01*pixel_width;

      //+++++++++++++++++++++++++
      // pixel
      //+++++++++++++++++++++++++
      name_obj = "pixel_box";
      name_log = "pixel_LV";
      G4Box *pixel = new G4Box(name_obj, 0.5*pixel_width, 0.5*pixel_length, 0.5*pixel_thick);
      pixel_logic[i] = new G4LogicalVolume(pixel, silicon, name_log);

      //+++++++++++++++++++++++++
      // 2D array
      //+++++++++++++++++++++++++
      name_obj = "two_dim_array_box" + tag;
      name_log = "two_dim_array_LV" + tag;
      name_vol = "two_dim_array_PV" + tag;
      G4double two_dim_array_size = occupied_fraction*worldYZSize;
      G4double two_dim_array_thick = pixel_thick;
      G4Box *two_dim_array = new G4Box(name_obj, 0.5*two_dim_array_size, 0.5*two_dim_array_size, 0.5*two_dim_array_thick);
      G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, air, name_log);
      //new G4PVReplica(name_vol, one_dim_array_logic, two_dim_array_logic, kXAxis, n_pixels, space_x);
      G4int copyNo = 0;
      G4double firstYPosition = -0.5*n_pixels*space_z + 0.5*space_z; // center of left-most pixel
      G4double firstXPosition = -0.5*n_pixels*space_x + 0.5*space_x; // center of bottom 1d array
      for(G4int y=0; y<n_pixels; ++y)
      {
          G4double YPosition = firstYPosition + y*space_z;
          for(G4int x=0; x<n_pixels; ++x)
          {
              name_vol = "pixel";
              G4double XPosition = firstXPosition + x*space_x;
              G4ThreeVector position = G4ThreeVector(XPosition, YPosition, 0.);
              new G4PVPlacement(nullptr, position, pixel_logic[i], name_vol, two_dim_array_logic, copyNo, fCheckOverlaps);
              copyNo++;
          }
      }

      //++++++++++++++++++++++++++++++
      // place element in detector
      //++++++++++++++++++++++++++++++
      new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,locations[i]), two_dim_array_logic, layer_name, tracker_LV, i, fCheckOverlaps);
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
