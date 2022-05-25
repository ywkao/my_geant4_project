
#include "YourActionInitialization.hh"

#include "YourPrimaryGeneratorAction.hh"

// #include "YourDetectorConstruction.hh"

#include "YourEventAction.hh"
#include "YourSteppingAction.hh"

YourActionInitialization::YourActionInitialization()
  : G4VUserActionInitialization()
{
   // CHANGE this 
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


  YourSteppingAction* stepAction = new YourSteppingAction();  // CHANGE this 
  //   The SteppingAction needs the target volume from the DetectorConstruction !!

  // Set UserSteppingAction
  SetUserAction( stepAction );
  
    // Set UserEventAction
  YourEventAction* eventAction = new YourEventAction(stepAction);
  SetUserAction(eventAction);
}  
