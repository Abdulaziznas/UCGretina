#ifndef FAUSTDetectorConstruction_hh
#define FAUSTDetectorConstruction_hh 1

#include "G4VUserDetectorConstruction.hh"
#include "G4GenericMessenger.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"

#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4ExtrudedSolid.hh"


#include "FAUSTmyGlobalVariables.hh"

#include "FAUSTTrapPixel.hh"
#include "FAUSTS1annaularD.hh"
#include "FAUST_S3rings_Parameterisation.hh"
#include "FAUSTPixelToCoordinates.hh"
#include "G4UserLimits.hh"

#include "FAUSTVertical_strip_sol.hh"

#include "G4VPVParameterisation.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4ReplicatedSlice.hh"









class G4VPhysicalVolume;
class G4LogicalVolume;
class G4UserLimits;

class FAUSTDetectorConstruction
{
public:
  FAUSTDetectorConstruction();
  virtual ~FAUSTDetectorConstruction();

  virtual G4VPhysicalVolume* Construct(G4LogicalVolume*);


private:
void FAUSTMaterialDefinition();

G4GenericMessenger *fMessenger;
G4UserLimits* fStepLimit;


G4Material *fAir, *fVacuum, *fGermanium, *fSilicon_Mat, *fCsI_Mat, *fStainlessSteel_Mat, *fAluminum_Mat, *fCarbonFiber_Mat, *fMylar_Mat, *fDeuteratedPlastic_Mat;
G4Element *fSilicon, *fCaesium, *fIodine,*fDeuterium;
G4Isotope *fDeuterium_Isotope;

G4double fStrip_gap ,fRings_gap, fStrip_thickness, fCsI_thickness , fS3_thickness, fChamber_gap, fS3_target_dist, fLastHStripMagConstant; 
G4int fVstripNo ,fHstripNo, fS3ringNo;
G4String fAnnular_det, fOutFileName;




};













#endif

