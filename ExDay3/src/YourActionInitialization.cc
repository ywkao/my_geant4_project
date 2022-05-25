
#include "YourActionInitialization.hh"

#include "YourPrimaryGeneratorAction.hh"

#include "YourDetectorConstruction.hh"

#include "YourEventAction.hh"
#include "YourSteppingAction.hh"

YourActionInitialization::YourActionInitialization(YourDetectorConstruction* detector)
  : G4VUserActionInitialization(),
    fDetector(detector)
{
   // We need to get/keep a pointer to 'YourDetectorConstruction'
}
    

YourActionInitialization::~YourActionInitialization()
{
}


// NOT CALLED IN SEQUENTIAL MODE i.e. ONLY FOR MT: 
// - ONLY FOR creating RunAction for the Master thread
void YourActionInitialization::BuildForMaster() const
{
}


// Create all User Actions here: 
// - it will be invoked immediately by G4RunManager 
//   when the ActionInitialization object is registered in the main
void YourActionInitialization::Build() const
{
  // Set UserPrimaryGeneratorAction
  YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction();
  SetUserAction(primaryAction);

  G4VPhysicalVolume* fTargetPhysical = fDetector->GetTargetPhysicalVolume();

  // the target volume is nullptr... // fixed by runManager->InitializeGeometry() in yourMainApplication.cc
  if(fTargetPhysical != nullptr) std::cout << ">>>>> YourActionInitialization::Build fTargetPhysical = " << std::endl;
  else                           std::cout << ">>>>> YourActionInitialization::Build fTargetPhysical is a nullptr" << std::endl;

  if(fTargetPhysical == nullptr)
    G4cerr << "WARNING: your fTargetPhysical is not (yet) defined!!" << G4endl;

  YourSteppingAction* stepAction = new YourSteppingAction(fTargetPhysical);

  // Set UserSteppingAction
  SetUserAction( stepAction );
  
  // Set UserEventAction
  YourEventAction* eventAction = new YourEventAction(stepAction);
  SetUserAction(eventAction);
}  
