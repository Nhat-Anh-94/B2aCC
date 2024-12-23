#ifndef DETECTOR_CONSTRUCTION_HH
#define DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "globals.hh"

// Lớp DetectorConstruction kế thừa từ G4VUserDetectorConstruction
class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();  // Constructor
    virtual ~DetectorConstruction(); // Destructor

    virtual G4VPhysicalVolume* Construct(); // Hàm xây dựng hình học

private:
    void DefineMaterials(); // Hàm định nghĩa vật liệu

    G4Material* fSiMaterial;  // Vật liệu Silicon
    G4Material* fCZTMaterial; // Vật liệu Cadmium Zinc Telluride
    G4LogicalVolume* fLogicSi; // Logical Volume của Si
    G4LogicalVolume* fLogicCZT; // Logical Volume của CZT
    G4bool fCheckOverlaps; // Cờ kiểm tra chồng lấn
};

#endif
