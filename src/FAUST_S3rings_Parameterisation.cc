#include "FAUST_S3rings_Parameterisation.hh"

FAUST_S3rings_Parameterisation::FAUST_S3rings_Parameterisation(G4int NoCChambers, G4double inner_radius_StartZ , G4double outer_radius_EndZ, G4double ring_gap, G4bool SiLayerCheck) : G4VPVParameterisation(),
 fNoCChambers(NoCChambers),
 fInner_radius_StartZ(inner_radius_StartZ),
 fOuter_radius_EndZ(outer_radius_EndZ),
 fRing_gap(ring_gap),
 fSiLayer(SiLayerCheck)
{ 
    fRingWidth = (outer_radius_EndZ - inner_radius_StartZ - (fRing_gap*(fNoCChambers - 1)))/(fNoCChambers);
}

FAUST_S3rings_Parameterisation::~FAUST_S3rings_Parameterisation()
{}


 void FAUST_S3rings_Parameterisation::ComputeTransformation(G4int copyNo, G4VPhysicalVolume* physVol) const
 {

    //physVol->SetCopyNo(0);

 }

void FAUST_S3rings_Parameterisation::ComputeDimensions(G4Tubs& trackerChamber, G4int copyNo, const G4VPhysicalVolume* physVol) const
{

    if (fSiLayer == true)
    {
    G4int pixelID = 0;
    G4int TopID = 0; 
    G4int BottomID = 0;
    for (G4int i = 0; i < 24; i++)
    {
        for (G4int j = 0; j < 16; j++)
        {
            if (pixelID == copyNo)
            {
                TopID = i;
                BottomID = j;
            }
           pixelID++; 
        }
    }
    // G4cout << "TopID "<< TopID << G4endl;
    G4double InnerRadius = fInner_radius_StartZ + TopID * (fRing_gap + fRingWidth) ;
    G4double OuterRadius = InnerRadius + fRingWidth;
    trackerChamber.SetInnerRadius(InnerRadius);
    trackerChamber.SetOuterRadius(OuterRadius);


    G4double RingGapDphi = (fRing_gap/fInner_radius_StartZ); // the gap in radian  
    G4double pDphi = (M_PI/8.) - RingGapDphi ; // the width of a panel in radian 
    G4double PSPhi = (0.5 * RingGapDphi) + (BottomID *(RingGapDphi + pDphi)); // the radius of a ring given by the copy number
    //G4double PixelCoordinatePhi =  PSPhi + pDphi/2; 

    trackerChamber.SetStartPhiAngle(PSPhi);
    trackerChamber.SetDeltaPhiAngle(pDphi);
    }
    else
    {
        G4double InnerRadius = fInner_radius_StartZ + copyNo * (fRing_gap + fRingWidth) ;
        G4double OuterRadius = InnerRadius + fRingWidth;
        trackerChamber.SetInnerRadius(InnerRadius);
        trackerChamber.SetOuterRadius(OuterRadius);
        trackerChamber.SetStartPhiAngle(0.);
        trackerChamber.SetDeltaPhiAngle(2.*M_PI);
    }


 // G4cout << "innerR: " << InnerRadius << G4endl;
 // G4cout << "outerR: " << OuterRadius << G4endl;
 // G4cout << "RingWidth: " << fRingWidth << G4endl; 

// trackerChamber.SetDeltaPhiAngle(M_PI/2. - 2*copyNo*(fRing_gap/OuterRadius));
}

