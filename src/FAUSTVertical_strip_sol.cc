#include "Vertical_strip_sol.hh"

G4ExtrudedSolid* Vertical_strip_sol(G4int copyNo, G4int NoCChambers, G4double SpacingZ ,G4double base_width, G4double top_width, G4double trap_hight, G4double strip_thickness)
{
  G4double Vstrip_width = (base_width - (SpacingZ*(NoCChambers -1)))/(NoCChambers);

  //G4cout << "Vstrip_width " << Vstrip_width << G4endl; 
  G4double trap_halfWidth_diff = (base_width - top_width)/2; // = 33 *mm 
  G4double triEnd = ((base_width/2.) - trap_halfWidth_diff); // the x axis position of the long side of a triangle 

  G4double trap_tan = trap_hight/trap_halfWidth_diff ; // used to calculate the hight of strips along Z
  G4double StripShortX_Position =   (base_width - ((copyNo) * 2*(SpacingZ+Vstrip_width)))/2.;
  G4double StripLongX_Position =  StripShortX_Position - Vstrip_width;
  G4double StripShortZ_Position = ((base_width/2.) - (StripShortX_Position)) * trap_tan;
  G4double StripLongZ_Position =  ((base_width/2.) - (StripLongX_Position)) * trap_tan;
  //G4cout << "StripShortX_P: " <<  StripShortX_Position << G4endl;
  //G4cout << "StripLongX_P: " <<  StripLongX_Position << G4endl;

  G4ExtrudedSolid* vertStripTEmp;
    if (copyNo == 0)
    {
    std::vector<G4TwoVector> polygon = {G4TwoVector (StripLongX_Position, 0.),
                                       G4TwoVector(StripShortX_Position, 0),
                                       G4TwoVector(StripLongX_Position, StripLongZ_Position)};
     vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2.,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
     return vertStripTEmp;

     
    }
    else if (copyNo == NoCChambers-1)
    {
    StripShortZ_Position = ((base_width/2.) + (StripShortX_Position)) * trap_tan;
    std::vector<G4TwoVector> polygon = {G4TwoVector (StripLongX_Position, 0.),
                                       G4TwoVector(StripShortX_Position, 0),
                                       G4TwoVector(StripShortX_Position, StripShortZ_Position)};
     vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
     return vertStripTEmp;
    }

    else if (StripLongX_Position > triEnd )
    {
      std::vector<G4TwoVector> polygon = {G4TwoVector (StripLongX_Position, 0.),
                                       G4TwoVector(StripShortX_Position, 0),
                                       G4TwoVector(StripShortX_Position, StripShortZ_Position),
                                       G4TwoVector(StripLongX_Position, StripLongZ_Position)};
      vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
     return vertStripTEmp;
    }

    else if (StripShortX_Position > triEnd && StripLongX_Position < triEnd )
    {
      std::vector<G4TwoVector> polygon = {G4TwoVector (StripLongX_Position, 0.),
                                          G4TwoVector(StripShortX_Position, 0),
                                          G4TwoVector(StripShortX_Position, StripShortZ_Position),
                                          G4TwoVector(triEnd, trap_hight),
                                          G4TwoVector(StripLongX_Position, trap_hight)};
      vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
     return vertStripTEmp;
    }

    else if (StripShortX_Position < triEnd && StripLongX_Position > - triEnd)
    {
      std::vector<G4TwoVector> polygon = {G4TwoVector (StripLongX_Position, 0.),
                                       G4TwoVector(StripShortX_Position, 0),
                                       G4TwoVector(StripShortX_Position, trap_hight),
                                       G4TwoVector(StripLongX_Position, trap_hight),};
      vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
     return vertStripTEmp;
    }



    else if (StripShortX_Position > - triEnd && StripLongX_Position < -triEnd )
    {
      StripLongZ_Position = ((base_width/2.) + (StripLongX_Position)) * trap_tan;
      std::vector<G4TwoVector> polygon = {G4TwoVector (StripLongX_Position, 0.),
                                          G4TwoVector(StripShortX_Position, 0),
                                          G4TwoVector(StripShortX_Position, trap_hight),
                                          G4TwoVector(- triEnd, trap_hight),
                                          G4TwoVector(StripLongX_Position, StripLongZ_Position)};
      vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2.  ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
     return vertStripTEmp;
    }

    else if (StripShortX_Position < -triEnd)
    {
      StripShortZ_Position = ((base_width/2.) + (StripShortX_Position)) * trap_tan;
      StripLongZ_Position =  ((base_width/2.) + (StripLongX_Position)) * trap_tan;

      std::vector<G4TwoVector> polygon = {G4TwoVector (StripLongX_Position, 0.),
                                       G4TwoVector(StripShortX_Position, 0),
                                       G4TwoVector(StripShortX_Position, StripShortZ_Position),
                                       G4TwoVector(StripLongX_Position, StripLongZ_Position)};
      vertStripTEmp = new G4ExtrudedSolid("vetStripTemp", polygon, strip_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1);
     return vertStripTEmp;
    }
    
      return vertStripTEmp;
    
}
