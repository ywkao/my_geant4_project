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
  G4Material* galactic  = nist->FindOrBuildMaterial("G4_Galactic");
  G4Material* silicon = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  G4Material* lead = G4NistManager::Instance()->FindOrBuildMaterial("G4_Pb");

  G4Material* envMaterial = galactic;

  // Option to switch on/off checking of volumes overlaps
  G4bool fCheckOverlaps = true;

  //==================================================
  // start of my detector
  //==================================================
  G4double worldXSize  = 500*cm;
  G4double worldYZSize = 1.25*worldXSize;
  G4double occupied_fraction = 0.96;

  G4double yt = mm; // unit in y coordinate, cm
  std::vector<G4double> locations = {  50*yt,  75*yt, 100*yt, 125*yt, 150*yt, 175*yt, 200*yt, 225*yt, 250*yt, 275*yt,
                                      300*yt, 325*yt, 350*yt, 375*yt, 400*yt, 425*yt, 450*yt, 475*yt, 500*yt, 525*yt,
                                      550*yt, 575*yt, 600*yt, 625*yt, 650*yt, 675*yt,
                                    };

  G4int n_pixels = 2000;
  G4double pixel_unit = micrometer;
  //G4int n_pixels = 10; // quick test
  //G4double pixel_unit = 200*micrometer; // quick test
  G4double default_pixel_length = 100*pixel_unit; // y direction
  G4double default_pixel_width  = 100*pixel_unit; // x direction
  G4double default_pixel_thick  = 300*micrometer; // z direction

  // world
  G4Box* worldSolid = new G4Box("World", 0.5*worldXSize, 0.5*worldYZSize, 0.5*worldYZSize);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid, envMaterial, "World");
  G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), worldLV, "World", nullptr, 0, fCheckOverlaps);

  // detector
  G4double detXSize  = occupied_fraction*worldXSize;
  G4double detYZSize = occupied_fraction*worldYZSize;
  G4Box *tracker = new G4Box("tracker", 0.5*detXSize, 0.5*detYZSize, 0.5*detYZSize);
  G4LogicalVolume *tracker_LV = new G4LogicalVolume(tracker, envMaterial, "tracker_LV");
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), tracker_LV, "tracker_PV", worldLV, 999, fCheckOverlaps);

  //----------------------------------------------------------------------------------------------------

  G4LogicalVolume *pixel_logic[26];

  for(unsigned int i=0; i<locations.size(); ++i)
  {
      G4String idx = std::to_string(i);
      G4String tag = "-layer" + idx;
      G4String name_obj;
      G4String name_log;
      G4String name_vol;
      G4String layer_name = "layer";
      //G4String layer_name = "layer" + idx;

      // insure all hits can be detected
      G4double scale = locations[i] / locations[0];
      G4cout << ">>> mycheck: " << i << ", scale = " << scale << ", layer_name = " << layer_name << G4endl;
      G4double pixel_length = scale*default_pixel_length; // z direction
      G4double pixel_width  = scale*default_pixel_width ; // x direction
      G4double pixel_thick  = default_pixel_thick ; // y direction
      G4double space_y = 1.01*pixel_length;
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
      G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, envMaterial, name_log);
      //new G4PVReplica(name_vol, one_dim_array_logic, two_dim_array_logic, kXAxis, n_pixels, space_x);
      G4int copyNo = 0;
      G4double firstYPosition = -0.5*n_pixels*space_y + 0.5*space_y; // center of left-most pixel
      G4double firstXPosition = -0.5*n_pixels*space_x + 0.5*space_x; // center of bottom 1d array
      for(G4int y=0; y<n_pixels; ++y)
      {
          G4double YPosition = firstYPosition + y*space_y;
          for(G4int x=0; x<n_pixels; ++x)
          {
              name_vol = "pixel";
              G4double XPosition = firstXPosition + x*space_x;
              G4ThreeVector position = G4ThreeVector(XPosition, YPosition, 0.);
              new G4PVPlacement(nullptr, position, pixel_logic[i], name_vol, two_dim_array_logic, copyNo, fCheckOverlaps);
              copyNo++;
          }
      }

      //+++++++++++++++++++++++++
      // passive material
      //+++++++++++++++++++++++++
      name_obj = "lead_box" + tag;
      name_log = "lead_LV" + tag;
      name_vol = "lead_PV" + tag;
      G4double radiation_length_lead = 5.612*mm;
      G4double thickness_lead = radiation_length_lead;
      G4double lead_plate_dimension = n_pixels*space_y;
      G4Box *lead_box = new G4Box(name_obj, 0.5*lead_plate_dimension, 0.5*lead_plate_dimension, 0.5*thickness_lead);
      G4LogicalVolume *lead_LV = new G4LogicalVolume(lead_box, lead, name_log);
      G4ThreeVector position = G4ThreeVector(0., 0., locations[i] - 0.5*thickness_lead - 0.5*pixel_thick);
      new G4PVPlacement(nullptr, position, lead_LV, name_vol, tracker_LV, i, fCheckOverlaps);

      //++++++++++++++++++++++++++++++
      // place element in detector
      //++++++++++++++++++++++++++++++
      new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,locations[i]), two_dim_array_logic, layer_name, tracker_LV, i, fCheckOverlaps);

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
