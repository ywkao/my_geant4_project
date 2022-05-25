
// Include global-definitions
#include "globals.hh"

// RunManager includes
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
    // Interactive OR batch mode? Arguments will determine
    G4String macrofile = "";
    G4UIExecutive* ui  = nullptr;
    if ( argc == 1 )
    {
      ui = new G4UIExecutive(argc, argv);
    }
    else
    {
      macrofile = argv[1];
      G4cout << " Reading G4 commands from macro file" << macrofile << " \n";
    }

    // Construct the run manager
    //
    G4RunManager* runManager = new G4RunManager();

    // Set Mandatory User initialization classes: 
    //
    // 1. DetectorConstruction  = YourDetectorConstruction <- G4VUserDetectorConstruction
    //
    //G4VUserDetectorConstruction* detector = new YourDetectorConstruction;
    YourDetectorConstruction* detector = new YourDetectorConstruction;
    runManager->SetUserInitialization( detector );

    runManager->InitializeGeometry(); // important for constructing & passing fTargetVolume
    G4VPhysicalVolume* fTargetPhysical = detector->GetTargetPhysicalVolume();
    if(fTargetPhysical != nullptr) std::cout << ">>>>> YourMainApplication::main fTargetPhysical = " << fTargetPhysical->GetName() << std::endl;
    else                           std::cout << ">>>>> YourMainApplication::main fTargetPhysical is a nullptr" << std::endl;
    
    //
    // 2. PhysicsList
    const G4String plName = "FTFP_BERT_EMZ";
    G4PhysListFactory plFactory;
    G4VModularPhysicsList *pl = plFactory.GetReferencePhysList( plName ); 
    runManager->SetUserInitialization( pl );
    
    // 
    // 3. ActionInitialization 
    runManager->SetUserInitialization( new YourActionInitialization(detector) );
    
    // Visualization: 
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    //
    G4UImanager* UImanager = G4UImanager::GetUIpointer(); 
    if ( !ui )
    {
      // batch mode - just executes commands in the maro file
      G4String command  = "/control/execute ";
      UImanager->ApplyCommand( command + macrofile );
    }
    else
    {
      // interactive mode: create ui-session       
      // 
      UImanager->ApplyCommand("/control/execute vis.mac");
      ui->SessionStart();
      // Here we are after the ui-session termination!
      delete ui;
    } 
    
    // delete the Visualization Manger 
    delete visManager;
    
    // At the end delete the RunManager
    delete runManager;

    return 0;
}
