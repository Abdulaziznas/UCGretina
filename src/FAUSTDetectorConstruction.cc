#include "FAUSTDetectorConstruction.hh"

#include <iostream>
#include <fstream>
using namespace std;

FAUSTDetectorConstruction::FAUSTDetectorConstruction() : fStepLimit(NULL)
{
  fMessenger = new G4GenericMessenger(this, "/Detector/", "Detector Dimensions" );
  fMessenger->DeclareProperty("trap_inter-strip_gap", fStrip_gap, "Inter-strip gap for the horizontal & vertical strips in mm");
  fMessenger->DeclareProperty("trap_Si_thk", fStrip_thickness, "the thickness of the H & V strips in mm" );
  fMessenger->DeclareProperty("CsI_thk", fCsI_thickness, "The thickness of CsI layer");
  fMessenger->DeclareProperty("S3_thk", fS3_thickness, "The thickness of S1/S3 annular detector");
  fMessenger->DeclareProperty("S3_target_distance", fS3_target_dist, "the distance between S1/S3 annular detector and the target in mm");
  fMessenger->DeclareProperty("Chamber_gap", fChamber_gap, "Size of separation between up- and down-stream arrays at 90-degree in mm");
  fMessenger->DeclareProperty("VstripNo", fVstripNo, " the number of Vertical strips default 16 strip (this is G4int)");
  fMessenger->DeclareProperty("HstripNo", fHstripNo, " the number of Vertical strips default 64 strip (this is G4int)");
  fMessenger->DeclareProperty("S3_or_S1", fAnnular_det, "type {S3} or {S1}");
  fMessenger->DeclareProperty("StripWidthRatio", fLastHStripMagConstant, "Strip Width ratio, strip near 180 deg / strip near 90 deg");
  fMessenger->DeclareProperty("OutputFileAddOns", fOutFileName, "type Something unique about this Run");

  // Default values 
  fStrip_gap = 60. *um; // inter-strip gap for the Horizontal and vertical strips
  fRings_gap = 100. *um;  // inter-strip gap for the S1/S3 annular detector
  fStrip_thickness = 0.5 *mm; // thickness of Silicon strips ( horizontal + vertical = 0.5 mm)
  fCsI_thickness = 3. *mm;  // Thickness of CsI back-plates
  fS3_thickness = 1. *mm; // thickness of S3 annular detector
  fChamber_gap = 10. *mm;  // Size of separation between up- and down-stream arrays at 90-degree in mm.
  fS3_target_dist = 125. *mm; // the distance between S1/S3 annular detector and the target
  fVstripNo = 16; // number of vertical strips of the trapezoid 
  fHstripNo = 64; // number of the horizontal strips of the trapezoid
  fS3ringNo = 24; // number of S3 annular detector rings 
  fLastHStripMagConstant = 1;  
  fAnnular_det = "S3";
  G4String SiThickness = to_string(fStrip_thickness);
  G4String CsIThickness = to_string(fCsI_thickness);
  fOutFileName = SiThickness + "um" + CsIThickness + "mm";
  FAUSTMaterialDefinition();
}

FAUSTDetectorConstruction::~FAUSTDetectorConstruction()
{
  delete fStepLimit;

}

void FAUSTDetectorConstruction::FAUSTMaterialDefinition()
{
  // Materials
  G4NistManager* nist = G4NistManager::Instance();
  fVacuum = nist->FindOrBuildMaterial("G4_Galactic");
  fGermanium  = nist->FindOrBuildMaterial("G4_Ge");
     //silicon definition
 // fSilicon = new G4Element("Silicon", "Si", 14., 28.0855*g/mole);
 // fSilicon_Mat = new G4Material("Silicon_Mat", 2.329002*g/cm3, 1);
 // fSilicon_Mat->AddElement(fSilicon, 1);
 fSilicon_Mat = nist->FindOrBuildMaterial("G4_Si");
  // Caesium iodide definition
 fCsI_Mat = nist->FindOrBuildMaterial("G4_CESIUM_IODIDE");

  //  STAINLESS STEEL definition
 fStainlessSteel_Mat = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");

   //  aluminum definition
 fAluminum_Mat = nist->FindOrBuildMaterial("G4_Al");

 // Carbon fiber definition
 fCarbonFiber_Mat = new G4Material("CarbonFiber_Mat", 1.6 *g/cm3, 2);
 fCarbonFiber_Mat->AddElement(nist->FindOrBuildElement("C"), 90.*perCent);
 fCarbonFiber_Mat->AddMaterial(nist->FindOrBuildMaterial("G4_POLYETHYLENE"), 10.*perCent); // 10% resin (e.g., polyethylene)

 fMylar_Mat = nist->FindOrBuildMaterial("G4_MYLAR");

 // Deuterated plastic definition
  fDeuterium = new G4Element("Deuterium", "D", 1);
  fDeuterium_Isotope = new G4Isotope("Deuterium", 1, 2, 2.014102*g/mole);
  fDeuterium->AddIsotope(fDeuterium_Isotope, 100.*perCent);

  fDeuteratedPlastic_Mat = new G4Material("DeuteratedPlastic_Mat", 1.08 *g/cm3, 2); // 1.08 g/cm3 is the density of deuterated plastic; source: https://www.chemicalbook.com/ChemicalProductProperty_EN_CB8137689.htm
  fDeuteratedPlastic_Mat->AddElement(fDeuterium, 4);
  fDeuteratedPlastic_Mat->AddElement(nist->FindOrBuildElement("C"), 2);


  // fCaesium = new G4Element("Caesium", "Cs", 55., 132.90545*g/mole);
  // fIodine = new G4Element("Iodine", "I", 53., 126.904473*g/mole);
  // fCsI_Mat = new G4Material("CsI_Mat", 4.51*g/cm3, 2);
  // fCsI_Mat->AddElement(fCaesium, 1);
  // fCsI_Mat->AddElement(fIodine, 1);
}

