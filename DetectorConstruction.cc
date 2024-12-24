#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "TrackerSD.hh"

#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4GeometryManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

using namespace B2;

namespace B2a {

    G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

    DetectorConstruction::DetectorConstruction()
    {
        fMessenger = new DetectorMessenger(this);

        fNbOfChambers = 5;
        fLogicChamber = new G4LogicalVolume * [fNbOfChambers];
    }

    DetectorConstruction::~DetectorConstruction()
    {
        delete[] fLogicChamber;
        delete fStepLimit;
        delete fMessenger;
    }

    G4VPhysicalVolume* DetectorConstruction::Construct()
    {
        // Define materials
        DefineMaterials();

        // Define volumes
        return DefineVolumes();
    }

    void DetectorConstruction::DefineMaterials()
    {
        G4NistManager* nistManager = G4NistManager::Instance();

        // Define materials using NIST Manager
        nistManager->FindOrBuildMaterial("G4_AIR");
        fTargetMaterial = nistManager->FindOrBuildMaterial("G4_Pb");
        fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Si");

        // Print materials
        G4cout << *(G4Material::GetMaterialTable()) << G4endl;
    }

    G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
    {
        G4Material* air = G4Material::GetMaterial("G4_AIR");

        // Define geometrical components (solids)
        G4double chamberSpacing = 80 * cm;
        G4double chamberWidth = 20.0 * cm;
        G4double targetLength = 5.0 * cm;
        G4double trackerLength = (fNbOfChambers + 1) * chamberSpacing;
        G4double worldLength = 1.2 * (2 * targetLength + trackerLength);
        G4double targetRadius = 0.5 * targetLength;
        targetLength = 0.5 * targetLength;
        G4double trackerSize = 0.5 * trackerLength;

        // World
        G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);
        auto worldS = new G4Box("world", worldLength / 2, worldLength / 2, worldLength / 2);
        auto worldLV = new G4LogicalVolume(worldS, air, "World");
        auto worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0, fCheckOverlaps);

        // Target
        G4ThreeVector positionTarget = G4ThreeVector(0, 0, -(targetLength + trackerSize));
        auto targetS = new G4Tubs("target", 0., targetRadius, targetLength, 0. * deg, 360. * deg);
        fLogicTarget = new G4LogicalVolume(targetS, fTargetMaterial, "Target");
        new G4PVPlacement(nullptr, positionTarget, fLogicTarget, "Target", worldLV, false, 0, fCheckOverlaps);

        // Tracker
        G4ThreeVector positionTracker = G4ThreeVector(0, 0, 0);
        auto trackerS = new G4Tubs("tracker", 0, trackerSize, trackerSize, 0. * deg, 360. * deg);
        auto trackerLV = new G4LogicalVolume(trackerS, air, "Tracker");
        new G4PVPlacement(nullptr, positionTracker, trackerLV, "Tracker", worldLV, false, 0, fCheckOverlaps);

        // Visualization attributes
        G4VisAttributes boxVisAtt(G4Colour::White());
        G4VisAttributes chamberVisAtt(G4Colour::Yellow());
        worldLV->SetVisAttributes(boxVisAtt);
        fLogicTarget->SetVisAttributes(boxVisAtt);
        trackerLV->SetVisAttributes(boxVisAtt);

        // Tracker segments
        G4cout << "There are " << fNbOfChambers << " chambers in the tracker region. " << G4endl
            << "The chambers are " << chamberWidth / cm << " cm of " << fChamberMaterial->GetName()
            << G4endl << "The distance between chambers is " << chamberSpacing / cm << " cm" << G4endl;

        G4double firstPosition = -trackerSize + chamberSpacing;
        G4double rmaxFirst = 0.5 * (trackerLength / 10);
        G4double rmaxIncr = 0.0;
        if (fNbOfChambers > 0) {
            rmaxIncr = 0.5 * (trackerLength - firstPosition) / (fNbOfChambers - 1);
            if (chamberSpacing < chamberWidth) {
                G4Exception("DetectorConstruction::DefineVolumes()", "InvalidSetup", FatalException, "Width>Spacing");
            }
        }

        for (G4int copyNo = 0; copyNo < fNbOfChambers; copyNo++) {
            G4double Zposition = firstPosition + copyNo * chamberSpacing;
            G4double rmax = rmaxFirst + copyNo * rmaxIncr;

            // Define chamber as a box
            G4double halfLength = rmax;
            G4double halfWidth = rmax;
            G4double halfHeight = 0.5 * chamberWidth;

            auto chamberS = new G4Box("Chamber_solid", halfLength, halfWidth, halfHeight);
            fLogicChamber[copyNo] = new G4LogicalVolume(chamberS, fChamberMaterial, "Chamber_LV");
            fLogicChamber[copyNo]->SetVisAttributes(chamberVisAtt);
            new G4PVPlacement(nullptr, G4ThreeVector(0, 0, Zposition), fLogicChamber[copyNo], "Chamber_PV", trackerLV, false, copyNo, fCheckOverlaps);
        }

        // Create silicon box
        G4double Sisize = 57.6 * mm;
        G4double Siheight = 2 * mm;
        G4Box* SiBox = new G4Box("SiBox", Sisize / 2, Sisize / 2, Siheight / 2);

        // Create silicon logical volume
        G4LogicalVolume* SiLogical = new G4LogicalVolume(SiBox, fChamberMaterial, "Chamber_LV");
        SiLogical->SetVisAttributes(chamberVisAtt);

        // Place silicon box in world volume
        new G4PVPlacement(0, G4ThreeVector(0, 0, 12 * cm), SiLogical, "Si_PV", trackerLV, false, 0, fCheckOverlaps);

        // Set user limits for max step
        G4double maxStep = 0.5 * chamberWidth;
        fStepLimit = new G4UserLimits(maxStep);
        trackerLV->SetUserLimits(fStepLimit);

        // Always return the physical world
        return worldPV;
    }

    void DetectorConstruction::ConstructSDandField()
    {
        // Sensitive detectors
        G4String trackerChamberSDname = "/TrackerChamberSD";
        auto trackerSD = new TrackerSD(trackerChamberSDname, "TrackerHitsCollection");
        G4SDManager::GetSDMpointer()->AddNewDetector(trackerSD);
        SetSensitiveDetector("Chamber_LV", trackerSD, true);

        // Create global magnetic field messenger
        G4ThreeVector fieldValue = G4ThreeVector();
        fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
        fMagFieldMessenger->SetVerboseLevel(1);
        G4AutoDelete::Register(fMagFieldMessenger);
    }

    void DetectorConstruction::SetTargetMaterial(G4String materialName)
    {
        G4NistManager* nistManager = G4NistManager::Instance();
        G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

        if (fTargetMaterial != pttoMaterial) {
            if (pttoMaterial) {
                fTargetMaterial = pttoMaterial;
                if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
                G4cout << "The target is made of " << materialName << G4endl;
            }
            else {
                G4cout << "WARNING: Material " << materialName << " not found" << G4endl;
            }
        }
    }

    void DetectorConstruction::SetChamberMaterial(G4String materialName)
    {
        G4NistManager* nistManager = G4NistManager::Instance();
        G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

        if (fChamberMaterial != pttoMaterial) {
            if (pttoMaterial) {
                fChamberMaterial = pttoMaterial;
                for (G4int copyNo = 0; copyNo <
