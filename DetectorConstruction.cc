#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4ThreeVector.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GeometryManager.hh"
#include "G4GeometryTolerance.hh"

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nistManager = G4NistManager::Instance();

    // ??nh ngh?a v?t li?u Silicon (Si) và CZT
    fSiMaterial = nistManager->FindOrBuildMaterial("G4_Si");
    fCZTMaterial = nistManager->FindOrBuildMaterial("G4_CdZnTe");  // V?t li?u CZT

    G4cout << "Materials defined: Si and CZT" << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
    G4Material* air = G4Material::GetMaterial("G4_AIR");

    // Kích th??c c?a các kh?i h?p
    G4double SiBoxWidth = 57.6 * mm;
    G4double SiBoxHeight = 57.6 * mm;
    G4double SiBoxLength = 2.0 * mm;

    G4double CZTBoxWidth = 100.0 * mm;
    G4double CZTBoxHeight = 100.0 * mm;
    G4double CZTBoxLength = 10.0 * mm;

    // Kích th??c c?a World Volume
    G4double worldLength = 2 * (CZTBoxLength + SiBoxLength + 50.0 * mm); // C?ng thêm kho?ng cách 5 cm

    // World Volume
    auto worldS = new G4Box("world", worldLength / 2, worldLength / 2, worldLength / 2);
    auto worldLV = new G4LogicalVolume(worldS, air, "World");
    auto worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0, fCheckOverlaps);

    // Kh?i h?p Si: ?i?u ch?nh v? trí c?a Si ?? có kho?ng cách 5 cm so v?i CZT
    G4ThreeVector positionSi = G4ThreeVector(0, 0, -(SiBoxLength / 2 + 50.0 * mm + CZTBoxLength / 2)); // 50 mm là 5 cm
    auto SiBox = new G4Box("SiBox", SiBoxWidth / 2, SiBoxHeight / 2, SiBoxLength / 2);
    fLogicSi = new G4LogicalVolume(SiBox, fSiMaterial, "SiBox");
    new G4PVPlacement(nullptr, positionSi, fLogicSi, "SiBox", worldLV, false, 0, fCheckOverlaps);

    // Kh?i h?p CZT: V? trí c?a CZT không thay ??i
    G4ThreeVector positionCZT = G4ThreeVector(0, 0, 0); // CZT v?n ? v? trí g?c
    auto CZTBox = new G4Box("CZTBox", CZTBoxWidth / 2, CZTBoxHeight / 2, CZTBoxLength / 2);
    fLogicCZT = new G4LogicalVolume(CZTBox, fCZTMaterial, "CZTBox");
    new G4PVPlacement(nullptr, positionCZT, fLogicCZT, "CZTBox", worldLV, false, 0, fCheckOverlaps);

    // Thi?t l?p các thu?c tính hi?n th? (Visualization)
    G4VisAttributes siVisAtt(G4Colour::Green());
    fLogicSi->SetVisAttributes(siVisAtt);

    G4VisAttributes cztVisAtt(G4Colour::Blue());
    fLogicCZT->SetVisAttributes(cztVisAtt);

    // Tr? v? world volume
    return worldPV;
}
