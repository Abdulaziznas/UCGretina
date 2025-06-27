#ifndef FAUST_S3rings_Parameterisation_H
#define FAUST_S3rings_Parameterisation_H 1


#include "G4VPVParameterisation.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"




class FAUST_S3rings_Parameterisation : public G4VPVParameterisation
{
  public:
    FAUST_S3rings_Parameterisation(G4int NoCChambers, G4double inner_radius_StartZ , G4double outter_radius_EndZ, G4double ring_gap, G4bool SiLayerCheck);
     ~FAUST_S3rings_Parameterisation() ;
    void ComputeTransformation (G4int copyNo, G4VPhysicalVolume* physVol) const;
    void ComputeDimensions (G4Tubs& trackerChamber, G4int copyNo, const G4VPhysicalVolume* physVol) const;
  private:
    G4double fRingWidth; G4int fNoCChambers; G4double fInner_radius_StartZ ; G4double fOuter_radius_EndZ; G4double fRing_gap; G4bool fSiLayer;
};


#endif
