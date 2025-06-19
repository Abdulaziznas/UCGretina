#include "FAUSTPixelToCoordinates.hh"
G4ThreeVector FAUSTPixelToCoordinates(G4int VstripID, G4int HstripID, G4int PanelID, G4int NoOfHstrips, G4int NoOfVstrips,
                             G4double InterStripGap , G4double base_width, G4double top_width, G4double trap_hight, G4double strip_thickness, G4double LastHStripMagConstant)
{

    // calculate the difference between the base and the top of trapezoid 
    G4double trap_halfWidth_diff = (base_width - top_width)/2;

    // calculate the width of V & H strips


    G4double PreHStrip_width = 0.; // the  total width of the previuse strips
G4double FirstStripWidth = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips + (((LastHStripMagConstant - 1.)/(NoOfHstrips -1)) * (NoOfHstrips * (NoOfHstrips -1) / 2.)));
        for ( G4int i = 0; i < HstripID ; i++ )
    {
        G4double HStrip_widthT = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips);
        HStrip_widthT =  FirstStripWidth + (i * ((LastHStripMagConstant - 1.)/(NoOfHstrips - 1)) *FirstStripWidth) ; 
        PreHStrip_width += HStrip_widthT;
    }
    G4double HStrip_width = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips);
             HStrip_width =   FirstStripWidth + (HstripID * ((LastHStripMagConstant - 1.)/(NoOfHstrips - 1)) *FirstStripWidth) ; 
    G4double HPositionY = (HstripID*InterStripGap)  + PreHStrip_width  +  HStrip_width/2.;

    // calculate the centre of V & H stirps
    /*
    # To do list
        * find the formula for the spacing between the center of the H strips
            * the first strip should start at (HStrip_width/2.)
            * distance to the next center will be InterStripGap + HstripWidth
            * write the formula using the copy number of the strips
        * The first centre of the vertical strips should be at the positive X-axis
            * I need to figure this out today 
        * the equation blew maybe correct   
    */





        // check if the pixel complete rectangle 
        /*
        * calculate the borders HStripTopHalfLength of the top side of the H strip
            G4double Striptop_PositionY = (HstripID*InterStripGap) + (HstripID * HStrip_width) + HStrip_width;
            G4double HStripTopHalfLength = fTrap_base_width/2. - (trap_halfWidth_diff-(Striptop_PositionX  * trap_halfWidth_diff/fTrap_hight));
        * calculate the right and left X position of the vertical strip
            * right side: centre + VStrip_width/2
            * left side: centre - VStrip_width/2
        *  check that both right/left sides lie between ± HStripTopHalfLength
            * if both border outside the abort calculation and the pixel is not part of the detector 
            * if both inside the border, the pixel is complete rectangle and the xy coordinates are (HPositionY + 6 mm) & VPositionX 
            * if one is inside things are complicated and I need to figure it out
        * if no 
        */
        G4double HStripTop_PositionY = HPositionY + HStrip_width/2.;   // Top point of Horizontal strips
        G4double HStripBottom_PositionY = HPositionY - HStrip_width/2.;// Bottom point of Horizontal strips

        G4double HStripTopHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripTop_PositionY)/trap_hight  )); // this is the half length of the top side of a vertical strip
        G4double HStripBottomHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripBottom_PositionY)/trap_hight  )); // this is the half length of the bottom side of a Horizontal strip 


        G4double VStrip_width = (base_width - (InterStripGap*(NoOfVstrips -1)))/(NoOfVstrips);
        G4double VPositionX = (base_width/2) - (VstripID*InterStripGap) - (VstripID * VStrip_width) - VStrip_width/2;
        G4double VStripRightSide =  VPositionX + VStrip_width/2;
        G4double VStripLeftSide =  VPositionX - VStrip_width/2;


        G4double trap_tan = trap_hight/trap_halfWidth_diff ; // used to calculate the hight of strips 

        G4double VStripLeftHeight = ((base_width/2.) - (std::abs(VStripLeftSide))) * trap_tan; // The height of vertical strip on the left hand side
        G4double VStripRightHeight = ((base_width/2.) - (std::abs(VStripRightSide))) * trap_tan; // The height of Vertical strip on the right edge 

        G4bool TLeftInside = false;
        G4bool TRightInside = false;
        G4bool BLeftInside = false;
        G4bool BRightInside = false;
        G4ThreeVector PixelCoordinates = G4ThreeVector(0., 0., 0.);

        // the  + 0.0000005 margin between the two value to overcome rounding errors that may ocurr in previuse calculation ;
        if (std::abs(VStripLeftSide) + 0.0000005 < std::abs(HStripTopHalfLength)) TLeftInside = true;
        if (std::abs(VStripRightSide) + 0.0000005 < std::abs(HStripTopHalfLength)) TRightInside = true;
        if (std::abs(VStripLeftSide) + 0.0000005 < std::abs(HStripBottomHalfLength)) BLeftInside = true;
        if (std::abs(VStripRightSide) + 0.0000005 < std::abs(HStripBottomHalfLength)) BRightInside = true;


        if (TLeftInside && TRightInside) // Complete Pixel
        {
            PixelCoordinates = G4ThreeVector(VPositionX, HPositionY + 6., 0.);
        }
        else if (TLeftInside && BLeftInside && BRightInside) // pixel with five vertices on the Poz X-axis
        {
        G4double TriCenterX = ((2*HStripTopHalfLength) + VStripRightSide)/3.;
        G4double TriCenterY = ((2*VStripRightHeight) + HStripTop_PositionY)/3.;
        G4double TriArea = (std::abs(VStripRightSide) - std::abs(HStripTopHalfLength)) * (HStripTop_PositionY - VStripRightHeight) * 0.5;

        G4double Rect1CenterX = (HStripTopHalfLength + VStripLeftSide)/2.;
        G4double Rect1CenterY = (VStripRightHeight + HStripTop_PositionY)/2.;
        G4double Rect1Area = (std::abs(HStripTopHalfLength) - std::abs(VStripLeftSide)) * (HStripTop_PositionY - VStripRightHeight);


        G4double Rect2CenterX = (VStripLeftSide + VStripRightSide)/2.;
        G4double Rect2CenterY = (VStripRightHeight + HStripBottom_PositionY)/2.;
        G4double Rect2Area = (std::abs(VStripRightSide) - std::abs(VStripLeftSide)) * (VStripRightHeight - HStripBottom_PositionY);

        VPositionX = ((TriCenterX * TriArea) + (Rect1CenterX * Rect1Area) + (Rect2CenterX * Rect2Area)) / (TriArea + Rect1Area + Rect2Area);
        HPositionY = ((TriCenterY * TriArea) + (Rect1CenterY * Rect1Area) + (Rect2CenterY * Rect2Area)) / (TriArea + Rect1Area + Rect2Area);

        PixelCoordinates = G4ThreeVector(VPositionX, HPositionY + 6., 0.);
  
        }
        else if (TRightInside && BLeftInside && BRightInside) // pixel with five vertices on the Neg X-axis
        {
        G4double TriCenterX = ((-2*HStripTopHalfLength) + VStripLeftSide)/3.;
        G4double TriCenterY = ((2*VStripLeftHeight) + HStripTop_PositionY)/3.;
        G4double TriArea = (std::abs(VStripLeftSide) - std::abs(HStripTopHalfLength)) * (HStripTop_PositionY - VStripLeftHeight) * 0.5;

        G4double Rect1CenterX = (-HStripTopHalfLength + VStripRightSide)/2.;
        G4double Rect1CenterY = (VStripLeftHeight + HStripTop_PositionY)/2.;
        G4double Rect1Area = (std::abs(HStripTopHalfLength) - std::abs(VStripLeftSide)) * (HStripTop_PositionY - VStripRightHeight);


        G4double Rect2CenterX = (VStripLeftSide + VStripRightSide)/2.;
        G4double Rect2CenterY = (VStripLeftHeight + HStripBottom_PositionY)/2.;
        G4double Rect2Area = (std::abs(VStripLeftSide) - std::abs(VStripRightSide)) * (VStripLeftHeight - HStripBottom_PositionY);

        VPositionX = ((TriCenterX * TriArea) + (Rect1CenterX * Rect1Area) + (Rect2CenterX * Rect2Area)) / (TriArea + Rect1Area + Rect2Area);
        HPositionY = ((TriCenterY * TriArea) + (Rect1CenterY * Rect1Area) + (Rect2CenterY * Rect2Area)) / (TriArea + Rect1Area + Rect2Area);

        PixelCoordinates = G4ThreeVector(VPositionX, HPositionY + 6., 0.);           
        }
        else if (TLeftInside) // Four vertices incomplete pixel on the Poz X-axis
        {
            G4double TriCenterX = ((2*HStripTopHalfLength) + HStripBottomHalfLength)/3.;
            G4double TriCenterY = ((2*HStripBottom_PositionY) + HStripTop_PositionY)/3.;
            G4double RectCenterX = (HStripTopHalfLength + VStripLeftSide)/2.;
            G4double RectCenterY = HPositionY;

            G4double TriArea = (HStripBottomHalfLength - HStripTopHalfLength) * HStrip_width * 0.5;
            G4double RectArea = (HStripTopHalfLength - std::abs(VStripLeftSide)) * HStrip_width;

            VPositionX = ((TriCenterX * TriArea) + (RectCenterX * RectArea)) / (TriArea + RectArea);
            HPositionY = ((TriCenterY * TriArea) + (RectCenterY * RectArea)) / (TriArea + RectArea);

            PixelCoordinates = G4ThreeVector(VPositionX, HPositionY + 6., 0.);
        }
        else if (TRightInside) // Four vertices incomplete pixel on the Neg X-axis
        {
            G4double TriCenterX = -((2*HStripTopHalfLength) + HStripBottomHalfLength)/3.;
            G4double TriCenterY = ((2*HStripBottom_PositionY) + HStripTop_PositionY)/3.;
            G4double RectCenterX = -(HStripTopHalfLength + std::abs(VStripRightSide))/2.;
            G4double RectCenterY = HPositionY;

            G4double TriArea = (HStripBottomHalfLength - HStripTopHalfLength) * HStrip_width * 0.5;
            G4double RectArea = (HStripTopHalfLength - std::abs(VStripRightSide)) * HStrip_width;

            VPositionX = ((TriCenterX * TriArea) + (RectCenterX * RectArea)) / (TriArea + RectArea);
            HPositionY = ((TriCenterY * TriArea) + (RectCenterY * RectArea)) / (TriArea + RectArea);

            PixelCoordinates = G4ThreeVector(VPositionX, HPositionY + 6., 0.);
        }
        else if(std::abs(VStripLeftSide) < std::abs(HStripBottomHalfLength)) // triangle on the positive x-axis
        {
            G4double TriCenterX = ((HStripBottomHalfLength) + (2.* VStripLeftSide))/3.;
            G4double TriCenterY = ((VStripLeftHeight) + (2. * HStripBottom_PositionY))/3.;
            PixelCoordinates = G4ThreeVector(TriCenterX, TriCenterY + 6., 0.);
        }
        else // Triangle on the negative x-axis
        {
            G4double TriCenterX = ((-HStripBottomHalfLength) + (2.* VStripRightSide))/3.;
            G4double TriCenterY = ((VStripRightHeight) + (2. * HStripBottom_PositionY))/3.;
            PixelCoordinates = G4ThreeVector(TriCenterX, TriCenterY + 6., 0.);
        }

        return PixelCoordinates;
}

