#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIdirectory.hh"

namespace B2a
{

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DetectorMessenger::DetectorMessenger(DetectorConstruction* det) : fDetectorConstruction(det)
    {
        // T?o th? m?c l?nh cho UI
        fDirectory = new G4UIdirectory("/B2/");
        fDirectory->SetGuidance("UI commands specific to this example.");

        fDetDirectory = new G4UIdirectory("/B2/det/");
        fDetDirectory->SetGuidance("Detector construction control");

        // L?nh thay ??i v?t li?u m?c tiêu
        fTargMatCmd = new G4UIcmdWithAString("/B2/det/setTargetMaterial", this);
        fTargMatCmd->SetGuidance("Select Material of the Target.");
        fTargMatCmd->SetParameterName("choice", false);
        fTargMatCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        // L?nh thay ??i v?t li?u phòng thí nghi?m
        fChamMatCmd = new G4UIcmdWithAString("/B2/det/setChamberMaterial", this);
        fChamMatCmd->SetGuidance("Select Material of the Chamber.");
        fChamMatCmd->SetParameterName("choice", false);
        fChamMatCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

        // L?nh thay ??i b??c t?i ?a
        fStepMaxCmd = new G4UIcmdWithADoubleAndUnit("/B2/det/stepMax", this);
        fStepMaxCmd->SetGuidance("Define a step max");
        fStepMaxCmd->SetParameterName("stepMax", false);
        fStepMaxCmd->SetUnitCategory("Length");
        fStepMaxCmd->AvailableForStates(G4State_Idle);
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    DetectorMessenger::~DetectorMessenger()
    {
        // Xóa các ??i t??ng l?nh UI khi không c?n thi?t
        delete fTargMatCmd;
        delete fChamMatCmd;
        delete fStepMaxCmd;
        delete fDirectory;
        delete fDetDirectory;
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

    void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
    {
        // N?u ng??i dùng g?i l?nh setTargetMaterial, c?p nh?t v?t li?u c?a m?c tiêu
        if (command == fTargMatCmd) {
            fDetectorConstruction->SetTargetMaterial(newValue);
        }

        // N?u ng??i dùng g?i l?nh setChamberMaterial, c?p nh?t v?t li?u c?a phòng thí nghi?m
        if (command == fChamMatCmd) {
            fDetectorConstruction->SetChamberMaterial(newValue);
        }

        // N?u ng??i dùng g?i l?nh setMaxStep, c?p nh?t giá tr? b??c t?i ?a
        if (command == fStepMaxCmd) {
            fDetectorConstruction->SetMaxStep(fStepMaxCmd->GetNewDoubleValue(newValue));
        }
    }

    //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B2a
