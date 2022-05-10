
#include "YourDetectorConstruction.hh"

// for geometry definitions 
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4RotationMatrix.hh"

// for material definitions
#include "G4Material.hh"
#include "G4NistManager.hh"

// for having units and constants
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


// Constructor
//
YourDetectorConstruction::YourDetectorConstruction()
  : G4VUserDetectorConstruction()
{
}

// Destructor
//
YourDetectorConstruction::~YourDetectorConstruction()
{
}

// The detector is a simple slab with a thickness along the x-direction.
//
G4VPhysicalVolume* YourDetectorConstruction::Construct()
{
    // I. CREATE MATERIALS:
    // 1. Material for the world: low density hydrogen defined by "hand"
    G4double zet      = 1.0;
    G4double amass    = 1.01*g/mole;
    G4double density  = universe_mean_density;
    G4double pressure = 3.e-18*pascal;
    G4double tempture = 2.73*kelvin;
    G4Material* materialWorld
           = new G4Material("Galactic", zet, amass, density,
                            kStateGas, tempture, pressure);
    // 2. Material for the target: set to be Silicon
    G4Material* targetMaterial
           = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    // II. CREATE GEOMETRY:

    // a. Create the world (a box), defining first its size:
    G4double worldXSize   = 1.1*cm;
    G4double worldYZSize  = 1.25*worldXSize;
    G4Box* worldSolid
           = new G4Box("solid-World",    // name
    	               0.5*worldXSize,   // half x-size
    	               0.5*worldYZSize,  // half y-size 
    	               0.5*worldYZSize); // half z-size
    G4LogicalVolume* worldLogical
           = new G4LogicalVolume(worldSolid,     // solid
                                 materialWorld,  // material
                                 "logic-World"); // name
    G4VPhysicalVolume* worldPhysical
           = new G4PVPlacement(nullptr,                 // (no) rotation
                               G4ThreeVector(0.,0.,0.), // translation
                               worldLogical,            // its logical volume
                               "World",                 // its name
                               nullptr,                 // its mother volume
                               false,                   // not used
                               0);                      // copy number

    // ++++++++++++++++++++++++++ YOUR CODE FROM HERE ++++++++++++++++++++++++
    G4Box *mybox = new G4Box("mybox", 1.*mm, 2.*mm, 2.*mm);
    G4LogicalVolume *mylog = new G4LogicalVolume(mybox, materialWorld, "logic-Target");
    G4VPhysicalVolume *myphy = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.), mylog, "myphy", worldLogical, false, 1);
    fTargetPhysical = myphy;

    // translation
    G4ThreeVector myVec(-2.*mm, 0., 0.);

    // u, v, w are the daughter axes, projected on the mother frame
    G4double phi = 30*deg;
    G4ThreeVector u = G4ThreeVector(0, 0, -1);
    G4ThreeVector v = G4ThreeVector(-std::sin(phi), std::cos(phi),0.);
    G4ThreeVector w = G4ThreeVector( std::cos(phi), std::sin(phi),0.);
    G4RotationMatrix *myRotation  = new G4RotationMatrix(u, v, w);
    G4cout << "\n --> phi = " << phi/deg << " deg;  direct rotation matrix : ";
    myRotation->print(G4cout);

    G4VSolid *pTubSolid = new G4Tubs("aTubSolid", 0.*mm, 1.*mm, 0.5*mm, 0., 6.28);
    G4LogicalVolume *pTubLog = new G4LogicalVolume(pTubSolid, materialWorld, "aTubLog");
    G4VPhysicalVolume *aTubPhys = new G4PVPlacement(myRotation, myVec, pTubLog, "aTubPhys", worldLogical, 0, 2);

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // III. RETURN THE World PHYSICAL-VOLUME POINTER:
    return worldPhysical;
}
