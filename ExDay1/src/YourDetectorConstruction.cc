
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
    G4double yt = 0.5*mm; // unit in y coordinate, cm
    std::vector<G4double> locations = { 5*yt, 15*yt, 25*yt, 35*yt, 45*yt, 55*yt, 65*yt, 75*yt, 85*yt, 100*yt };

    //------------------------------
    // I. CREATE MATERIALS:
    //------------------------------
    // 0. pixel info & world dimension
    G4int n_pixels = 10; // 100
    G4double pixel_unit   = micrometer;
    G4double default_pixel_length = 200*pixel_unit; // z direction
    G4double default_pixel_width  = 100*pixel_unit; // x direction
    G4double default_pixel_thick  = 300*pixel_unit; // y direction

    G4double worldXSize  = 10*cm; // 200*cm
    G4double worldYZSize = 1.25*worldXSize;
    G4double occupied_fraction = 0.96;

    // 1. Material for the world: low density hydrogen defined by "hand"
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
    G4Box* worldSolid = new G4Box("solid-World", 0.5*worldXSize, 0.5*worldYZSize, 0.5*worldYZSize);
    G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, materialWorld, "logic-World");
    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), worldLogical, "World", nullptr, false, 0);

    // env
    G4double envXSize  = occupied_fraction*worldXSize;
    G4double envYZSize = occupied_fraction*worldYZSize;
    G4Box* envSolid = new G4Box("env-box", 0.5*envXSize, 0.5*envYZSize, 0.5*envYZSize);
    G4LogicalVolume* envLogical = new G4LogicalVolume(envSolid, materialWorld, "logic-env");
    G4VPhysicalVolume *envPhysical = new G4PVPlacement(nullptr, G4ThreeVector(0.,0.,0.), envLogical, "Env", worldLogical, false, 1);

    // detector
    G4double detXSize  = occupied_fraction*envXSize;
    G4double detYZSize = occupied_fraction*envYZSize;
    G4Box *mybox = new G4Box("mybox", 0.5*detXSize, 0.5*detYZSize, 0.5*detYZSize);
    G4LogicalVolume *mylog = new G4LogicalVolume(mybox, materialWorld, "logic-Target");

    //----------------------------------------------------------------------------------------------------

    for(unsigned int i=0; i<locations.size(); ++i)
    {
        G4String idx = std::to_string(i+1);
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

        // pixel
        name_obj = "pixel" + tag;
        name_log = "logic-pixel" + tag;
        G4Box *pixel = new G4Box(name_obj, 0.5*pixel_width, 0.5*pixel_thick, 0.5*pixel_length);
        G4LogicalVolume *pixel_logic = new G4LogicalVolume(pixel, targetMaterial, name_log);

        // 1D array
        name_obj = "box_one_dim_array" + tag;
        name_log = "logic-one_dim_array" + tag;
        name_vol = "one_dim_array" + tag;
        G4double one_dim_array_size = occupied_fraction*envYZSize;
        G4double one_dim_array_thick = pixel_thick;
        G4Box *one_dim_array = new G4Box(name_obj, 0.5*one_dim_array_thick, 0.5*one_dim_array_thick, 0.5*one_dim_array_size);
        G4LogicalVolume *one_dim_array_logic = new G4LogicalVolume(one_dim_array, materialWorld, name_log);
        new G4PVReplica(name_vol, pixel_logic, one_dim_array_logic, kZAxis, n_pixels, space_z);

        // 2D array
        name_obj = "box_two_dim_array" + tag;
        name_log = "logic-two_dim_array" + tag;
        name_vol = "two_dim_array" + tag;
        G4double two_dim_array_size = occupied_fraction*envYZSize;
        G4double two_dim_array_thick = pixel_thick;
        G4Box *two_dim_array = new G4Box(name_obj, 0.5*two_dim_array_size, 0.5*two_dim_array_thick, 0.5*two_dim_array_size);
        G4LogicalVolume *two_dim_array_logic = new G4LogicalVolume(two_dim_array, materialWorld, name_log);
        new G4PVReplica(name_vol, one_dim_array_logic, two_dim_array_logic, kXAxis, n_pixels, space_x);

        // place element in detector
        G4String layer_name = "layer" + idx;
        G4cout << ">>> layer_name = " << layer_name << G4endl;
        new G4PVPlacement(nullptr, G4ThreeVector(0.,locations[i],0.), two_dim_array_logic, layer_name, mylog, false, i+2);
    }

    //----------------------------------------------------------------------------------------------------

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
