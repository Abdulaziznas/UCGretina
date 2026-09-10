#ifndef VisManager_h
#define VisManager_h

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
// Register only drivers supported by the installed Geant4 build.
using VisManager = G4VisExecutive;
#endif

#endif
