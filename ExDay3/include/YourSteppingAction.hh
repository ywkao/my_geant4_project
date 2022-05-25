#ifndef YOURSTEPPINGACTION_HH
#define YOURSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"

// forward declarations
class G4VPhysicalVolume;

class YourSteppingAction : public G4UserSteppingAction {

  // Method declaration:
  public:

    // --- Mandatory methods
    YourSteppingAction();   //  CHANGE this --> Add 'target' volume as argument !
    virtual ~YourSteppingAction();

    // Virtual method called by the kernel after each step
    virtual void UserSteppingAction(const G4Step* step);
    // It will accumulate the quantities

    // -----------------------

    // --- Our own methods
    //
    //  The event action must collect the cumulative values at the end of an event
    double GetSumEnergyDeposit() const;
    double GetSumChargedSteps() const;
    void     ResetSums();

    // void   SetTargetVolume(G4VPhysicalVolume* vol);
    // { fTargetVol = vol;}

  // Data member declarations:
  private:
    G4VPhysicalVolume* fTargetVol = nullptr;
    //   Keep the target volume

    double  fSumEnergyDeposit = 0.0;
    double  fSumChargedSteps  = 0.0;
    //  Total energy deposit - in target volume   
};

#endif
