
#ifndef YOURPRIMARYGENERATORACTION_HH
#define YOURPRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"

#include "globals.hh"

// forward declarations
class G4ParticleGun;
class G4Event;
class G4String;

class YourPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:  // Method declarations
    
    // Constructor & Destructor
    YourPrimaryGeneratorAction();
    virtual ~YourPrimaryGeneratorAction();

    // (Pure) virtual method to generata primary events
    virtual void GeneratePrimaries(G4Event*);

    // Public method to set the default primary particle kinematics
    void SetDefaultKinematics();

  private:  // Data member declarations

     G4ParticleGun* fParticleGun = nullptr;
};

#endif
