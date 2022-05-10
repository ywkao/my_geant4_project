
// Include global-definitions
#include "globals.hh"

// Simple RunManager
#include "G4RunManager.hh"

// Your detector construction
#include "YourDetectorConstruction.hh"
#include "YourActionInitialization.hh"

// To be able to use and combine the reference physics lists
#include "G4PhysListFactory.hh"

// For the UI manager (to execute the G4 macro input file)
#include "G4UImanager.hh"

// For VIS and/or UI-session
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

int main(int argc, char** argv)
{
    // Setup for interactive session only
    //
    G4UIExecutive* ui  =  new G4UIExecutive(argc, argv);

    // Construct a run manager (Sequential)
    //
    G4RunManager* runManager = new G4RunManager();       

    // Set (MANDATORY) User initialization classes: 
    //  = 1. G4VUserDetectorConstruction
    //  = 2. G4VUserPhysicsList
    //  = 3. G4VUserActionInitialization (that contains G4VUserPrimaryGeneratorAction)
    //
    // 1. DetectorConstruction = YourDetectorConstruction <- G4VUserDetectorConstruction
    G4VUserDetectorConstruction* detector = new YourDetectorConstruction;
    runManager->SetUserInitialization( detector );
    //
    // 2. PhysicsList = G4VModularPhysicsList <- G4VUserPhysicsList
    //    (use reference physics list through the G4PhysListFactory)
    const G4String plName = "FTFP_BERT_EMZ";
    G4PhysListFactory plFactory;
    G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName ); 
    runManager->SetUserInitialization( pl );
    // 
    // 3. ActionInitialization = YourActionInitialization <- G4VUserActionInitialization 
    //    (and YourActionInitialization contains YourPrimaryGeneratorAction which is 
    //     <- G4VUserPrimaryGeneratorActions)
    //    Note, that: 
    //     - all other UserActions (Run, Event, Stepping, Tracking, Stacking) are optional !
    //     - the DetectorConstruction pointer is propagated down to the UserActions
    //       because some actions (e.g. PrimaryGenerator) needs information from the detector.
    runManager->SetUserInitialization( new YourActionInitialization() );  

    // Add visualization: 
    // - create a G4VisExecutive object as the Visualization Manager
    // - initialize the Visualization Manager (will handle all vis components)
    //
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    // - interactive mode: create ui-session
    //
    G4UImanager* UImanager = G4UImanager::GetUIpointer(); 

    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();

    // Here we are after the ui-session termination!
    delete ui;
    
    // Delete the Visualization Manager 
    delete visManager;

    // At the end, delete the Run Manager
    delete runManager;

    return 0;
}
