#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"  // Đảm bảo rằng bạn đã bao gồm thư viện này để sử dụng các đơn vị như mm

void DetectorConstruction::DefineMaterials()
{
    G4NistManager* nistManager = G4NistManager::Instance();

    // Định nghĩa vật liệu Silicon (Si) và CZT
    fSiMaterial = nistManager->FindOrBuildMaterial("G4_Si");
    fCZTMaterial = nistManager->FindOrBuildMaterial("G4_CdZnTe");

    G4cout << "Materials defined: Si and CZT" << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
    G4Material* air = G4Material::GetMaterial("G4_AIR");

    // Kích thước của các khối hộp
    G4double SiBoxWidth = 57.6 * mm;
    G4double SiBoxHeight = 57.6 * mm;
    G4double SiBoxLength = 2.0 * mm;

    G4double CZTBoxWidth = 100.0 * mm;
    G4double CZTBoxHeight = 100.0 * mm;
    G4double CZTBoxLength = 10.0 * mm;

    // Kích thước của World Volume
    G4double worldLength = 2 * (CZTBoxLength + SiBoxLength + 50.0 * mm);

    // World Volume
    auto worldS = new G4Box("world", worldLength / 2, worldLength / 2, worldLength / 2);
    auto worldLV = new G4LogicalVolume(worldS, air, "World");
    auto worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0, fCheckOverlaps);

    // Khối hộp Si
    G4ThreeVector positionSi = G4ThreeVector(0, 0, -(SiBoxLength / 2 + 50.0 * mm + CZTBoxLength / 2));
    auto SiBox = new G4Box("SiBox", SiBoxWidth / 2, SiBoxHeight / 2, SiBoxLength / 2);
    fLogicSi = new G4LogicalVolume(SiBox, fSiMaterial, "SiBox");
    new G4PVPlacement(nullptr, positionSi, fLogicSi, "SiBox", worldLV, false, 0, fCheckOverlaps);

    // Khối hộp CZT
    G4ThreeVector positionCZT = G4ThreeVector(0, 0, 0);
    auto CZTBox = new G4Box("CZTBox", CZTBoxWidth / 2, CZTBoxHeight / 2, CZTBoxLength / 2);
    fLogicCZT = new G4LogicalVolume(CZTBox, fCZTMaterial, "CZTBox");
    new G4PVPlacement(nullptr, positionCZT, fLogicCZT, "CZTBox", worldLV, false, 0, fCheckOverlaps);

    // Thiết lập các thuộc tính hiển thị (Visualization)
    G4VisAttributes siVisAtt(G4Colour::Green());
    fLogicSi->SetVisAttributes(siVisAtt);

    G4VisAttributes cztVisAtt(G4Colour::Blue());
    fLogicCZT->SetVisAttributes(cztVisAtt);

    // Trả về world volume
    return worldPV;
}

DetectorConstruction::DetectorConstruction()
    : fCheckOverlaps(true)
{
}
