#ifndef YOURACTIONINITIALIZATION_HH
#define YOURACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

// forward declarations
#include "YourDetectorConstruction.hh"


class YourActionInitialization : public G4VUserActionInitialization {

  // Method declaration:
  public:
    
    YourActionInitialization(YourDetectorConstruction* detector);
   
    virtual ~YourActionInitialization(); 

    // (Pure) Virtual method -- must be implemented
    //   It's task is to create all user action classes
    virtual void Build() const;

    // Advanced method for multi-threaded applications 
    virtual void BuildForMaster() const;


  // Data member declarations:
  private:
      YourDetectorConstruction* fDetector = nullptr;
};

#endif
