#include "RunAction.hh"

#include "G4Timer.hh"
//#include "G4AnalysisManager.hh"
#include "FAUSTmyGlobalVariables.hh"

extern G4Timer Timer;

RunAction::RunAction(DetectorConstruction* detector, Incoming_Beam* BI,EventAction* ev): myDetector(detector), BeamIn(BI), evaction(ev)
{
//  G4AnalysisManager* mgr = G4AnalysisManager::Instance();
//  
//  mgr->SetVerboseLevel(0);
//  mgr->SetNtupleMerging(true);
//  
//  mgr->CreateNtuple("Ntuple1", "Pixels");
//  mgr->CreateNtupleIColumn("EventID");
//  mgr->CreateNtupleIColumn("PixelID");
//  mgr->CreateNtupleIColumn("PanelID");
//  mgr->CreateNtupleIColumn("TopID");
//  mgr->CreateNtupleIColumn("BottomID");
//  mgr->CreateNtupleDColumn("EdepPixel");
//  mgr->FinishNtuple(0);
//
//  mgr->CreateNtuple("Ntuple2", "CsI");
//  mgr->CreateNtupleIColumn("EventID");
//  mgr->CreateNtupleIColumn("PanelID");
//  mgr->CreateNtupleDColumn("EdepCsI");
//  mgr->FinishNtuple(1);
//
//  mgr->CreateNtuple("Ntuple3", "TotalEdep");
//  mgr->CreateNtupleIColumn("EventID");
//  mgr->CreateNtupleIColumn("PixelID");
//  mgr->CreateNtupleDColumn("EdepTotal");
//  mgr->CreateNtupleDColumn("EdepSi");
//  mgr->CreateNtupleDColumn("EdepCsI"); 
//  mgr->CreateNtupleDColumn("VertexTheta");
//  mgr->CreateNtupleDColumn("VertexX");
//  mgr->CreateNtupleDColumn("VertexY");
//  mgr->CreateNtupleDColumn("VertexZ");
//  mgr->CreateNtupleDColumn("VertexR");
//  mgr->FinishNtuple(2);
}


RunAction::~RunAction()
{

}

void RunAction::BeginOfRunAction(const G4Run* run)
{

  G4cout<<" Beginning of run "<<G4endl;

 // // Get analysis manager
 // G4AnalysisManager* mgr = G4AnalysisManager::Instance();
 // G4String SiThickness = to_string(GlobalVariables::SiThickness);
 // G4String CsIThickness = to_string(GlobalVariables::CsIThickness);
 // // Open an output file
 // G4String fileName = "../simulationData/Data" + GlobalVariables::OutFileName + ".root";
 // mgr->OpenFile(fileName);

  evaction->SetNTotalevents(run->GetNumberOfEventToBeProcessed());
  if(run->GetNumberOfEventToBeProcessed() > 1000000)
    evaction->SetEveryNEvents(10000);
  else if(run->GetNumberOfEventToBeProcessed() > 1000)
    evaction->SetEveryNEvents(1000);
  else if(run->GetNumberOfEventToBeProcessed() > 100)
    evaction->SetEveryNEvents(100);
  else
    evaction->SetEveryNEvents(1);

  G4cout << " Simulating " << run->GetNumberOfEventToBeProcessed()
	 << " events." << G4endl;

  if(BeamIn->getKE()>0)
    evaction->SetInBeam(true);
  
  if(evaction->EvOut())
    G4cout << " Writing ASCII output to " 
	   << evaction->GetOutFileName() << G4endl;
  if(evaction->Mode2Out())
    G4cout << " Writing Mode 2 output to " 
	   << evaction->GetMode2FileName() << G4endl;

  Timer.Start();
}


 
void RunAction::EndOfRunAction(const G4Run*)
{

// G4AnalysisManager* mgr = G4AnalysisManager::Instance();
// mgr->Write();
// mgr->CloseFile();

  if(evaction->EvOut())
    evaction->closeEvfile();
  if(evaction->Mode2Out())
    evaction->closeMode2file();

  Timer.Stop();

  G4cout << "                                                     " << G4endl;

  G4double time, hours, minutes, seconds;

  G4cout << "Real time: ";
  time = Timer.GetRealElapsed();
  hours = floor(time/3600.0);
  if(hours>0){
    G4cout << std::setprecision(0) << std::setw(2) 
	   << hours << ":";
    G4cout << std::setfill('0');
  } else {
    G4cout << std::setfill(' ');
  }
  minutes = floor(fmod(time,3600.0)/60.0);
  if(minutes>0){
    G4cout << std::setprecision(0) << std::setw(2) << minutes << ":";
    G4cout << std::setfill('0');
  } else {
    G4cout << std::setfill(' ');
  }
  seconds = fmod(time,60.0);
  if(seconds>0)
    G4cout << std::setprecision(2) << std::setw(5) << seconds;
  G4cout << std::setfill(' ');

  G4cout << "   System time: ";
  time = Timer.GetSystemElapsed();
  hours = floor(time/3600.0);
  if(hours>0){
    G4cout << std::setprecision(0) << std::setw(2) 
	   << hours << ":";
    G4cout << std::setfill('0');
  } else {
    G4cout << std::setfill(' ');
  }
  minutes = floor(fmod(time,3600.0)/60.0);
  if(minutes>0){
    G4cout << std::setprecision(0) << std::setw(2) << minutes << ":";
    G4cout << std::setfill('0');
  } else {
    G4cout << std::setfill(' ');
  }
  seconds = fmod(time,60.0);
  if(seconds>0)
    G4cout << std::setprecision(2) << std::setw(5) << seconds;
  G4cout << std::setfill(' ');

  G4cout << "   User time: ";
  time = Timer.GetUserElapsed();
  hours = floor(time/3600.0);
  if(hours>0){
    G4cout << std::setprecision(0) << std::setw(2) 
	   << hours << ":";
    G4cout << std::setfill('0');
  } else {
    G4cout << std::setfill(' ');
  }
  minutes = floor(fmod(time,3600.0)/60.0);
  if(minutes>0){
    G4cout << std::setprecision(0) << std::setw(2) << minutes << ":";
    G4cout << std::setfill('0');
  } else {
    G4cout << std::setfill(' ');
  }
  seconds = fmod(time,60.0);
  if(seconds>0)
    G4cout << std::setprecision(2) << std::setw(5) << seconds;
  G4cout << std::setfill(' ');

  G4cout << "   "
	 << evaction->GetNTotalevents()/Timer.GetRealElapsed()
	 << " events/s" << G4endl;
 
}

