#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "globals.hh"

namespace B2a { // Bao namespace nếu cần

    // Lớp DetectorConstruction kế thừa từ G4VUserDetectorConstruction
    class DetectorConstruction : public G4VUserDetectorConstruction {
    public:
        DetectorConstruction();  // Constructor
        virtual ~DetectorConstruction(); // Destructor

        virtual G4VPhysicalVolume* Construct(); // Hàm xây dựng hình học

        // Thêm các hàm getter để truy cập các thuộc tính private (nếu cần)
        G4LogicalVolume* GetLogicSi() const { return fLogicSi; }
        G4LogicalVolume* GetLogicCZT() const { return fLogicCZT; }

        // Setter (nếu cần thêm logic thiết lập)
        void SetCheckOverlaps(G4bool value) { fCheckOverlaps = value; }
        void SetMaxStep(G4double maxStep); // Chỉ khai báo, không định nghĩa

    private:
        void DefineMaterials(); // Hàm định nghĩa vật liệu

        G4Material* fSiMaterial;  // Vật liệu Silicon
        G4Material* fCZTMaterial; // Vật liệu Cadmium Zinc Telluride
        G4LogicalVolume* fLogicSi; // Logical Volume của Si
        G4LogicalVolume* fLogicCZT; // Logical Volume của CZT
        G4bool fCheckOverlaps = true; // Cờ kiểm tra chồng lấn, mặc định là true

        G4double fMaxStep;  // Biến lưu trữ giá trị bước tối đa
    };

} // namespace B2a

#endif // DETECTOR_CONSTRUCTION_HH
