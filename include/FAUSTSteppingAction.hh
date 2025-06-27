#ifndef AnnihilationPhotonsSteppingAction_hh
#define AnnihilationPhotonsSteppingAction_hh

#include "FAUSTVSteppingAction.hh"

#include "G4ThreeVector.hh"
#include "FAUSTSteppingAction.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4AnalysisManager.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"

#include "myGlobalVariables.hh"

class FAUSTSteppingAction: public FAUSTVSteppingAction
{
public:
  FAUSTSteppingAction();
  virtual void BeginOfEventAction();
  virtual void UserSteppingAction(const G4Step*);
  virtual void EndOfEventAction();

private:

  // These are used to remember quantities from call to call of
  // UserSteppingAction
  G4int fTopID, fBottomID, fPanelID, fEventID;
  G4double fEdepCsI[22], fEdepGRETINA, fEdepDeadLayer, fVertexTheta, fVertexX, fVertexY, fVertexZ, fVertexR;
  G4double fEdepPixel[22][128][128]; 
};

#endif
