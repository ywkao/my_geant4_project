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
/// \brief Implementation of the B2a::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "TrackerSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

using namespace B2;

namespace B2a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

DetectorConstruction::DetectorConstruction()
{
  fMessenger = new DetectorMessenger(this);

  fNbOfChambers = 5;
  fLogicChamber = new G4LogicalVolume*[fNbOfChambers];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete [] fLogicChamber;
  delete fStepLimit;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Material definition

  G4NistManager* nistManager = G4NistManager::Instance();

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR");

  // Lead defined using NIST Manager
  fTargetMaterial  = nistManager->FindOrBuildMaterial("G4_Si");

  // Xenon gas defined using NIST Manager
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_AIR");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  G4Material* air  = G4Material::GetMaterial("G4_AIR");

  // Sizes of the principal geometrical components (solids)

  G4double chamberSpacing = 80*cm; // from chamber center to center!

  G4double chamberWidth = 20.0*cm; // width of the chambers
  G4double targetLength =  5.0*cm; // full length of Target

  G4double trackerLength = (fNbOfChambers+1)*chamberSpacing;

  G4double worldLength = 1.2 * (2*targetLength + trackerLength);

  G4double targetRadius  = 0.5*targetLength;   // Radius of Target
  targetLength = 0.5*targetLength;             // Half length of the Target
  G4double trackerSize   = 0.5*trackerLength;  // Half length of the Tracker

  // Definitions of Solids, Logical Volumes, Physical Volumes

  // u, v, w are the daughter axes, projected on the mother frame
  G4double phi = 90*deg;
  G4ThreeVector u = G4ThreeVector( std::cos(phi), std::sin(phi),0.);
  G4ThreeVector v = G4ThreeVector(0, 0, -1);
  G4ThreeVector w = G4ThreeVector(-std::sin(phi), std::cos(phi),0.);
  G4RotationMatrix *myRotation  = new G4RotationMatrix(u, v, w);

  //==================================================
  // start of my detector
  //==================================================
  G4double worldXSize  = 10*cm; // 200*cm
  G4double worldYZSize = 1.25*worldXSize;
  G4double occupied_fraction = 0.96;

  G4double yt = 0.5*mm; // unit in y coordinate, cm
  std::vector<G4double> locations = { 5*yt, 15*yt, 25*yt, 35*yt, 45*yt, 55*yt, 65*yt, 75*yt, 85*yt, 100*yt };

  G4int n_pixels = 10; // 100
  G4double pixel_unit   = micrometer;
  G4double default_pixel_length = 200*pixel_unit; // z direction
  G4double default_pixel_width  = 100*pixel_unit; // x direction
  G4double default_pixel_thick  = 300*pixel_unit; // y direction

  G4Material* targetMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

  // world
  G4Box* worldSolid = new G4Box("solid-World", 0.5*worldXSize, 0.5*worldYZSize, 0.5*worldYZSize);
  G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, air, "logic-World");
  G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), worldLogical, "World", nullptr, false, 0);

  // env
  G4double envXSize  = occupied_fraction*worldXSize;
  G4double envYZSize = occupied_fraction*worldYZSize;
  G4Box* envSolid = new G4Box("env-box", 0.5*envXSize, 0.5*envYZSize, 0.5*envYZSize);
  G4LogicalVolume* envLogical = new G4LogicalVolume(envSolid, air, "logic-env");
  G4VPhysicalVolume *envPhysical = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), envLogical, "Env", worldLogical, false, 1);

  // detector
  G4double detXSize  = occupied_fraction*envXSize;
  G4double detYZSize = occupied_fraction*envYZSize;
  G4Box *mybox = new G4Box("mybox", 0.5*detXSize, 0.5*detYZSize, 0.5*detYZSize);
  G4LogicalVolume *mylog = new G4LogicalVolume(mybox, air, "logic-Target");

  //----------------------------------------------------------------------------------------------------

  //for(unsigned int i=0; i<locations.size(); ++i)
  //{
  //    G4String idx = std::to_string(i+1);
  //    G4String tag = "-layer" + idx;
  //    G4String name_obj;
  //    G4String name_log;
  //    G4String name_vol;

  //    G4double scale = locations[i] / locations[0];
  //    G4cout << ">>> mycheck: " << i << ", scale = " << scale << G4endl;
  //    G4double pixel_length = scale*default_pixel_length; // z direction
  //    G4double pixel_width  = scale*default_pixel_width ; // x direction
  //    G4double pixel_thick  = default_pixel_thick ; // y direction
  //    G4double space_z = 1.2*pixel_length;
  //    G4double space_x = 1.2*pixel_width;
  //    G4double space_y = 3.0*pixel_thick;

  //    // pixel
  //    name_obj = "pixel" + tag;
  //    name_log = "logic-pixel" + tag;
  //    G4Box *pixel = new G4Box(name_obj, 0.5*pixel_width, 0.5*pixel_thick, 0.5*pixel_length);
  //    G4LogicalVolume *pixel_logic = new G4LogicalVolume(pixel, targetMaterial, name_log);

  //    // 1D array
  //    name_obj = "box_one_dim_array" + tag;
  //    name_log = "logic-one_dim_array" + tag;
  //    name_vol = "one_dim_array" + tag;
  //    G4double one_dim_array_size = occupied_fraction*envYZSize;
  //    G4double one_dim_array_thick = pixel_thick;
  //    G4Box *one_dim_array = new G4Box(name_obj, 0.5*one_dim_array_thick, 0.5*one_dim_array_thick, 0.5*one_dim_array_size);
  //    G4LogicalVolume *one_dim_array_logic = new G4LogicalVolume(one_dim_array, air, name_log);
  //    new G4PVReplica(name_vol, pixel_logic, one_dim_array_logic, kZAxis, n_pixels, space_z);

  //    // 2D array
  //    name_obj = "box_two_dim_array" + tag;
  //    name_log = "logic-two_dim_array" + tag;
  //    name_vol = "two_dim_array" + tag;
  //    G4double two_dim_array_size = occupied_fraction*envYZSize;
  //    G4double two_dim_array_thick = pixel_thick;
  //    G4Box *two_dim_array = new G4Box(name_obj, 0.5*two_dim_array_size, 0.5*two_dim_array_thick, 0.5*two_dim_array_size);
  //    G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, air, name_log);
  //    new G4PVReplica(name_vol, one_dim_array_logic, two_dim_array_logic, kXAxis, n_pixels, space_x);

  //    // place element in detector
  //    G4String layer_name = "layer" + idx;
  //    G4cout << ">>> layer_name = " << layer_name << G4endl;
  //    new G4PVPlacement(nullptr, G4ThreeVector(0.,locations[i],0.), two_dim_array_logic, layer_name, mylog, false, i+2);
  //}

  //----------------------------------------------------------------------------------------------------

  G4VPhysicalVolume* myphy = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), mylog, "detector", envLogical, false, 0);

  /*
  // World

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  G4Box* worldS
    = new G4Box("world",                                    //its name
                worldLength/2,worldLength/2,worldLength/2); //its size
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,   //its solid
                 air,      //its material
                 "World"); //its name

  //  Must place the World Physical volume unrotated at (0,0,0).
  //
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,               // no rotation
                 G4ThreeVector(), // at (0,0,0)
                 worldLV,         // its logical volume
                 "World",         // its name
                 0,               // its mother  volume
                 false,           // no boolean operations
                 0,               // copy number
                 fCheckOverlaps); // checking overlaps

  // Target

  G4ThreeVector positionTarget = G4ThreeVector(0,0,-(targetLength+trackerSize));

  G4Tubs* targetS
    = new G4Tubs("target",0.,targetRadius,targetLength,0.*deg,360.*deg);
  fLogicTarget
    = new G4LogicalVolume(targetS, fTargetMaterial,"Target",0,0,0);
  new G4PVPlacement(0,               // no rotation
                    positionTarget,  // at (x,y,z)
                    fLogicTarget,    // its logical volume
                    "Target",        // its name
                    worldLV,         // its mother volume
                    false,           // no boolean operations
                    0,               // copy number
                    fCheckOverlaps); // checking overlaps

  G4cout << "Target is " << 2*targetLength/cm << " cm of "
         << fTargetMaterial->GetName() << G4endl;

  // Tracker

  G4ThreeVector positionTracker = G4ThreeVector(0,0,0);

  G4Tubs* trackerS
    = new G4Tubs("tracker",0,trackerSize,trackerSize, 0.*deg, 360.*deg);
  G4LogicalVolume* trackerLV
    = new G4LogicalVolume(trackerS, air, "Tracker",0,0,0);
  new G4PVPlacement(0,               // no rotation
                    positionTracker, // at (x,y,z)
                    trackerLV,       // its logical volume
                    "Tracker",       // its name
                    worldLV,         // its mother  volume
                    false,           // no boolean operations
                    0,               // copy number
                    fCheckOverlaps); // checking overlaps

  // Visualization attributes

  G4VisAttributes* boxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));

  worldLV      ->SetVisAttributes(boxVisAtt);
  fLogicTarget ->SetVisAttributes(boxVisAtt);
  trackerLV    ->SetVisAttributes(boxVisAtt);

  // Tracker segments

  G4cout << "There are " << fNbOfChambers << " chambers in the tracker region. "
         << G4endl
         << "The chambers are " << chamberWidth/cm << " cm of "
         << fChamberMaterial->GetName() << G4endl
         << "The distance between chamber is " << chamberSpacing/cm << " cm"
         << G4endl;

  G4double firstPosition = -trackerSize + chamberSpacing;
  G4double firstLength   = trackerLength/10;
  G4double lastLength    = trackerLength;

  G4double halfWidth = 0.5*chamberWidth;
  G4double rmaxFirst = 0.5 * firstLength;

  G4double rmaxIncr = 0.0;
  if( fNbOfChambers > 0 ){
    rmaxIncr =  0.5 * (lastLength-firstLength)/(fNbOfChambers-1);
    if (chamberSpacing  < chamberWidth) {
       G4Exception("DetectorConstruction::DefineVolumes()",
                   "InvalidSetup", FatalException,
                   "Width>Spacing");
    }
  }

  for (G4int copyNo=0; copyNo<fNbOfChambers; copyNo++) {

      G4double Zposition = firstPosition + copyNo * chamberSpacing;
      G4double rmax =  rmaxFirst + copyNo * rmaxIncr;

      G4Tubs* chamberS
        = new G4Tubs("Chamber_solid", 0, rmax, halfWidth, 0.*deg, 360.*deg);

      fLogicChamber[copyNo] =
              new G4LogicalVolume(chamberS,fChamberMaterial,"Chamber_LV",0,0,0);

      fLogicChamber[copyNo]->SetVisAttributes(chamberVisAtt);

      new G4PVPlacement(0,                            // no rotation
                        G4ThreeVector(0,0,Zposition), // at (x,y,z)
                        fLogicChamber[copyNo],        // its logical volume
                        "Chamber_PV",                 // its name
                        trackerLV,                    // its mother  volume
                        false,                        // no boolean operations
                        copyNo,                       // copy number
                        fCheckOverlaps);              // checking overlaps

  }

  // Example of User Limits
  //
  // Below is an example of how to set tracking constraints in a given
  // logical volume
  //
  // Sets a max step length in the tracker region, with G4StepLimiter

  G4double maxStep = 0.5*chamberWidth;
  fStepLimit = new G4UserLimits(maxStep);
  trackerLV->SetUserLimits(fStepLimit);

  /// Set additional contraints on the track, with G4UserSpecialCuts
  ///
  /// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
  /// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
  ///                                           maxLength,
  ///                                           maxTime,
  ///                                           minEkin));
  */

  // Always return the physical world

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  G4String trackerChamberSDname = "/TrackerChamberSD";
  TrackerSD* aTrackerSD = new TrackerSD(trackerChamberSDname,
                                            "TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
  // Setting aTrackerSD to all logical volumes with the same name
  // of "Chamber_LV".
  //SetSensitiveDetector("Chamber_LV", aTrackerSD, true);

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector(0., 0., 2.*tesla);
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTargetMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial =
              nistManager->FindOrBuildMaterial(materialName);

  if (fTargetMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fTargetMaterial = pttoMaterial;
        if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
        G4cout
          << G4endl
          << "----> The target is made of " << materialName << G4endl;
     } else {
        G4cout
          << G4endl
          << "-->  WARNING from SetTargetMaterial : "
          << materialName << " not found" << G4endl;
     }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetChamberMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial =
              nistManager->FindOrBuildMaterial(materialName);

  if (fChamberMaterial != pttoMaterial) {
     if ( pttoMaterial ) {
        fChamberMaterial = pttoMaterial;
        for (G4int copyNo=0; copyNo<fNbOfChambers; copyNo++) {
            if (fLogicChamber[copyNo]) fLogicChamber[copyNo]->
                                               SetMaterial(fChamberMaterial);
        }
        G4cout
          << G4endl
          << "----> The chambers are made of " << materialName << G4endl;
     } else {
        G4cout
          << G4endl
          << "-->  WARNING from SetChamberMaterial : "
          << materialName << " not found" << G4endl;
     }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
  fCheckOverlaps = checkOverlaps;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
