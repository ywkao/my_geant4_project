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

#include <stdlib.h>

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction(int type)
: fType(type)
{
    // note for fType
    // 0: nominal
    // 1: alternative
    // 2: pcb before odd layers
    // 3: pcb after odd layers
    // 4: pcb (lead) before odd layers
    // 5: pcb (lead) after odd layers
    // 6: pcb (aluminum) before odd layers
    // 7: pcb (aluminum) after odd layers
    // 8: pcb (air) before odd layers
    // 9: pcb (air) after odd layers
    // 10: pcb before odd layers & after even layers
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    DefineMaterials();
    DefineDimensions();

    return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  fEnvMaterial     = nist -> FindOrBuildMaterial("G4_Galactic");
  fTargetMaterial  = nist -> FindOrBuildMaterial("G4_Si");
  fPassiveMaterial = nist -> FindOrBuildMaterial("G4_Pb");

  G4bool isotopes = false;
  G4Element* Si = nist->FindOrBuildElement("Si", isotopes);
  G4Element* O  = nist->FindOrBuildElement("O" , isotopes);
  G4Element* C  = nist->FindOrBuildElement("C" , isotopes);
  G4Element* H  = nist->FindOrBuildElement("H" , isotopes);
  G4Element* Br = nist->FindOrBuildElement("Br", isotopes);

  pcb = new G4Material("PCB", 1.70*g/cm3, 5);
  pcb->AddElement(Si , 0.18077359);
  pcb->AddElement(O  , 0.4056325);
  pcb->AddElement(C  , 0.27804208);
  pcb->AddElement(H  , 0.068442752);
  pcb->AddElement(Br , 0.067109079);

  if(fType==4 || fType==5) {
    pcb = nist -> FindOrBuildMaterial("G4_Pb");
    printf(">>>>> [Note] pcb is replaced by lead\n");
  } else if (fType==6 || fType==7) {
    pcb = nist -> FindOrBuildMaterial("G4_Al");
    printf(">>>>> [Note] pcb is replaced by aluminum\n");
  } else if (fType==8 || fType==9) {
    pcb = nist -> FindOrBuildMaterial("G4_AIR");
    printf(">>>>> [Note] pcb is replaced by air\n");
  }

  nist -> FindOrBuildMaterial("PCB");

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

void DetectorConstruction::DefineDimensions()
{

  fCheckOverlaps = true;

  // dimensions
  n_pixels = 20; // quick test
  pixel_unit = 100*micrometer; // quick test
  default_pixel_length = 100*pixel_unit; // y direction
  default_pixel_width  = 100*pixel_unit; // x direction
  default_pixel_thick  = 300*micrometer; // z direction

  thickness_pcb = 1.60*mm;
  radiation_length_lead = 5.612*mm;

  worldXSize  = 500*cm;
  worldYZSize = 1.25*worldXSize;
  occupied_fraction = 0.96;

  detXSize  = occupied_fraction*worldXSize;
  detYZSize = occupied_fraction*worldYZSize;

  two_dim_array_size = detYZSize;
  two_dim_array_thick = default_pixel_thick;

  //----------------------------------------------------------------------------------------------------

  locations = {  50*mm,  75*mm, 100*mm, 125*mm, 150*mm, 175*mm, 200*mm, 225*mm, 250*mm, 275*mm,
                 300*mm, 325*mm, 350*mm, 375*mm, 400*mm, 425*mm, 450*mm, 475*mm, 500*mm, 525*mm,
                 550*mm, 575*mm, 600*mm, 625*mm, 650*mm, 675*mm
               };

  factor_passive_layer = {  0.564, 1.003, 0.98, 1.002, 0.979, 1.003, 0.978, 1.003, 0.978, 1.003,
                            0.979, 1.002, 0.979, 1.002, 0.979, 1.003, 0.978, 1.003, 1.557, 1.003,
                            1.558, 1.002, 1.558, 1.003, 1.557, 1.003
                         };

  if(fType!=1)
  {
    factor_passive_layer = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.}; // equal width for each layer
  }

  //----------------------------------------------------------------------------------------------------
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{

  // world
  G4Box* worldSolid = new G4Box("World", 0.5*worldXSize, 0.5*worldYZSize, 0.5*worldYZSize);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldSolid, fEnvMaterial, "World");
  G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), worldLV, "World", nullptr, 0, fCheckOverlaps);

  // detector
  G4Box *tracker = new G4Box("tracker", 0.5*detXSize, 0.5*detYZSize, 0.5*detYZSize);
  G4LogicalVolume *tracker_LV = new G4LogicalVolume(tracker, fEnvMaterial, "tracker_LV");
  new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), tracker_LV, "tracker_PV", worldLV, 999, fCheckOverlaps);

  //==================================================
  // start of my detector
  //==================================================

  G4LogicalVolume *pixel_logic[26];

  for(unsigned int i=0; i<locations.size(); ++i)
  {
      G4String idx = std::to_string(i);
      G4String tag = "-layer" + idx;
      G4String name_obj;
      G4String name_log;
      G4String name_vol;

      // insure all hits can be detected
      G4double scale = locations[i] / locations[0];
      G4cout << ">>> mycheck: " << i << ", scale = " << scale << G4endl;
      G4double pixel_length = scale*default_pixel_length; // z direction
      G4double pixel_width  = scale*default_pixel_width ; // x direction
      G4double pixel_thick  = default_pixel_thick; // y direction
      G4double space_y = 1.01*pixel_length;
      G4double space_x = 1.01*pixel_width;

      //+++++++++++++++++++++++++
      // pixel
      //+++++++++++++++++++++++++
      name_obj = "pixel_box"; name_log = "pixel_LV";
      G4Box *pixel = new G4Box(name_obj, 0.5*pixel_width, 0.5*pixel_length, 0.5*pixel_thick);
      pixel_logic[i] = new G4LogicalVolume(pixel, fTargetMaterial, name_log);

      //+++++++++++++++++++++++++
      // 2D array
      //+++++++++++++++++++++++++
      name_obj = "arr2D_box" + tag; name_log = "arr2D_LV" + tag; name_vol = "arr2D_PV" + tag;
      G4Box *two_dim_array = new G4Box(name_obj, 0.5*two_dim_array_size, 0.5*two_dim_array_size, 0.5*two_dim_array_thick);
      G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, fEnvMaterial, name_log);
      //new G4PVReplica(name_vol, one_dim_array_logic, two_dim_array_logic, kXAxis, n_pixels, space_x);
      G4int copyNo = 0;
      G4double firstYPosition = -0.5*n_pixels*space_y + 0.5*space_y; // center of left-most pixel
      G4double firstXPosition = -0.5*n_pixels*space_x + 0.5*space_x; // center of bottom 1d array

      G4ThreeVector position;
      for(G4int y=0; y<n_pixels; ++y)
      {
          G4double YPosition = firstYPosition + y*space_y;
          for(G4int x=0; x<n_pixels; ++x)
          {
              name_vol = "pixel";
              G4double XPosition = firstXPosition + x*space_x;
              position = G4ThreeVector(XPosition, YPosition, 0.);
              new G4PVPlacement(nullptr, position, pixel_logic[i], name_vol, two_dim_array_logic, copyNo, fCheckOverlaps);
              copyNo++;
          }
      }

      //+++++++++++++++++++++++++
      // passive material
      //+++++++++++++++++++++++++
      G4double plate_dimension = n_pixels*space_y;

      name_obj="pcb_box"+tag; name_log="pcb_LV"+tag; name_vol="pcb_PV"+tag;
      G4Box *pcb_box = new G4Box(name_obj, 0.5*plate_dimension, 0.5*plate_dimension, 0.5*thickness_pcb);
      G4LogicalVolume *pcb_LV = new G4LogicalVolume(pcb_box, pcb, name_log);

      name_obj="lead_box"+tag; name_log="lead_LV"+tag; name_vol="lead_PV"+tag;
      G4double thickness_lead = radiation_length_lead * factor_passive_layer[i];
      G4Box *lead_box = new G4Box(name_obj, 0.5*plate_dimension, 0.5*plate_dimension, 0.5*thickness_lead);
      G4LogicalVolume *lead_LV = new G4LogicalVolume(lead_box, fPassiveMaterial, name_log);

      //--------------------
      // odd layers
      //--------------------
      if(i%2==0) {
        // no pcb
        if(fType==0 || fType==1) {
            name_obj="lead_box"+tag; name_log="lead_LV"+tag; name_vol="lead_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] - 0.5*thickness_lead - 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, lead_LV, name_vol, tracker_LV, i, fCheckOverlaps);

        // pcb before
        } else if(fType==2 || fType==4 || fType==6 || fType==8 || fType==10) {
            name_obj="pcb_box"+tag; name_log="pcb_LV"+tag; name_vol="pcb_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] - 0.5*thickness_pcb - 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, pcb_LV, name_vol, tracker_LV, i, fCheckOverlaps);

            name_obj="lead_box"+tag; name_log="lead_LV"+tag; name_vol="lead_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] - 0.5*thickness_lead - 1.0*thickness_pcb - 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, lead_LV, name_vol, tracker_LV, i, fCheckOverlaps);

        // pcb after
        } else if(fType==3 || fType==5 || fType==7 || fType==9){
            name_obj="pcb_box"+tag; name_log="pcb_LV"+tag; name_vol="pcb_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] + 0.5*thickness_pcb + 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, pcb_LV, name_vol, tracker_LV, i, fCheckOverlaps);

            name_obj="lead_box"+tag; name_log="lead_LV"+tag; name_vol="lead_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] - 0.5*thickness_lead - 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, lead_LV, name_vol, tracker_LV, i, fCheckOverlaps);

        // print error messages
        } else {
            printf("[ERROR] fType is not an expected value\n");
            printf("0: nominal                                    \n ");
            printf("1: alternative                                \n ");
            printf("2: pcb before odd layers                      \n ");
            printf("3: pcb after odd layers                       \n ");
            printf("4: pcb (lead) before odd layers               \n ");
            printf("5: pcb (lead) after odd layers                \n ");
            printf("6: pcb (aluminum) before odd layers           \n ");
            printf("7: pcb (aluminum) after odd layers            \n ");
            printf("8: pcb (air) before odd layers                \n ");
            printf("9: pcb (air) after odd layers                 \n ");
            printf("10: pcb before odd layers & after even layers \n ");
            exit(2);
        }

      //--------------------
      // even layers
      //--------------------
      } else {
        // place pcb after even layers
        if(fType==10) {
            name_obj="pcb_box"+tag; name_log="pcb_LV"+tag; name_vol="pcb_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] + 0.5*thickness_pcb + 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, pcb_LV, name_vol, tracker_LV, i, fCheckOverlaps);

            name_obj="lead_box"+tag; name_log="lead_LV"+tag; name_vol="lead_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] - 0.5*thickness_lead - 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, lead_LV, name_vol, tracker_LV, i, fCheckOverlaps);

        // no pcb
        } else {
            name_obj="lead_box"+tag; name_log="lead_LV"+tag; name_vol="lead_PV"+tag;
            position = G4ThreeVector(0., 0., locations[i] - 0.5*thickness_lead - 0.5*pixel_thick);
            new G4PVPlacement(nullptr, position, lead_LV, name_vol, tracker_LV, i, fCheckOverlaps);
        }
      }

      //++++++++++++++++++++++++++++++
      // place element in detector
      //++++++++++++++++++++++++++++++
      new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,locations[i]), two_dim_array_logic, "layer", tracker_LV, i, fCheckOverlaps);
  }

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

  /***** the following lines are for debug only *****/
  //SetSensitiveDetector("tracker_LV", aTrackerSD, true);

  //SetSensitiveDetector("arr2D_LV-layer0", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer1", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer2", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer3", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer4", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer5", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer6", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer7", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer8", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer9", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer10", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer11", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer12", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer13", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer14", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer15", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer16", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer17", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer18", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer19", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer20", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer21", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer22", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer23", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer24", aTrackerSD, true);
  //SetSensitiveDetector("arr2D_LV-layer25", aTrackerSD, true);

  //SetSensitiveDetector("lead_LV-layer0", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer1", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer2", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer3", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer4", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer5", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer6", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer7", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer8", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer9", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer10", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer11", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer12", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer13", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer14", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer15", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer16", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer17", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer18", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer19", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer20", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer21", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer22", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer23", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer24", aTrackerSD, true);
  //SetSensitiveDetector("lead_LV-layer25", aTrackerSD, true);

  //SetSensitiveDetector("pcb_LV-layer0", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer1", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer2", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer3", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer4", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer5", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer6", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer7", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer8", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer9", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer10", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer11", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer12", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer13", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer14", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer15", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer16", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer17", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer18", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer19", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer20", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer21", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer22", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer23", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer24", aTrackerSD, true);
  //SetSensitiveDetector("pcb_LV-layer25", aTrackerSD, true);
  //**********************************************************************//
  
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector(0, 0, 3.8*tesla);
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}


  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
