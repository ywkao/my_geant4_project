
#include "YourActionInitialization.hh"

#include "YourPrimaryGeneratorAction.hh"

YourActionInitialization::YourActionInitialization()
  : G4VUserActionInitialization()
{
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
// - for sequential mode (will be invoked immediately by G4RunManager 
//   when the ActionInitialization object is registered in the main)
// - for worker threads (will be invoked later by all worker RunManagers) 
void YourActionInitialization::Build() const
{
  // Set UserPrimaryGeneratorAction
  YourPrimaryGeneratorAction* primaryAction = new YourPrimaryGeneratorAction();
  SetUserAction(primaryAction);
}  
