
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
#include "G4PVReplica.hh"


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
    //------------------------------
    // I. CREATE MATERIALS:
    //------------------------------
    // 1. Material for the world: low density hydrogen defined by "hand"
    G4int n_pixels = 10; //100
    G4double pixel_size = 0.1*mm;
    G4double pixel_thick = 0.05*mm;
    G4double space = 1.01*pixel_size;

    G4double zet      = 1.0;
    G4double amass    = 1.01*g/mole;
    G4double density  = universe_mean_density;
    G4double pressure = 3.e-18*pascal;
    G4double tempture = 2.73*kelvin;
    G4Material* materialWorld = new G4Material("Galactic", zet, amass, density, kStateGas, tempture, pressure);

    // 2. Material for the target: set to be Silicon
    G4Material* targetMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");

    //------------------------------
    // II. CREATE GEOMETRY:
    //------------------------------
    // world
    G4double worldXSize   = 11*cm;
    G4double worldYZSize  = 1.25*worldXSize;
    G4Box* worldSolid = new G4Box("solid-World", 0.5*worldXSize, 0.5*worldYZSize, 0.5*worldYZSize);
    G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, materialWorld, "logic-World");
    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), worldLogical, "World", nullptr, false, 0);

    // env
    G4double envXSize = 0.48*worldXSize;
    G4double envYZSize = 0.48*worldYZSize;
    G4Box* envSolid = new G4Box("env-box", envXSize, envYZSize, envYZSize);
    G4LogicalVolume* envLogical = new G4LogicalVolume(envSolid, materialWorld, "logic-env");
    G4VPhysicalVolume *envPhysical = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), envLogical, "Env", worldLogical, false, 1);

    // pixel
    G4Box *pixel = new G4Box("pixel", pixel_thick/2, pixel_size/2, pixel_size/2);
    G4LogicalVolume *pixel_logic = new G4LogicalVolume(pixel, targetMaterial, "logic-pixel");

    // 1D array
    G4double one_dim_array_size = 0.48*envYZSize;
    G4double one_dim_array_thick = pixel_thick;
    G4Box *one_dim_array = new G4Box("box_one_dim_array", one_dim_array_thick/2, one_dim_array_thick/2, one_dim_array_size/2);
    G4LogicalVolume *one_dim_array_logic = new G4LogicalVolume(one_dim_array, materialWorld, "logic-one_dim_array");
    new G4PVReplica("one_dim_array", pixel_logic, one_dim_array_logic, kZAxis, n_pixels, space);

    // 2D array
    G4double two_dim_array_size = 0.48*envYZSize;
    G4double two_dim_array_thick = pixel_thick;
    G4Box *two_dim_array = new G4Box("box_two_dim_array", two_dim_array_thick/2, two_dim_array_size/2, two_dim_array_size/2);
    G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, materialWorld, "logic-two_dim_array");
    new G4PVReplica("two_dim_array", one_dim_array_logic, two_dim_array_logic, kYAxis, n_pixels, space);

    // detector
    G4double detXSize  = 0.48*envXSize;
    G4double detYZSize = 0.48*envYZSize;
    G4Box *mybox = new G4Box("mybox", detXSize/2, detYZSize/2, detYZSize/2);
    G4LogicalVolume *mylog = new G4LogicalVolume(mybox, materialWorld, "logic-Target");
    new G4PVReplica("layers", two_dim_array_logic, mylog, kXAxis, 10, 10*space);

    G4VPhysicalVolume* myphy = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), mylog, "detector", envLogical, false, 0);
    fTargetPhysical = myphy;

    // // translation
    // G4ThreeVector myVec(-2.*mm, 0., 0.);

    // // u, v, w are the daughter axes, projected on the mother frame
    // G4double phi = 30*deg;
    // G4ThreeVector u = G4ThreeVector(0, 0, -1);
    // G4ThreeVector v = G4ThreeVector(-std::sin(phi), std::cos(phi),0.);
    // G4ThreeVector w = G4ThreeVector( std::cos(phi), std::sin(phi),0.);
    // G4RotationMatrix *myRotation  = new G4RotationMatrix(u, v, w);
    // G4cout << "\n --> phi = " << phi/deg << " deg;  direct rotation matrix : ";
    // myRotation->print(G4cout);

    // G4VSolid *pTubSolid = new G4Tubs("aTubSolid", 0.*mm, 1.*mm, 0.5*mm, 0., 6.28);
    // G4LogicalVolume *pTubLog = new G4LogicalVolume(pTubSolid, materialWorld, "aTubLog");
    // G4VPhysicalVolume *aTubPhys = new G4PVPlacement(myRotation, myVec, pTubLog, "aTubPhys", worldLogical, 0, 2);

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // III. RETURN THE World PHYSICAL-VOLUME POINTER:
    return worldPhysical;
}
