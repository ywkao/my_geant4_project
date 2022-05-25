#ifndef YOURACTIONINITIALIZATION_HH
#define YOURACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

// forward declarations


class YourActionInitialization : public G4VUserActionInitialization {

  // Method declaration:
  public:
    
    YourActionInitialization();
    //  HERE: Add arguments: anything needed to initialize data members
    //  CHANGE - We need the pointer to YourDetectorConstruction
   
    virtual ~YourActionInitialization(); 

    // (Pure) Virtual method -- must be implemented
    //   It's task is to create all user action classes
    virtual void Build() const;

    // Advanced method for multi-threaded applications 
    virtual void BuildForMaster() const;


  // Data member declarations:
  private:
      // New data members HERE
      //   We will need the pointer to YourDetectorConstruction   
};

#endif
