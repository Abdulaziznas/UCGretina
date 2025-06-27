#ifndef FAUSTPixelToCoordinates_H
#define FAUSTPixelToCoordinates_H 1


#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

G4ThreeVector FAUSTPixelToCoordinates(G4int VstripID, G4int HstripID, G4int PanelID, G4int NoOfHstrips, G4int NoOfVstrips,
                             G4double SpacingZ , G4double base_width, G4double top_width, G4double trap_hight, G4double strip_thickness, G4double LastHStripMagConstant);
                             
G4double FAUSTPixelToCoordinatesS3R(G4int RingId, G4int NoOfRings, G4double InnerRadius , G4double OuterRadius, G4double RingGap);
G4double FAUSTPixelToCoordinatesS3Phi(G4int PanelId, G4int NoOfPanels, G4double InnerRadius, G4double RingGap);

G4String PixelCheck(G4int VstripID, G4int HstripID, G4int PanelID, G4int NoOfHstrips, G4int NoOfVstrips,
                             G4double InterStripGap , G4double base_width, G4double top_width, G4double trap_hight, G4double strip_thickness, G4double LastHStripMagConstant);

G4double FAUSTPixelToCoordinatesS1R(G4int RingId);
G4double FAUSTPixelToCoordinatesS1Phi(G4int RingId, G4int PanelId);

#endif