
#include "YourPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4Event.hh" 


YourPrimaryGeneratorAction::YourPrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction()
{
   // create the particle-gun object
   G4int nParticle = 1;
   fParticleGun    = new G4ParticleGun(nParticle);
}


YourPrimaryGeneratorAction::~YourPrimaryGeneratorAction()
{
   delete fParticleGun;
}


void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
   fParticleGun->GeneratePrimaryVertex(evt);
}
