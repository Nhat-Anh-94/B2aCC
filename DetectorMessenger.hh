#ifndef B2aDetectorMessenger_h
#define B2aDetectorMessenger_h 1

#include "G4UImessenger.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcommand;

namespace B2a
{

    class DetectorConstruction;

    /// Messenger class that defines commands for DetectorConstruction.
    ///
    /// It implements commands:
    /// - /B2/det/setTargetMaterial name
    /// - /B2/det/setChamberMaterial name
    /// - /B2/det/stepMax value unit

    class DetectorMessenger : public G4UImessenger
    {
    public:
        DetectorMessenger(DetectorConstruction*);
        ~DetectorMessenger() override;

        void SetNewValue(G4UIcommand*, G4String) override;

    private:
        DetectorConstruction* fDetectorConstruction = nullptr;

        G4UIdirectory* fDirectory = nullptr;
        G4UIdirectory* fDetDirectory = nullptr;

        G4UIcmdWithAString* fTargMatCmd = nullptr;  // Command to set target material
        G4UIcmdWithAString* fChamMatCmd = nullptr;  // Command to set chamber material
        G4UIcmdWithADoubleAndUnit* fStepMaxCmd = nullptr; // Command to set step max

        // Các hàm setter b? sung (n?u c?n) trong này c?ng có th? ???c khai báo
    };

}  // namespace B2a

#endif
