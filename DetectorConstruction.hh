#ifndef B2aDetectorConstruction_h
#define B2aDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4Threading.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4UserLimits;
class G4GlobalMagFieldMessenger;

namespace B2a
{

    class DetectorMessenger;

    /// Detector construction class to define materials, geometry
    /// and global uniform magnetic field.

    class DetectorConstruction : public G4VUserDetectorConstruction
    {
    public:
        DetectorConstruction();
        ~DetectorConstruction() override;

    public:
        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

        // Set methods
        void SetTargetMaterial(G4String);
        void SetChamberMaterial(G4String);
        void SetMaxStep(G4double);
        void SetCheckOverlaps(G4bool);

    private:
        // methods
        void DefineMaterials();
        G4VPhysicalVolume* DefineVolumes();

        // static data members
        static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;
        // magnetic field messenger
        // data members
        G4int fNbOfChambers = 0;

        G4LogicalVolume* fLogicTarget = nullptr;  // pointer to the logical Target
        G4LogicalVolume** fLogicChamber = nullptr;  // pointer to the logical Chamber

        G4Material* fTargetMaterial = nullptr;  // pointer to the target  material
        G4Material* fChamberMaterial = nullptr;  // pointer to the chamber material

        G4UserLimits* fStepLimit = nullptr;  // pointer to user step limits

        DetectorMessenger* fMessenger = nullptr;  // messenger

        G4bool fCheckOverlaps = true;  // option to activate checking of volumes overlaps

        // materials
        G4Material* fSiMaterial = nullptr;
        G4Material* fCZTMaterial = nullptr;

        // logical volumes
        G4LogicalVolume* fLogicSi = nullptr;
        G4LogicalVolume* fLogicCZT = nullptr;
    };

}  // namespace B2a

#endif
