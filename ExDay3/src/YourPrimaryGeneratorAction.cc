
#include "YourPrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4Event.hh" 

#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

YourPrimaryGeneratorAction::YourPrimaryGeneratorAction()
  : G4VUserPrimaryGeneratorAction()
{
   // create the particle-gun object
   G4int nParticle = 1;
   fParticleGun    = new G4ParticleGun(nParticle);

   SetDefaultKinematics();
}

YourPrimaryGeneratorAction::~YourPrimaryGeneratorAction()
{
   delete fParticleGun;
}


void YourPrimaryGeneratorAction::SetDefaultKinematics()
{
   // here it is possible to place extra code
   fParticleGun->SetParticleDefinition(
      G4ParticleTable::GetParticleTable()->FindParticle( "e+" ) );

   fParticleGun->SetParticleMomentumDirection( G4ThreeVector(1., 0., 0.) );
   fParticleGun->SetParticlePosition( G4ThreeVector(-0.45*cm, 0., 0.) );
   fParticleGun->SetParticleEnergy( 100*MeV );
   //
   G4cout << "SetDefaultKinematics called.  Set particleGun's particle type, energy, position and momentum direction: " << G4endl;
   G4cout << "     particle = " << fParticleGun->GetParticleDefinition()->GetParticleName() << G4endl;
   //  Look at the file $G4INCLUDE/G4ParticleDefinition.hh to find G4ParticleDefinition's other methods
   G4cout << "     position = " << fParticleGun->GetParticlePosition() << G4endl;
   G4cout << "     direction= " << fParticleGun->GetParticleMomentumDirection() << G4endl;
}

void YourPrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
   fParticleGun->GeneratePrimaryVertex(evt);
}
