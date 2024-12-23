#include "DetectorConstruction.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

// Constructor
DetectorConstruction::DetectorConstruction()
    : G4VUserDetectorConstruction(),
    fSiMaterial(nullptr),
    fCZTMaterial(nullptr),
    fLogicSi(nullptr),
    fLogicCZT(nullptr),
    fCheckOverlaps(true) {
}

// Destructor
DetectorConstruction::~DetectorConstruction() {}

// Hàm định nghĩa vật liệu
void DetectorConstruction::DefineMaterials() {
    auto nistManager = G4NistManager::Instance();
    fSiMaterial = nistManager->FindOrBuildMaterial("G4_Si"); // Silicon
    fCZTMaterial = nistManager->FindOrBuildMaterial("G4_CdZnTe"); // Cadmium Zinc Telluride
}

// Hàm xây dựng hình học
G4VPhysicalVolume* DetectorConstruction::Construct() {
    // Gọi hàm định nghĩa vật liệu
    DefineMaterials();

    // Kích thước của world
    G4double worldSize = 1.0 * m;

    // World Volume
    auto worldS = new G4Box("World", worldSize / 2, worldSize / 2, worldSize / 2);
    auto worldLV = new G4LogicalVolume(worldS, fSiMaterial, "World");
    auto worldPV = new G4PVPlacement(
        nullptr,                  // Không có phép biến đổi
        G4ThreeVector(),          // Vị trí ở gốc tọa độ
        worldLV,                  // Logical Volume
        "World",                  // Tên
        nullptr,                  // Không có mother volume
        false,                    // Không cần pMany
        0,                        // Copy number
        fCheckOverlaps);          // Kiểm tra chồng lấn

    // Detector Silicon Box
    G4double SiBoxWidth = 57.6 * mm;
    auto SiBox = new G4Box("SiBox", SiBoxWidth / 2, SiBoxWidth / 2, SiBoxWidth / 2);
    fLogicSi = new G4LogicalVolume(SiBox, fSiMaterial, "SiBox");

    // Detector CZT Box
    G4double CZTBoxWidth = 10.0 * mm;
    auto CZTBox = new G4Box("CZTBox", CZTBoxWidth / 2, CZTBoxWidth / 2, CZTBoxWidth / 2);
    fLogicCZT = new G4LogicalVolume(CZTBox, fCZTMaterial, "CZTBox");

    return worldPV; // Trả về world volume
}
