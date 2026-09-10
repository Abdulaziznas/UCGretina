#include "globals.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4UImanager.hh"

#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "Incoming_Beam.hh"
#include "Incoming_Beam_Messenger.hh"
#include "Outgoing_Beam.hh"
#include "Outgoing_Beam_Messenger.hh"

#ifdef G4VIS_USE
#include "VisManager.hh"
#endif

#include "Git_Hash.hh"

#include "G4Timer.hh"
G4Timer Timer;
G4Timer Timerintern;

int main(int argc,char** argv) 
{
  
  const G4bool visualize = argc > 1 && G4String(argv[1]) == "--vis";
  if ((visualize && argc != 3) || (!visualize && argc > 2) ||
      (argc == 2 && G4String(argv[1]) == "--help")) {
    G4cout << "Usage: " << argv[0] << " [macro.mac | --vis visualization.mac]\n"
           << "No arguments: terminal session. --vis: Qt GUI with a startup macro."
           << G4endl;
    return argc == 2 && G4String(argv[1]) == "--help" ? 0 : 1;
  }
#ifndef G4VIS_USE
  if (visualize) {
    G4cerr << "Visualization is disabled; rebuild with -DWITH_VIS=ON." << G4endl;
    return 1;
  }
#endif
  // Save arguments before Qt can consume its own command-line options.
  const G4String macroFile = visualize ? argv[2] : (argc == 2 ? argv[1] : "");
  G4UIExecutive* session = nullptr;
  if (argc == 1 || visualize) {
    session = new G4UIExecutive(argc, argv, visualize ? "qt" : "csh");
    if (visualize && !session->IsGUI()) {
      G4cerr << "The --vis option requires a Geant4 installation with Qt support."
             << G4endl;
      delete session;
      return 1;
    }
  }

  // Construct the default run manager
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  G4cout << "Executable: " << argv[0] << G4endl;
  G4cout << "Git commit: " << GIT_HASH << G4endl;
  G4cout << "Git branch: " << GIT_BRANCH << G4endl;
  
  G4cout << "Instantiating DetectorConstruction ..." << G4endl;
  // set mandatory initialization classes
  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  PhysicsList *physicsList = new PhysicsList(detector);
  runManager->SetUserInitialization(physicsList);

  G4cout << "... Done" << G4endl;

  // Construct incoming and outgoing beams
  Incoming_Beam* BeamIn = new Incoming_Beam();
  Incoming_Beam_Messenger* IncomingBeamMessenger = new Incoming_Beam_Messenger(BeamIn);

  Outgoing_Beam* BeamOut = new Outgoing_Beam();
  BeamOut->defaultIncomingIon(BeamIn);
  physicsList->SetOutgoingBeam(BeamOut);
  Outgoing_Beam_Messenger* OutgoingBeamMessenger = new Outgoing_Beam_Messenger(BeamOut);

  runManager->SetUserInitialization(new ActionInitialization(detector, BeamIn, BeamOut, /*enableStepping=*/true));


#ifdef G4VIS_USE
  // visualization manager
  G4VisManager* visManager=0;
#endif

  if (session)   // Initialize visualization for terminal and GUI sessions.
    {

#ifdef G4VIS_USE
      // visualization manager
      G4cout << "Starting visualization...";
      visManager = new VisManager; 
      visManager->Initialize();
      G4cout << "Done!" << G4endl;
#endif


    }

  // Initialize G4 kernel
  // G4cout << "*** Initializing runManager" << G4endl;
  // //  runManager->SetVerboseLevel(2);
  // runManager->Initialize();
  // G4cout << "*** Initialized runManager" << G4endl;

  // get the pointer to the UI manager and set verbosities
  G4UImanager* UI = G4UImanager::GetUIpointer();

  G4int commandStatus = 0;
  if (!macroFile.empty()) {
    commandStatus = UI->ApplyCommand("/control/execute " + macroFile);
  }
  if (session) {
    if (commandStatus == 0) session->SessionStart();
    delete session;
  }

#ifdef G4VIS_USE
  delete visManager;
#endif

  delete runManager;

  delete BeamIn;

  delete IncomingBeamMessenger;

  delete BeamOut;

  delete OutgoingBeamMessenger;

  return commandStatus == 0 ? 0 : 1;
}