G4double FAUSTPixelToCoordinatesS3R(G4int RingId, G4int NoOfRings, G4double InnerRadius , G4double OuterRadius, G4double RingGap)
{
        G4double RingWidth = (OuterRadius - InnerRadius - (RingGap*(NoOfRings - 1)))/(NoOfRings);
        G4double RingInnerRadius = InnerRadius + RingId * (RingGap + RingWidth) ; // the radius of a ring given by the copy number
        G4double RingOuterRadius = RingInnerRadius + RingWidth;
        //G4double PixelCoordinateR = RingInnerRadius + (RingWidth/2.);
        G4double PixelCoordinateR = (2./3.) * ((std::pow(RingOuterRadius,3.)) - (std::pow(RingInnerRadius,3.))) / (std::pow(RingOuterRadius,2.) - std::pow(RingInnerRadius,2.)); // the center of the ring
        return PixelCoordinateR;
}

G4double FAUSTPixelToCoordinatesS3Phi(G4int PanelId, G4int NoOfPanels, G4double InnerRadius, G4double RingGap)
{       
        G4double RingGapDphi = (RingGap/InnerRadius); // the gap in radian  
        G4double pDphi = (M_PI/8.) - RingGapDphi ; // the width of a panel in radian ; // the width of a panel in radian 
        G4double PSPhi = (0.5 * RingGapDphi) + (PanelId *(RingGapDphi + pDphi)); // the radius of a ring given by the copy number
        G4double PixelCoordinatePhi =  PSPhi + (pDphi/2.); 
        return PixelCoordinatePhi;
}

