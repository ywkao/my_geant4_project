
#include "YourEventAction.hh"
#include "YourSteppingAction.hh"
// Because we chose to use YourSteppingAction's methods

#include "G4Event.hh"


YourEventAction::YourEventAction( YourSteppingAction* stepAction ) 
: G4UserEventAction(),
  fSteppingAction(stepAction)
{}
//  CHANGE this 
//    We need to remember the stepping action:
//    Add an argument 'YourSteppingAction*' and store it in fSteppingAction


YourEventAction::~YourEventAction() {}

// Before each event: reset per-event variables 
void YourEventAction::BeginOfEventAction(const G4Event* /*anEvent*/) {
   if( fSteppingAction ) {
     // fSteppingAction->SetTarget(det->GetTargetVolume()); 
     // Ensure that the target is correctly set

     fSteppingAction->ResetSums();
      // Don't accumulate between events !
   } else { 
     G4cerr << "ERROR in YourEventAction> There is no stepping action" << G4endl;
   }
}


// After each event:
//   print the energy deposited in target and the length of charged particle steps
void YourEventAction::EndOfEventAction(const G4Event* anEvent) {
    G4double eDep = fSteppingAction->GetSumEnergyDeposit();
    G4double len  = fSteppingAction->GetSumChargedSteps();
    G4cout << " Event " << anEvent->GetEventID() << "  energy deposit = " << eDep 
           << "  sum charged step len = " << len
           << G4endl;
}

