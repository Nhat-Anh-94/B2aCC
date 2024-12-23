#ifndef B2aDetectorConstruction_h
#define B2aDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4Threading.hh"
#include "globals.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

class G4VPhysicalVolume;
class G4Material;
class G4UserLimits;
class G4GlobalMagFieldMessenger;
class DetectorMessenger;

namespace B2a {

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
        void DefineMaterials();
        G4VPhysicalVolume* DefineVolumes();

        static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;

        G4LogicalVolume* fLogicSi = nullptr;
        G4LogicalVolume* fLogicCZT = nullptr;

        G4Material* fSiMaterial = nullptr;
        G4Material* fCZTMaterial = nullptr;

        G4UserLimits* fStepLimit = nullptr;
        G4bool fCheckOverlaps = true;

        DetectorMessenger* fMessenger = nullptr;
    };

}  // namespace B2a

#endif
