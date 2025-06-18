#include "FAUSTTrapPixel.hh"


G4ExtrudedSolid* TrapPixel(G4int VstripID, G4int HstripID, G4int PanelID, 
                    G4int NoOfHstrips, G4int NoOfVstrips,
                    G4double InterStripGap , G4double base_width, 
                    G4double top_width, G4double trap_hight,
                    G4double strip_thickness, G4double LastHStripMagConstant)
{
            G4double PreHStrip_width = 0.; // the  total width of the previuse strips
             G4double FirstStripWidth = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips + (((LastHStripMagConstant - 1.)/(NoOfHstrips -1)) * (NoOfHstrips * (NoOfHstrips -1) / 2.)));

        for ( G4int i = 0; i < HstripID ; i++ )
    {
        G4double HStrip_widthT = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips);
        HStrip_widthT =   FirstStripWidth + (i * FirstStripWidth * (LastHStripMagConstant - 1.)/(NoOfHstrips - 1) ) ; 
        PreHStrip_width += HStrip_widthT;
    }
    G4double HStrip_width = (trap_hight - (InterStripGap*(NoOfHstrips -1)))/(NoOfHstrips);
             HStrip_width =  FirstStripWidth + (HstripID * FirstStripWidth * (LastHStripMagConstant - 1.)/(NoOfHstrips - 1) ) ; 
    G4double HPositionY = (HstripID*InterStripGap) + PreHStrip_width  +  HStrip_width/2.;

    G4double trap_halfWidth_diff = (base_width - top_width)/2;
    G4double VStrip_width = (base_width - (InterStripGap*(NoOfVstrips -1)))/(NoOfVstrips);

    G4double VPositionX = (base_width/2) - (VstripID*InterStripGap) - (VstripID * VStrip_width) - VStrip_width/2; // mid points of Vertical   strips


    G4double HStripTop_PositionY = HPositionY + HStrip_width/2.;   // Top point of Horizontal strips
    G4double HStripBottom_PositionY = HPositionY - HStrip_width/2.;// Bottom point of Horizontal strips

    G4double VStripRightSide =  VPositionX + VStrip_width/2; // V strip's right edge
    G4double VStripLeftSide =  VPositionX - VStrip_width/2;  // V strip's left edge 


    G4double HStripBottomHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripBottom_PositionY)/trap_hight  )); // this is the half length of the bottom side of a Horizontal strip 
    G4double HStripTopHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripTop_PositionY)/trap_hight  )); // this is the half length of the top side of a Horizontal strip     
    G4double HStripMidHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HPositionY)/trap_hight)); // this is the half length of the bottom side of a Horizontal strip 

    G4double trap_tan = trap_hight/trap_halfWidth_diff ; // used to calculate the hight of strips 

    G4double VStripLeftHeight = ((base_width/2.) - (std::abs(VStripLeftSide))) * trap_tan; // The height of vertical strip on the left hand side
    G4double VStripRightHeight = ((base_width/2.) - (std::abs(VStripRightSide))) * trap_tan; // The height of Vertical strip on the right edge     
    G4ExtrudedSolid* vertStripTEmp;   
    G4bool LeftInside = false;
    G4bool RightInside = false;
    G4bool BLeftInside = false;
    G4bool BRightInside = false;
   // the  + 0.0000005 margin between the two value to overcome rounding errors that may ocurr in previuse calculation ;
    if (std::abs(VStripLeftSide) + 0.0000005 < std::abs(HStripTopHalfLength)) LeftInside = true;
    if (std::abs(VStripRightSide) + 0.0000005 < std::abs(HStripTopHalfLength)) RightInside = true; 
    if (std::abs(VStripLeftSide) + 0.0000005 < std::abs(HStripBottomHalfLength)) BLeftInside = true;
    if (std::abs(VStripRightSide) + 0.0000005 < std::abs(HStripBottomHalfLength)) BRightInside = true;

    // check if pixel made of five vertices 
    /*
    * One of the top sides of the vertical strip is inside
    * Both bottom sides of the vertical strip are inside
    */
  
   if (LeftInside && BLeftInside && BRightInside)
   {

      std::vector<G4TwoVector> polygon = {G4TwoVector (VStripRightSide, HStripBottom_PositionY  - trap_hight/2.),
                                          G4TwoVector(VStripLeftSide, HStripBottom_PositionY - trap_hight/2.),
                                          G4TwoVector(VStripLeftSide, HStripTop_PositionY - trap_hight/2.),
                                          G4TwoVector(HStripTopHalfLength, HStripTop_PositionY- trap_hight/2.),
                                          G4TwoVector(VStripRightSide, VStripRightHeight - trap_hight/2.)};
          vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
         return vertStripTEmp; 
   }

   if (RightInside && BLeftInside && BRightInside)
    {

         // G4cout << "VStripLeftSide: " << VStripLeftSide << G4endl;
        //  G4cout << "VStripLeftSide: " << VStripRightSide << G4endl;
       std::vector<G4TwoVector> polygon = {G4TwoVector (VStripRightSide, HStripBottom_PositionY  - trap_hight/2.),
                                        G4TwoVector(VStripLeftSide, HStripBottom_PositionY - trap_hight/2.),
                                        G4TwoVector(VStripLeftSide, VStripLeftHeight - trap_hight/2.),
                                        G4TwoVector(-HStripTopHalfLength, HStripTop_PositionY- trap_hight/2.),
                                       G4TwoVector(VStripRightSide, HStripTop_PositionY - trap_hight/2.)};
      vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
      return vertStripTEmp; 
     }


   if (LeftInside)
    {   


          std::vector<G4TwoVector> polygon = {G4TwoVector (HStripBottomHalfLength, HStripBottom_PositionY  - trap_hight/2.),
                                          G4TwoVector(VStripLeftSide, HStripBottom_PositionY - trap_hight/2.),
                                          G4TwoVector(VStripLeftSide, HStripTop_PositionY - trap_hight/2.),
                                          G4TwoVector(HStripTopHalfLength, HStripTop_PositionY- trap_hight/2.)};
          vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
         
         return vertStripTEmp; 
    }
  if (RightInside)
  {

          std::vector<G4TwoVector> polygon = {G4TwoVector (-HStripBottomHalfLength, HStripBottom_PositionY - trap_hight/2.),
                                          G4TwoVector(VStripRightSide, HStripBottom_PositionY - trap_hight/2.),
                                          G4TwoVector(VStripRightSide, HStripTop_PositionY - trap_hight/2.),
                                          G4TwoVector(-HStripTopHalfLength, HStripTop_PositionY- trap_hight/2.)};
          vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
          
                   return vertStripTEmp; 
      }
      else if (std::abs(VStripLeftSide) < std::abs(HStripBottomHalfLength)){
            // The side on the positive x-axis
           // G4cout << "HStripTopHalfLength - VStripLeftSide: " << HStripTopHalfLength - VStripLeftSide  << G4endl;
              std::vector<G4TwoVector> polygon = {G4TwoVector (HStripBottomHalfLength, HStripBottom_PositionY - trap_hight/2.),
                                          G4TwoVector(VStripLeftSide, HStripBottom_PositionY - trap_hight/2.),
                                          G4TwoVector(VStripLeftSide, VStripLeftHeight  - trap_hight/2.)};
          vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
         return vertStripTEmp;
      }
      else {

                       std::vector<G4TwoVector> polygon = {G4TwoVector (-HStripBottomHalfLength, HStripBottom_PositionY - trap_hight/2.),
                                           G4TwoVector(VStripRightSide, HStripBottom_PositionY - trap_hight/2.),
                                           G4TwoVector(VStripRightSide, VStripRightHeight  - trap_hight/2.)};
           vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
          return vertStripTEmp;
      }
}