G4VPhysicalVolume* FAUSTDetectorConstruction::Construct()
{
  G4bool checkOverlaps = true;




  GlobalVariables::HStripNo_Global = fHstripNo;
  GlobalVariables::VStripNo_Global = fVstripNo;
  GlobalVariables::SiThickness = fStrip_thickness * 1000 ;
  GlobalVariables::CsIThickness = fCsI_thickness;
  GlobalVariables::OutFileName = fOutFileName;

  

      /***** Step limit *****/
  const G4double maxStep = 0.01*um;
  fStepLimit = new G4UserLimits(maxStep);
  // Definition of the volumes
  // World
  G4VSolid* expHall = new G4Box("ExperimentalHall", 2.*m, 2.*m, 2.*m);
  G4LogicalVolume* expHall_log = 
  new G4LogicalVolume(expHall, fVacuum, "ExperimentalHall");
  G4VPhysicalVolume* physExpHall = 
  new G4PVPlacement(G4Transform3D(), expHall_log, "ExperimentalHall", 0, 
                    false, 0);


  // sphere for repersnet GRETINA dimensions 
 // Definition of the  sphere
  G4double  pRmin = 180. * mm;
  G4double  pRmax = 185. * mm;                                                       // Outer radius
  G4double  pSPhi = 0. * degree;                                                    // Starting Phi angle of the segment in radians  
  G4double  pDPhi = 360 * degree;                                                   // Delta Phi angle of the segment in radians
  G4double  pSTheta = 0. * degree;                                                  // Starting Theta angle of the segment in radians
  G4double  pDTheta = 360 * degree;                                                 // Delta Theta angle of the segment in radians
  G4VSolid* GRETINA_sol = new G4Sphere("GRETINA",
                            pRmin,
                            pRmax,
                            pSPhi,
                            pDPhi,
                            pSTheta,
                            pDTheta);
  G4LogicalVolume* GRETINA_log = new G4LogicalVolume(GRETINA_sol, fGermanium, "GRETINA");
  // new G4PVPlacement(0 , G4ThreeVector(0., 0., 0.), GRETINA_log , "GRETINA", expHall_log, false, 0, checkOverlaps);

 // sphere for repersnet target chamber dimensions 
 // Definition of the  sphere
 pRmin = 160. * mm;
 pRmax = 161.5875 * mm;                                                       // Outer radius
 pSPhi = 0. * degree;                                                    // Starting Phi angle of the segment in radians  
 pDPhi = 360 * degree;                                                   // Delta Phi angle of the segment in radians
 pSTheta = 0. * degree;                                                  // Starting Theta angle of the segment in radians
 pDTheta = 360 * degree;                                                 // Delta Theta angle of the segment in radians
G4VSolid* TargetChamber_sol = new G4Sphere("TargetChamber",
                pRmin,
                pRmax,
                pSPhi,
                pDPhi,
                pSTheta,
                pDTheta);
G4LogicalVolume* TargetChamber_log = new G4LogicalVolume(TargetChamber_sol, fCarbonFiber_Mat, "TargetChamber");
//new G4PVPlacement(0 , G4ThreeVector(0., 0., 0.), TargetChamber_log , "TargetChamber", expHall_log, false, 0, checkOverlaps);

//  //  target frame
          G4double  pRMin =  12.5 *mm;   // inner Aluminum frame
          G4double  pRMax = 15. *mm;   // outer radius of Aluminum frame
          G4double  pDz = 4. *mm;   //  thickness of the Aluminum frame
                 pSPhi = 0. ;
                 pDPhi = 2. * M_PI;
       G4VSolid* TargetFrame_sol = new G4Tubs("TargetFrame", pRMin , pRMax, pDz/2  ,pSPhi ,pDPhi);
       G4LogicalVolume* TargetFrame_log = new G4LogicalVolume(TargetFrame_sol, fAluminum_Mat, "TargetFrame");
 //     new G4PVPlacement(0, G4ThreeVector(0.,0., 0.), TargetFrame_log, "TargetFrame", expHall_log, 0, 0, checkOverlaps);
  TargetFrame_log->SetUserLimits(fStepLimit);
  G4VisAttributes* TargetFrame_vis = new G4VisAttributes(G4Colour(1.,1., 1.));
  TargetFrame_vis->SetDaughtersInvisible(false);
  TargetFrame_vis->SetForceSolid(true);
  TargetFrame_log->SetVisAttributes(TargetFrame_vis);


  //  target foil
            pRMin =  0. *mm;   // inner radius target foil
            pRMax = 12.5 *mm;   // outer radius target foil
            pDz = 10. *um;   //  thickness target foil
            pSPhi = 0. ;
            pDPhi = 2. * M_PI;
  G4VSolid* TargetFoil_sol = new G4Tubs("TargetFoil", pRMin , pRMax, pDz/2  ,pSPhi ,pDPhi);
  G4LogicalVolume* TargetFoil_log = new G4LogicalVolume(TargetFoil_sol, fDeuteratedPlastic_Mat, "TargetFoil");
  // new G4PVPlacement(0, G4ThreeVector(0.,0., 0.), TargetFoil_log, "TargetFoil", expHall_log, 0, 0, checkOverlaps);
  TargetFoil_log->SetUserLimits(fStepLimit);
  G4VisAttributes* TargetFoil_vis = new G4VisAttributes(G4Colour(1.,1., 0.));
  TargetFoil_vis->SetDaughtersInvisible(false);
  TargetFoil_vis->SetForceSolid(true);
  TargetFoil_log->SetVisAttributes(TargetFoil_vis);



  // parameters for the PCB frames (trap's mother volume) 
  G4double AluminumThickness = 0. *um;
  G4double MylarThickness = 0. *um;
  G4double PCB_hight = 116. *mm;
  G4double PCB_base_width = 104. *mm;
  G4double PCB_top_width = 37. *mm;
  G4double PCB_ext = 6. *mm;  // the width of the box frame at the bottom of the trapezoid 
  G4double PCB_Total_thickness = fStrip_thickness + fCsI_thickness + (2. * AluminumThickness) + MylarThickness; // the thickness of the trapezoids' mother volume


  // common parameters for the trapezoids
  G4double trap_hight = 104. *mm; // the total hight of the trapezoid
  G4double base_width = 91. *mm; // the width of the trapezoid base
  G4double top_width = 25. *mm; // the width of the trapezoid top


 


  // lampshade radius and tilt angle (the origin is at the base of the PCB frame)
  G4double temp2 = PCB_ext * (PCB_base_width/2. - PCB_top_width/2.)/ (PCB_hight - PCB_ext); // additional width of the trapezoid because of the 6 mm frame at the base
  G4double tilt_angle = std::acos((PCB_base_width/2. - PCB_top_width/2.) * std::tan(67.5 * M_PI/180.)/(PCB_hight - PCB_ext));
  G4double temp = (PCB_Total_thickness/2.)/std::sin(((M_PI/2. - tilt_angle))); // the additional width of the octagon based on thickness and tilt angle
  G4double lampshade_radius = temp/2. + (temp2 + (PCB_base_width/2.)) * std::tan(67.5 * deg);



  // defintion of the mother volume trap's mother volume
std::vector<G4TwoVector> PCB_polygon = {G4TwoVector (PCB_base_width/2., 0.),
                                       G4TwoVector(-PCB_base_width/2., 0),
                                       G4TwoVector(-PCB_base_width/2., PCB_ext),
                                       G4TwoVector(-PCB_top_width/2., PCB_hight),
                                       G4TwoVector(PCB_top_width/2., PCB_hight),
                                       G4TwoVector(PCB_base_width/2., PCB_ext)};
G4VSolid* Trap_expHall = new G4ExtrudedSolid("vetStripTemp", PCB_polygon, PCB_Total_thickness/2. ,  G4TwoVector(0,0), 1., G4TwoVector(0,0), 1); // to show local axis use (/vis/scene/add/localAxes <phys-vol-name> [copy-no>])
G4LogicalVolume* Trap_expHall_log = new G4LogicalVolume(Trap_expHall, fVacuum, "Trap_ExperimentalHall");
 // new G4PVPlacement(G4Transform3D() , Trap_expHall_log , "trap_Detectors", expHall_log, false, 0, checkOverlaps); // this line should be commented 



  // CsI backplates
  //G4double trap_halfWidth_diff = (base_width - top_width)/2;
  G4double CsIOverflow = 3. *mm;
  G4VSolid* CsI_Trapezoid_sol = new G4Trd("CsItrapzoide", (base_width + CsIOverflow)/2., (top_width + CsIOverflow)/2., fCsI_thickness/2., fCsI_thickness/2., (trap_hight + CsIOverflow)/2.);
  G4LogicalVolume* CsI_Detector_log = new G4LogicalVolume(CsI_Trapezoid_sol, fCsI_Mat, "CsI_Detector");
  G4Transform3D CsI_DetectorTrans = G4Translate3D(G4ThreeVector(0., PCB_ext + trap_hight/2., AluminumThickness + MylarThickness/2. + fStrip_thickness/2.)) * G4Rotate3D(-M_PI/2. *rad , G4ThreeVector(1. , 0., 0.));
  G4VPhysicalVolume* CsI_phys = new G4PVPlacement(CsI_DetectorTrans, CsI_Detector_log, "CsItrapzoide", Trap_expHall_log, false, 0, checkOverlaps);
  CsI_Detector_log->SetUserLimits(fStepLimit);
  G4VisAttributes* CsI_Detector_vis = new G4VisAttributes(G4Colour(1.,0., 1.));
  CsI_Detector_vis->SetDaughtersInvisible(false);
  CsI_Detector_vis->SetForceSolid(true);
  CsI_Detector_log->SetVisAttributes(CsI_Detector_vis);

 // // Mylar Film
// G4VSolid* MylarFilm_sol = new G4Trd("MylarFilm", (base_width + CsIOverflow)/2., (top_width + CsIOverflow)/2., MylarThickness/2., MylarThickness/2., (trap_hight + CsIOverflow)/2.);
// G4LogicalVolume* MylarFilm_log = new G4LogicalVolume(MylarFilm_sol, fMylar_Mat, "MylarFilm");
// G4Transform3D MylarFilmTrans = G4Translate3D(G4ThreeVector(0., PCB_ext + trap_hight/2., PCB_Total_thickness/2. - fCsI_thickness - MylarThickness/2. )) * G4Rotate3D(-M_PI/2. *rad , G4ThreeVector(1. , 0., 0.));
// new G4PVPlacement(MylarFilmTrans, MylarFilm_log, "MylarFilm", Trap_expHall_log, false, 0, checkOverlaps);
// G4VisAttributes* MylarFilm_vis = new G4VisAttributes(G4Colour(.5,0., 1.));
// MylarFilm_vis->SetDaughtersInvisible(false);
// // MylarFilm_vis->SetForceSolid(true);
// MylarFilm_log->SetVisAttributes(MylarFilm_vis);
 

// Si Active Volume
  G4VSolid* Si_Trapezoid_sol = new G4Trd("Sitrapzoide",  base_width/2.,  top_width/2., fStrip_thickness/2., fStrip_thickness/2.,  trap_hight/2.);
  G4LogicalVolume* Si_Detector_log = new G4LogicalVolume(Si_Trapezoid_sol, fSilicon_Mat, "Si_Detector");
  G4Transform3D Si_DetectorTrans = G4Translate3D(G4ThreeVector(0., PCB_ext + trap_hight/2., - PCB_Total_thickness/2. + AluminumThickness + fStrip_thickness/2.)) * G4Rotate3D(-M_PI/2. *rad , G4ThreeVector(1. , 0., 0.));
  G4VPhysicalVolume* Si_phys = new G4PVPlacement(Si_DetectorTrans, Si_Detector_log, "SiTrapzoideDeadLayer", Trap_expHall_log, false, 0, checkOverlaps);
  Si_Detector_log->SetUserLimits(fStepLimit);







//////// pixel to coordinate loop ///////////////////
  G4int PixelID = 0;
  G4double VStrip_width = (base_width - (fStrip_gap*(fVstripNo -1)))/(fVstripNo);
  G4double HStrip_width = (trap_hight - (fStrip_gap*(fHstripNo -1)))/(fHstripNo);
  G4VSolid* FullPixel_sol = new G4Box("TrapPixel", VStrip_width/2., HStrip_width/2.,  fStrip_thickness/2.);
  G4LogicalVolume* FullPixel_log = new G4LogicalVolume(FullPixel_sol, fSilicon_Mat, "TrapPixel");
  std::vector<G4LogicalVolume*> PartPixel;
  G4int VectorIndex = 0;
  G4VisAttributes* TrapPixel_vis = new G4VisAttributes(G4Colour(1., 0., 0.));
  TrapPixel_vis->SetDaughtersInvisible(false);
  // TrapPixel_vis->SetForceSolid(true);
  FullPixel_log->SetVisAttributes(TrapPixel_vis);
  FullPixel_log->SetUserLimits(fStepLimit);


 G4double FirstStripWidth = (trap_hight - (fStrip_gap*(fHstripNo -1)))/(fHstripNo + (((fLastHStripMagConstant - 1.)/(fHstripNo -1)) * (fHstripNo * (fHstripNo -1) / 2.)));

  for (G4int j = 0; j < fHstripNo; j++) // top strip loop 
  {
    HStrip_width = FirstStripWidth + (j* FirstStripWidth * (fLastHStripMagConstant - 1.)/(fHstripNo - 1) ) ;
    for (G4int k = 0; k < fVstripNo; k++) //bottom strip loop
    {        
      G4String PixelTest = PixelCheck(k, j, 0, fHstripNo, fVstripNo, fStrip_gap , base_width, top_width,  trap_hight, fStrip_thickness, fLastHStripMagConstant );
      if (PixelTest == "FullPixel" )
      {
        G4VSolid* FullPixel_sol = new G4Box("TrapPixel", VStrip_width/2., HStrip_width/2.,  fStrip_thickness/2.);
        G4LogicalVolume* FullPixel_log = new G4LogicalVolume(FullPixel_sol, fSilicon_Mat, "TrapPixel");
        G4ThreeVector PixelCoordinate = FAUSTPixelToCoordinates(k, j, 0, fHstripNo, fVstripNo, fStrip_gap , base_width, top_width,  trap_hight, fStrip_thickness, fLastHStripMagConstant);
        G4Transform3D transformTemp = G4Rotate3D(90. *deg , G4ThreeVector(1. , 0., 0.)) * G4Translate3D(G4ThreeVector(PixelCoordinate.x(), PixelCoordinate.y() - trap_hight/2. - PCB_ext, PixelCoordinate.z()));
        new G4PVPlacement(transformTemp , FullPixel_log , "TrapPixel", Si_Detector_log, false, j*100000 + k, checkOverlaps);
        PixelID++;
        FullPixel_log->SetVisAttributes(TrapPixel_vis);
        FullPixel_log->SetUserLimits(fStepLimit);
      }
     else if (PixelTest == "PartPixel")
     {

         G4VSolid* Si_stripV_sol = TrapPixel(k, j, 0, fHstripNo, fVstripNo, fStrip_gap , base_width, top_width,  trap_hight, fStrip_thickness, fLastHStripMagConstant);
         PartPixel.push_back(new G4LogicalVolume(Si_stripV_sol, fSilicon_Mat, "TrapPixel"));
         G4Transform3D transformTemp = G4Rotate3D(90. *deg , G4ThreeVector(1. , 0., 0.));
         new G4PVPlacement(transformTemp, PartPixel.at(VectorIndex), "TrapPixel", Si_Detector_log, false, j*100000 + k, checkOverlaps);
         PartPixel.at(VectorIndex)->SetVisAttributes(TrapPixel_vis);
         PartPixel.at(VectorIndex)->SetUserLimits(fStepLimit);

      
         VectorIndex++;
         PixelID++;
     }
     }
   }



 // /// ########### Start of Aluminum contact Code ########## ///
 //     // Top strip
 //     for (G4int i = 0; i < fHstripNo; i++) // top strip loop
 //  {
 //     HStrip_width = FirstStripWidth + (i* FirstStripWidth * (fLastHStripMagConstant - 1.)/(fHstripNo - 1) );
 //     // calculate the total width of previuse strips
 //     G4double PreHStrip_width = 0.; // the  total width of the previuse strips
 //     for (G4int j = 0; j < i; j++)
 //     {
 //     G4double HStrip_widthT = (trap_hight - (fStrip_gap*(fHstripNo -1)))/(fHstripNo);
 //        HStrip_widthT =  FirstStripWidth + (j * ((fLastHStripMagConstant - 1.)/(fHstripNo - 1)) *FirstStripWidth) ; 
 //        PreHStrip_width += HStrip_widthT;
 //     }
 //     G4double HPositionY =  (i*fStrip_gap)  + PreHStrip_width  + HStrip_width/2.; 
 //     
 //     G4double trap_halfWidth_diff = (base_width - top_width)/2;
 //     G4double HStripTop_PositionY = HPositionY + HStrip_width/2.;   // Top point of Horizontal strips
 //     G4double HStripBottom_PositionY = HPositionY - HStrip_width/2.;// Bottom point of Horizontal strips
 //
 //     G4double HStripTopHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripTop_PositionY)/trap_hight  )); // this is the half length of the top side of a vertical strip         G4double strip_top_width = base_width/2. - (trap_halfWidth_diff-(HStripTop_PositionY  * trap_halfWidth_diff/trap_hight));
 //     G4double HStripBottomHalfLength = top_width/2. + (trap_halfWidth_diff * ((trap_hight - HStripBottom_PositionY)/trap_hight  )); // this is the half length of the bottom side of a Horizontal strip 
 //
 //     
 //     G4VSolid* AlTopStrip_sol = new G4Trd("AlTopStrip",   HStripBottomHalfLength,  HStripTopHalfLength, AluminumThickness/2., AluminumThickness/2.,  HStrip_width/2.);
 //     G4LogicalVolume* AlTopStrip_log = new G4LogicalVolume(AlTopStrip_sol, fAluminum_Mat, "AlTopStrip");
 //     G4Transform3D AlTopStripTrans = G4Translate3D(G4ThreeVector(0., HPositionY + PCB_ext , - PCB_Total_thickness/2. + AluminumThickness/2.)) * G4Rotate3D(-M_PI/2. *rad , G4ThreeVector(1. , 0., 0.));
 //     new G4PVPlacement(AlTopStripTrans , AlTopStrip_log , "AlTopStrip", Trap_expHall_log, false, i, checkOverlaps); 
 //     G4VisAttributes* AlTopStrip_vis = new G4VisAttributes(G4Colour(1., 1., 1.));
 //     AlTopStrip_vis->SetDaughtersInvisible(false);
 //    // AlTopStrip_vis->SetForceSolid(true);
 //     AlTopStrip_log->SetVisAttributes(AlTopStrip_vis);
 //  }
 //    // bottom Strips
 //     // V trap
 // std::vector<G4LogicalVolume*> Vstrip;
 // G4VisAttributes* Si_stripV_vis = new G4VisAttributes(G4Colour(1., 1., 1.));
 // Si_stripV_vis->SetDaughtersInvisible(false);
 // // Si_stripV_vis->SetForceSolid(true);
 //   for (G4int i = 0; i < fVstripNo; i++)
 //   {
 //     G4VSolid* Si_stripV_sol = Vertical_strip_sol(i, fVstripNo, fStrip_gap ,base_width, top_width, trap_hight, AluminumThickness);
 //     Vstrip.push_back(new G4LogicalVolume(Si_stripV_sol, fAluminum_Mat, "stripV"));
 //     G4Transform3D transformTemp = G4Translate3D(G4ThreeVector(0., PCB_ext , - PCB_Total_thickness/2. + fStrip_thickness + AluminumThickness + AluminumThickness/2.));// * G4Rotate3D(90. *deg , G4ThreeVector(1. , 0., 0.));
 //      new G4PVPlacement(transformTemp, Vstrip.at(i), "stripV", Trap_expHall_log, false, i, checkOverlaps);
 //     Vstrip.at(i)->SetVisAttributes(Si_stripV_vis);
 //     Vstrip.at(i)->SetUserLimits(fStepLimit);
 //   }
 //
 
 
 
  /// ############ End of Aluminum contact Code ########## ///

  // place the trapezoids in a lampshade configuration 
  G4double rot_angle_Z = 0. *rad;
  G4double x_coordinate;
  G4double y_coordinate;
  G4double lamp_rot = 0. *deg; // used placing the second lampshade 
  G4double trgt_dis = (fChamber_gap/2.) + ((PCB_Total_thickness/2.)*std::sin((M_PI/2.)- tilt_angle)); // distance between the closest edge of the PCB frame and the target

    ////////////////////////////////////////// Pixel to Coordinate code //////////////////////////////////////////
   // create G4orb to test PixelToCoordinates code
      // G4VSolid* Orb_sol =  new G4Box("SiPixel", 5.63125/2. , 1.56594/2., 0.5/2.);
      G4VSolid* Orb_sol =  new G4Orb("orb2", 0.1);
      G4LogicalVolume* Orb_log = new G4LogicalVolume(Orb_sol, fSilicon_Mat, "Orb" );
      G4int OrbId = 0;
      G4ThreeVector PixelCoordinate;
      G4double TranslateX;
      G4double TranslateY;
      G4double TranslateZ;

      ofstream PixelToCoordinateFile("../simulationData/PixelToCoordinate" + fOutFileName + ".txt");
      PixelToCoordinateFile.clear();


  for (G4int i = 0; i < 16; i++)
  {
    x_coordinate = (lampshade_radius) * std::cos(rot_angle_Z);
    y_coordinate = (lampshade_radius)* std::sin(rot_angle_Z);
    if (i > 7) lamp_rot = 180. *deg;
    G4Transform3D transform = G4Rotate3D(lamp_rot , G4ThreeVector(0. , 1., 0.))   
                                * G4Translate3D(G4ThreeVector(x_coordinate, y_coordinate, trgt_dis))
                                * G4Rotate3D(rot_angle_Z + M_PI/2. *rad , G4ThreeVector(0. , 0., 1.))
                                * G4Rotate3D( tilt_angle ,G4ThreeVector(1. , 0.,0.));
      new G4PVPlacement(transform , Trap_expHall_log , "trap_Detectors", expHall_log, false, i, checkOverlaps); // this contains Horizontal / vertical Si strips as well as CsI trap  
      ////////// pixel to coordinate loop ///////////////////
    for (G4int j = 0; j < fHstripNo; j++)
    {
          for (G4int k = 0; k < fVstripNo; k++)
            {  
              // if ( (i == 0) && (j == 0) && (k == 0)) PixelToCoordinateFile << "#PixelID PanelID TopID BottomID X Y Z" << endl;
              G4String PixelTest = PixelCheck(k, j, 0, fHstripNo, fVstripNo, fStrip_gap , base_width, top_width,  trap_hight, fStrip_thickness, fLastHStripMagConstant);    
                PixelCoordinate = FAUSTPixelToCoordinates(k, j, 0, fHstripNo, fVstripNo, fStrip_gap , base_width, top_width,  trap_hight, fStrip_thickness, fLastHStripMagConstant);    
                // to void including out of boundary pixels //
                if (PixelTest == "FullPixel" || PixelTest == "PartPixel" ){
                PixelCoordinate.setZ(- PCB_Total_thickness/2. + AluminumThickness + fStrip_thickness/2.);
                PixelCoordinate.rotateX(tilt_angle);
                PixelCoordinate.rotateZ(rot_angle_Z + M_PI/2. *rad); 
                PixelCoordinate.setX( PixelCoordinate.x() + x_coordinate);
                PixelCoordinate.setY(PixelCoordinate.y() + y_coordinate);
                PixelCoordinate.setZ(PixelCoordinate.z() + trgt_dis);
                PixelCoordinate.rotateY(lamp_rot);    
                PixelToCoordinateFile << i*100000000 + j*100000 + k << " " << i << " " << j << " " <<  k << " " <<  PixelCoordinate.x() << " " <<  PixelCoordinate.y() << " " <<  PixelCoordinate.z() << endl;
               //  new G4PVPlacement(0 , PixelCoordinate , Orb_log , "ZOrb", expHall_log, false, OrbId, false);   
                }
           }
     }

 //////////////////// end of pixel to coordinate loop //////////////////////////

    rot_angle_Z += M_PI/4. *rad;
  }


    if (fAnnular_det == "S3")
    {
  // S3 annular Detector
       // Mother volume for the rings
       G4VSolid* Disk_expHall = new G4Tubs("Disk_ExperimentalHall", 15.*mm, 35.*mm, fS3_thickness/2., 0., 360 *deg);
       G4LogicalVolume* Disk_expHall_log = new G4LogicalVolume(Disk_expHall, fSilicon_Mat, "Disk_ExperimentalHall");
       new G4PVPlacement(0, G4ThreeVector(0.,0., fS3_target_dist), Disk_expHall_log, "S3InterStripGap", expHall_log, 0, 16, checkOverlaps);
       new G4PVPlacement(0, G4ThreeVector(0.,0., -fS3_target_dist - 85.), Disk_expHall_log, "S3InterStripGap", expHall_log, 0, 17, checkOverlaps);
       // Mother volume for the aluminum contact
 //      G4VSolid* DiskAl_expHall = new G4Tubs("DiskAl_ExperimentalHall", 11.*mm, 35.*mm, AluminumThickness/2., 0., 360 *deg);
 //      G4LogicalVolume* DiskAl_expHall_log = new G4LogicalVolume(DiskAl_expHall, fVacuum, "DiskAl_ExperimentalHall");
 //      new G4PVPlacement(0, G4ThreeVector(0.,0., fS3_target_dist + fS3_thickness/2. + AluminumThickness/2.), DiskAl_expHall_log, "S3Al", expHall_log, 0, 18, checkOverlaps);
 //      new G4PVPlacement(0, G4ThreeVector(0.,0., -fS3_target_dist - fS3_thickness/2. - AluminumThickness/2.), DiskAl_expHall_log, "S3Al", expHall_log, 0, 19, checkOverlaps);
 //      new G4PVPlacement(0, G4ThreeVector(0.,0., fS3_target_dist - fS3_thickness/2. - AluminumThickness/2.), DiskAl_expHall_log, "S3Al", expHall_log, 0, 20, checkOverlaps);
 //      new G4PVPlacement(0, G4ThreeVector(0.,0., -fS3_target_dist + fS3_thickness/2. + AluminumThickness/2.), DiskAl_expHall_log, "S3Al", expHall_log, 0, 21, checkOverlaps);

       // S3 construction
       G4double inner_radius_StartZ = 15. *mm;
       G4double outer_radius_EndZ = 35. *mm;
                 pRMin =  22. *mm;  // Placeholder variable (will be changed by FAUST_rings_Parameterisation())
                 pRMax = 23. *mm;   // Placeholder variable (will be changed by FAUST_rings_Parameterisation())
                 pDz = fS3_thickness;   // thickness of the rings
                 pSPhi = 0. ;
                 pDPhi = 2. * M_PI;
       G4VSolid* S3_rings_sol = new G4Tubs("S3_rings_sol",inner_radius_StartZ/2. ,outer_radius_EndZ/2.  ,pDz/2  ,pSPhi ,pDPhi);
       G4LogicalVolume* S3_rings_log = new G4LogicalVolume(S3_rings_sol, fSilicon_Mat, "S3_ring");
       G4VPVParameterisation* S3_ringsParam = new FAUST_S3rings_Parameterisation(fS3ringNo, inner_radius_StartZ , outer_radius_EndZ, fRings_gap, true);
       new G4PVParameterised("S3_ring", S3_rings_log, Disk_expHall_log, kZAxis, 384, S3_ringsParam); 
       G4VisAttributes* Si_rings_vis = new G4VisAttributes(G4Colour(1., 1., 0.));
       Si_rings_vis->SetDaughtersInvisible(false);
       // Si_rings_vis->SetForceSolid(true);
       S3_rings_log->SetVisAttributes(Si_rings_vis); 
       S3_rings_log->SetUserLimits(fStepLimit);

      
        // S3 aluminum contact lead
//    G4double pRMin_Al =  11. *mm;   // inner radius of Al contact
//    G4double pRMax_Al = 12. *mm;   // outer radius of Al contact
//   G4double pDz_Al = AluminumThickness;   //  thickness of the Al contact
//    G4double pSPhi_Al = 0. ;
//    G4double pDPhi_Al = 2. * M_PI;
//    G4VSolid* S3_Al_sol = new G4Tubs("S3_Al_sol", pRMin_Al , pRMax_Al, pDz_Al/2  ,pSPhi_Al ,pDPhi_Al);
//    G4LogicalVolume* S3_Al_log = new G4LogicalVolume(S3_Al_sol, fAluminum_Mat, "S3_Al");
//    G4VPVParameterisation* S3_AlParam = new FAUST_S3rings_Parameterisation(fS3ringNo, inner_radius_StartZ , outer_radius_EndZ, fRings_gap, false);
//    new G4PVParameterised("S3_Al", S3_Al_log, DiskAl_expHall_log, kZAxis, 24, S3_AlParam); 
//    G4VisAttributes* S3_Al_vis = new G4VisAttributes(G4Colour(1., 0.5, 0.));
//    S3_Al_vis->SetDaughtersInvisible(false);
//    S3_Al_vis->SetForceSolid(true);
//    S3_Al_log->SetVisAttributes(S3_Al_vis);
//    S3_Al_log->SetUserLimits(fStepLimit);


      
      
        

    //
       Disk_expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());
    //
     PixelID = 0;
     // pixel to coordinates test
     for (G4int i = 16; i < 18; i++)
     {
         G4double Z = fS3_target_dist;
          if (i == 17) Z = -fS3_target_dist - 85;
         for (G4int j = 0; j < 24; j++)
         {
           G4double PixelRho = FAUSTPixelToCoordinatesS3R(j, 24 ,inner_radius_StartZ, outer_radius_EndZ, fRings_gap);
           for (G4int k = 0; k < 16; k++)
           {
             G4double PixelPhi = FAUSTPixelToCoordinatesS3Phi(k, 16, inner_radius_StartZ, fRings_gap);
             G4double X = PixelRho * std::cos(PixelPhi);
             G4double Y = PixelRho * std::sin(PixelPhi);
    //
                 // For visualization 
             PixelCoordinate = G4ThreeVector(X, Y, Z);
             // new G4PVPlacement(0 , PixelCoordinate , Orb_log , "ZOrb", expHall_log, false, OrbId, true);
             PixelToCoordinateFile << i*100000000 + j*100000 + k << " " << i << " " << j << " " <<  k << " " << X << " " <<  Y << " " <<  Z << endl;
             PixelID++;
           }
         }
     }
    //
  //    // CsI disk construction
  //             pRMin =  12.5 *mm;   // inner radius of CsI disk
  //             pRMax = 45. *mm;   // outer radius of CsI disk
  //             pDz = fCsI_thickness;   //  thickness of the CsI
  //             pSPhi = 0. ;
  //             pDPhi = 2. * M_PI;
  //   G4VSolid* CsI_rings_sol = new G4Tubs("CsI_rings", pRMin , pRMax, pDz/2  ,pSPhi ,pDPhi);
  //   G4LogicalVolume* CsI_rings_log = new G4LogicalVolume(CsI_rings_sol, fCsI_Mat, "CsI_rings");
  //   new G4PVPlacement(0, G4ThreeVector(0.,0., fS3_target_dist + fS3_thickness/2. + MylarThickness + AluminumThickness + pDz/2.), CsI_rings_log, "CsI_rings", expHall_log, 0, 16, checkOverlaps);
  //   new G4PVPlacement(0, G4ThreeVector(0.,0., -fS3_target_dist - fS3_thickness/2. - MylarThickness - AluminumThickness - pDz/2.), CsI_rings_log, "CsI_rings", expHall_log, 0, 17, checkOverlaps);
  //  CsI_rings_log->SetVisAttributes(CsI_Detector_vis); 
  //  CsI_rings_log->SetUserLimits(fStepLimit);

      //CsI disk mylar film
 //    G4VSolid* CsI_Mylar_sol = new G4Tubs("CsI_Mylar", pRMin , pRMax, MylarThickness/2.  ,pSPhi ,pDPhi);
 //    G4LogicalVolume* CsI_Mylar_log = new G4LogicalVolume(CsI_Mylar_sol, fMylar_Mat, "CsI_Mylar");
 //    new G4PVPlacement(0, G4ThreeVector(0.,0., fS3_target_dist + fS3_thickness/2. + AluminumThickness + MylarThickness/2.), CsI_Mylar_log, "CsI_Mylar", expHall_log, 0, 16, checkOverlaps);
 //    new G4PVPlacement(0, G4ThreeVector(0.,0., -fS3_target_dist - fS3_thickness/2. - AluminumThickness - MylarThickness/2.), CsI_Mylar_log, "CsI_Mylar", expHall_log, 0, 17, checkOverlaps);
 //    CsI_Mylar_log->SetVisAttributes(MylarFilm_vis);
 //    CsI_Mylar_log->SetUserLimits(fStepLimit);
    }





    if (fAnnular_det == "S3")
    {
      // S1 annular Detector
      G4VSolid* S1_expHall = new G4Tubs("Disk_ExperimentalHall", 22.*mm, 48.*mm, fS3_thickness/2., 0., 90. *deg); // 0.25 is the thickness of one layer
      G4LogicalVolume* S1_expHall_log = new G4LogicalVolume(S1_expHall, fSilicon_Mat, "Disk_ExperimentalHall");
         G4int CopyNo_ = 0; // used to distinguish between S1 disks on the positive Z-axis from the negative Z-axis

       G4double fS1_target_dist = -fS3_target_dist;
//
        G4Transform3D transform = G4Translate3D(G4ThreeVector(0., 0. , fS1_target_dist));
        new G4PVPlacement(transform, S1_expHall_log, "S1_Disk", expHall_log, 0, 18, checkOverlaps);
        transform =  G4Translate3D(G4ThreeVector(0., 0. , fS1_target_dist)) * G4Rotate3D(180 *deg , G4ThreeVector(0. , 1., 0.));
       new G4PVPlacement(transform, S1_expHall_log, "S1_Disk", expHall_log, 0, 19, checkOverlaps);
       transform =  G4Translate3D(G4ThreeVector(0., 0. , fS1_target_dist)) * G4Rotate3D(180 *deg , G4ThreeVector(1. , 0., 0.));
       new G4PVPlacement(transform, S1_expHall_log, "S1_Disk", expHall_log, 0, 20, checkOverlaps);
       transform = G4Translate3D(G4ThreeVector(0., 0. , fS1_target_dist)) * G4Rotate3D(180 *deg , G4ThreeVector(1. , 0., 0.)) * G4Rotate3D(180 *deg , G4ThreeVector(0. , 1., 0.));
       new G4PVPlacement(transform, S1_expHall_log, "S1_Disk", expHall_log, 0, 21, checkOverlaps);
      
        // S1 construction
      std::vector<G4LogicalVolume*> S1_log;
      G4VisAttributes* S1_vis = new G4VisAttributes(G4Colour(1., 0., 0.));
      S1_vis->SetDaughtersInvisible(false);
      //S1_vis->SetForceSolid(true);
      VectorIndex = 0;
      for (G4int i = 0; i < 16; i++) // Top strips
      {
        for (G4int j = 0; j < 4; j++) // Bottom Strips
        {
          if (i == 15 && j == 3) break;
          G4VSolid* S1_sol = S1annaularD(i, j); // (i, number of incomplete rings/4, inner R, outer R, inter-strip dead layer) 
          S1_log.push_back(new G4LogicalVolume(S1_sol, fSilicon_Mat, "S1_detector"));
          new G4PVPlacement(G4Transform3D(), S1_log.at(VectorIndex), "S1_detector", S1_expHall_log, false, i*100 + j, checkOverlaps);
          S1_log.at(VectorIndex)->SetVisAttributes(S1_vis);
          VectorIndex++;
          
          
        }
      }


           // pixel to coordinates test
     for (G4int i = 18; i < 22; i++)
     {
         G4double Z = fS1_target_dist;
         G4cout << "S1 Z: " << Z << G4endl;
         for (G4int j = 0; j < 16; j++)
         {
           G4double PixelRho = FAUSTPixelToCoordinatesS1R(j);
           for (G4int k = 0; k < 4; k++)
           {
             if (j == 15 && k == 3) break;

             G4double PixelPhi = FAUSTPixelToCoordinatesS1Phi(j, k);
             G4double X = PixelRho * std::cos(PixelPhi);
             G4double Y = PixelRho * std::sin(PixelPhi);
    //
                 // For visualization 
             PixelCoordinate = G4ThreeVector(X, Y, Z);
             if (i== 19) PixelCoordinate.rotateY(180. *deg);
             if (i== 20) PixelCoordinate.rotateX(180. *deg);
             if (i== 21) PixelCoordinate.rotateX(180. *deg).rotateY(180. *deg);
            //  new G4PVPlacement(0 , PixelCoordinate , Orb_log , "ZOrb", expHall_log, false, OrbId, true);
             PixelToCoordinateFile << i*100000000 + j*100000 + k << " " << i << " " << j << " " <<  k << " " << X << " " <<  Y << " " <<  Z << endl;
             PixelID++;
           }
         }


     }
     //   S1_expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());
    }



      

 







  // For the visualization
  expHall_log->SetVisAttributes(G4VisAttributes::GetInvisible());



  G4VisAttributes* Si_stripH_vis = new G4VisAttributes(G4Colour(0., 1., 0.));
  Si_stripH_vis->SetDaughtersInvisible(false);
  Si_stripH_vis->SetForceSolid(true);
  // Si_stripH_log->SetVisAttributes(Si_stripH_vis);
  Orb_log->SetVisAttributes(Si_stripH_vis);




  G4VisAttributes* PCB_vis = new G4VisAttributes(G4Colour(1., 1., 0.));
  PCB_vis->SetDaughtersInvisible(false);
  PCB_vis->SetForceSolid(true);
  // Trap_expHall_log->SetVisAttributes(PCB_vis);
  // Si_Detector_log->SetVisAttributes(PCB_vis);


  return physExpHall;
}




