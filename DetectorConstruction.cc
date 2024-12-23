#include "DetectorConstruction.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

// Đặt lớp vào namespace B2a
namespace B2a {

    // Constructor
    DetectorConstruction::DetectorConstruction()
        : G4VUserDetectorConstruction(),
        fSiMaterial(nullptr),
        fCZTMaterial(nullptr),
        fLogicSi(nullptr),
        fLogicCZT(nullptr),
        fMaxStep(1.0 * mm),  // Giá trị mặc định cho bước tối đa
        fCheckOverlaps(true) {
    }

    // Destructor
    DetectorConstruction::~DetectorConstruction() {}

    // Hàm định nghĩa vật liệu
    void DetectorConstruction::DefineMaterials() {
        auto nistManager = G4NistManager::Instance();

        // Kiểm tra và định nghĩa vật liệu Silicon
        fSiMaterial = nistManager->FindOrBuildMaterial("G4_Si");
        if (!fSiMaterial) {
            G4cerr << "Error: Material G4_Si not found!" << G4endl;
            return;
        }

        // Kiểm tra và định nghĩa vật liệu CZT
        fCZTMaterial = nistManager->FindOrBuildMaterial("G4_CdZnTe");
        if (!fCZTMaterial) {
            G4cerr << "Error: Material G4_CdZnTe not found!" << G4endl;
            return;
        }
    }

    // Hàm xây dựng hình học
    G4VPhysicalVolume* DetectorConstruction::Construct() {
        // Gọi hàm định nghĩa vật liệu
        DefineMaterials();
        if (!fSiMaterial || !fCZTMaterial) {
            G4cerr << "Error: Materials not defined properly!" << G4endl;
            return nullptr;
        }

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

        // Đặt Silicon Box trong World
        new G4PVPlacement(
            nullptr,                  // Không có phép biến đổi
            G4ThreeVector(0, 0, 0),   // Vị trí
            fLogicSi,                 // Logical Volume
            "SiBox",                  // Tên
            worldLV,                  // Mother volume
            false,                    // Không cần pMany
            1,                        // Copy number
            fCheckOverlaps);          // Kiểm tra chồng lấn

        // Detector CZT Box
        G4double CZTBoxWidth = 10.0 * mm;
        auto CZTBox = new G4Box("CZTBox", CZTBoxWidth / 2, CZTBoxWidth / 2, CZTBoxWidth / 2);
        fLogicCZT = new G4LogicalVolume(CZTBox, fCZTMaterial, "CZTBox");

        // Đặt CZT Box trong World
        new G4PVPlacement(
            nullptr,                  // Không có phép biến đổi
            G4ThreeVector(0, 0, 30 * mm), // Đặt cách Silicon Box 30 mm
            fLogicCZT,                // Logical Volume
            "CZTBox",                 // Tên
            worldLV,                  // Mother volume
            false,                    // Không cần pMany
            2,                        // Copy number
            fCheckOverlaps);          // Kiểm tra chồng lấn

        return worldPV; // Trả về world volume
    }

    // Setter cho MaxStep
    void DetectorConstruction::SetMaxStep(G4double maxStep) {
        fMaxStep = maxStep;
    }

} // namespace B2a