G4double FAUSTPixelToCoordinatesS1R(G4int RingId)
{
          G4double InnerRadiusStart = 22 *mm;
          G4double OuterRadiusStart = 48 *mm;
          G4double RingGap = 100 *um;

         G4double RingWidth = (OuterRadiusStart - InnerRadiusStart - (RingGap*(16 - 1)))/(16);

         // G4cout << "TopID "<< TopID << G4endl;
         G4double InnerRadius = InnerRadiusStart + RingId * (RingGap + RingWidth) ;
         G4double OuterRadius = InnerRadius + RingWidth;
        //G4double PixelCoordinateR = RingInnerRadius + (RingWidth/2.);
        G4double PixelCoordinateR = (2./3.) * ((std::pow(OuterRadius,3.)) - (std::pow(InnerRadius,3.))) / (std::pow(OuterRadius,2.) - std::pow(InnerRadius,2.)); // the center of the ring
        return PixelCoordinateR;
}

G4double FAUSTPixelToCoordinatesS1Phi(G4int RingId, G4int PanelId)
{       
   G4double InnerRadiusStart = 22 *mm;
   G4double OuterRadiusStart = 48 *mm;
   G4double RingGap = 100 *um;
    G4double RingGapDphi = (RingGap/InnerRadiusStart); // the gap in radian 
    G4double pDphi = (M_PI/8.) - RingGapDphi ; // the width of a panel in radian  
    G4double PSPhi = (0.5 * RingGapDphi) + (PanelId *(RingGapDphi + pDphi)); // the radius of a ring given by the copy number
    if (PanelId == 3)
    {
        pDphi = (M_PI/8) - (2.* RingId * RingGapDphi) - (2*RingGapDphi);
    }

        G4double PixelCoordinatePhi =  PSPhi + (pDphi/2.); 
        return PixelCoordinatePhi;
}

