#ifndef TrapPixel_H
#define TrapPixel_H 1
#include "G4ExtrudedSolid.hh"
#include "G4Box.hh"


G4ExtrudedSolid* TrapPixel(G4int VstripID, G4int HstripID, G4int PanelID, 
                    G4int NoOfHstrips, G4int NoOfVstrips,
                    G4double InterStripGap , G4double base_width, 
                    G4double top_width, G4double trap_hight,
                    G4double strip_thickness, G4double LastHStripMagConstant);
#endif