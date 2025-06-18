#include "FAUSTS1annaularD.hh"
G4VSolid* S1annaularD(G4int PixelIdTop, G4int PixelIdBottom)
{
  
   G4double InnerRadiusStart = 22 *mm;
   G4double OuterRadiusStart = 48 *mm;
   G4double RingGap = 100 *um;

   G4double RingWidth = (OuterRadiusStart - InnerRadiusStart - (RingGap*(16 - 1)))/(16);

    // G4cout << "TopID "<< TopID << G4endl;
    G4double InnerRadius = InnerRadiusStart + PixelIdTop * (RingGap + RingWidth) ;
    G4double OuterRadius = InnerRadius + RingWidth;
    G4double RingGapDphi = (RingGap/InnerRadiusStart); // the gap in radian 
    G4double pDphi = (M_PI/8.) - RingGapDphi ; // the width of a panel in radian  
    G4double PSPhi = (0.5 * RingGapDphi) + (PixelIdBottom *(RingGapDphi + pDphi)); // the radius of a ring given by the copy number
    if (PixelIdBottom == 3)
    {
        pDphi = (M_PI/8) - (2.* PixelIdTop * RingGapDphi) - (2*RingGapDphi);
    }

      G4VSolid* Pixel_sol = new G4Tubs("S1Det", InnerRadius, OuterRadius, 1.0/2., PSPhi, pDphi);
     // make additional cut for rings that exceed 43 *mm in radius
     if (OuterRadius > 43. && PixelIdBottom > 1)
     {
        G4VSolid* top_edge = new G4Box("top_edge", 30. , 5., 1.25 ); 
        G4Transform3D top_edge_tran = G4Translate3D(G4ThreeVector(0., 43. + 5. ,0.)); // these numbers need to be revised (waiting for accurate measurements from Micron Semiconductors 

        Pixel_sol = new G4SubtractionSolid("S1_rings_sol" , Pixel_sol, top_edge, top_edge_tran );
     }

    

      return Pixel_sol;

  
    }