G4String PixelCheck(G4int VstripID, G4int HstripID, G4int PanelID, G4int NoOfHstrips, G4int NoOfVstrips,
                             G4double InterStripGap , G4double base_width, G4double top_width, G4double trap_hight, G4double strip_thickness, G4double LastHStripMagConstant)
{
    G4double trap_halfWidth_diff = (base_width - top_width)/2;





        G4double PreHStrip_width = 0.; // the  total width of the previuse strips
        G4double FirstStripWidth = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips + (((LastHStripMagConstant - 1.)/(NoOfHstrips -1)) * (NoOfHstrips * (NoOfHstrips -1) / 2.)));
        for ( G4int i = 0; i < HstripID ; i++ )
    {
        G4double HStrip_widthT = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips);
        HStrip_widthT =  FirstStripWidth + (i * ((LastHStripMagConstant - 1.)/(NoOfHstrips - 1)) *FirstStripWidth) ; 
        PreHStrip_width += HStrip_widthT;
    }
    G4double HStrip_width = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips);
             HStrip_width =   FirstStripWidth + (HstripID * ((LastHStripMagConstant - 1.)/(NoOfHstrips - 1)) *FirstStripWidth) ; 
    G4double HPositionY = (HstripID*InterStripGap) + PreHStrip_width +  HStrip_width/2.;
    G4double HStripTop_PositionY = HPositionY + HStrip_width/2.;
    G4double HStripBottom_PositionY = HPositionY - HStrip_width/2.;// Bottom point of Horizontal strips

    G4double HStripTopHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripTop_PositionY)/trap_hight)); 
    G4double HStripMidHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HPositionY)/trap_hight));
    G4double HStripBottomHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripBottom_PositionY)/trap_hight  )); // this is the half length of the bottom side of a Horizontal strip 

   

    G4double VStrip_width = (base_width - (InterStripGap*(NoOfVstrips -1)))/(NoOfVstrips);
    G4double VPositionX = (base_width/2) - (VstripID*InterStripGap) - (VstripID * VStrip_width) - VStrip_width/2;
    // this is the half length of the top side of a vertical strip
    G4double VStripRightSide =  VPositionX + VStrip_width/2;
    G4double VStripLeftSide =  VPositionX - VStrip_width/2;

    
    G4double trap_tan = trap_hight/trap_halfWidth_diff ; // used to calculate the hight of strips 

    G4double VStripLeftHeight = ((base_width/2.) - (std::abs(VStripLeftSide))) * trap_tan; // The height of vertical strip on the left hand side
    G4double VStripRightHeight = ((base_width/2.) - (std::abs(VStripRightSide))) * trap_tan; // The height of Vertical strip on the right edge 

    G4bool PosXTri = false;
    G4bool NegXTri = false;

  





    G4bool TLeftInside = false;
    G4bool TRightInside = false;
    G4bool BLeftInside = false;
    G4bool BRightInside = false;
    // G4bool CompleteRectangle = false;
    G4ThreeVector PixelCoordinates = G4ThreeVector(0., 0., 0.);
    if (std::abs(VStripLeftSide) < std::abs(HStripTopHalfLength)) TLeftInside = true;
    if (std::abs(VStripRightSide) < std::abs(HStripTopHalfLength)) TRightInside = true;
    if (std::abs(VStripLeftSide) < std::abs(HStripBottomHalfLength)) BLeftInside = true;
    if (std::abs(VStripRightSide) < std::abs(HStripBottomHalfLength)) BRightInside = true;

    if (TLeftInside && TRightInside)
    {
        return "FullPixel";
    } 
    else if (TLeftInside || TRightInside)
    {
        return "PartPixel";
    }
    else if (BLeftInside || BRightInside)
    {
         return "PartPixel";
    }
    else{
        return "outside the detector";
    }
}