
#include "YourSteppingAction.hh"

#include "G4Step.hh"

//YourSteppingAction::YourSteppingAction(const G4VPhysicalVolume* vol)                                     
YourSteppingAction::YourSteppingAction(G4VPhysicalVolume* vol)                                     
:   G4UserSteppingAction()
{
//  3. We need to obtain and remember the Target Volume - in this constructor?
    fTargetVol = vol;
    if(vol != nullptr)
        std::cout << ">>>>> YourSteppingAction::YourSteppingAction fTargetVol = " << fTargetVol->GetName() << std::endl;
    else
        std::cout << ">>>>> YourSteppingAction::YourSteppingAction fTargetVol is a nullptr" << std::endl;
}

YourSteppingAction::~YourSteppingAction() {}

// Goals
//  1. Score energy deposition in the 'Target' volume:
//     - collect energy deposit to the mean (per-event) energy deposit computation
//  2. 'Score' the total length of steps of charged particles

void YourSteppingAction::UserSteppingAction(const G4Step* theStep) 
{
    const G4Track* theTrack = theStep->GetTrack();

    // 1. Get the energy deposit
    G4double edep = theStep->GetTotalEnergyDeposit();

    // 3. Check whether step was done inside the Target
    G4VPhysicalVolume* current_volume = theTrack->GetVolume();
    assert( fTargetVol != nullptr );   // A check 

    if(current_volume == fTargetVol)
    {
        fSumEnergyDeposit += edep;
    }

    // 5. Find the length of the current step 
    G4double step_length = theStep->GetStepLength();

    // 6. Check whether the particle is charged
    const G4ParticleDefinition* pDef = theTrack->GetParticleDefinition();
    G4double charge = pDef->GetPDGCharge();

    // 7. Sum the length of charged steps - everywhere?
    if( charge != 0.0 && current_volume == fTargetVol)
    {
        fSumChargedSteps += step_length;
    }
    //  add current energy deposit to the charged particle track length per-event

}

G4double YourSteppingAction::GetSumEnergyDeposit() const
{
    return fSumEnergyDeposit;
}

G4double YourSteppingAction::GetSumChargedSteps() const
{
    return fSumChargedSteps;
}

void      YourSteppingAction::ResetSums()
{
    fSumEnergyDeposit= 0.0;
    fSumChargedSteps = 0.0;
}
