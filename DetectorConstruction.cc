﻿//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B2/B2a/src/DetectorConstruction.cc
/// \brief Implementation of the B2a::DetectorConstruction class

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

namespace B2a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

DetectorConstruction::DetectorConstruction()
{
  fMessenger = new DetectorMessenger(this);

  fNbOfChambers = 5;
  fLogicChamber = new G4LogicalVolume*[fNbOfChambers];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete[] fLogicChamber;
  delete fStepLimit;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Material definition

  G4NistManager* nistManager = G4NistManager::Instance();

  // Air defined using NIST Manager
  nistManager->FindOrBuildMaterial("G4_AIR");

  // Lead defined using NIST Manager
  fTargetMaterial = nistManager->FindOrBuildMaterial("G4_Pb");

  // Xenon gas defined using NIST Manager
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Xe");

  //Si
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_Si");

  //CZT
  fChamberMaterial = nistManager->FindOrBuildMaterial("G4_CADMIUM_TELLURIDE");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  G4Material* air = G4Material::GetMaterial("G4_AIR");

  // Sizes of the principal geometrical components (solids)

  G4double chamberSpacing = 80 * cm;  // from chamber center to center!

  G4double chamberWidth = 20.0 * cm;  // width of the chambers
  G4double targetLength = 5.0 * cm;  // full length of Target

  G4double trackerLength = (fNbOfChambers + 1) * chamberSpacing;

  G4double worldLength = 1.2 * (2 * targetLength + trackerLength);

  G4double targetRadius = 0.5 * targetLength;  // Radius of Target
  targetLength = 0.5 * targetLength;  // Half length of the Target
  G4double trackerSize = 0.5 * trackerLength;  // Half length of the Tracker

  // Definitions of Solids, Logical Volumes, Physical Volumes

  // World

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / mm << " mm" << G4endl;

  auto worldS = new G4Box("world",  // its name
                          worldLength / 2, worldLength / 2, worldLength / 2);  // its size
  auto worldLV = new G4LogicalVolume(worldS,  // its solid
                                     air,  // its material
                                     "World");  // its name

  //  Must place the World Physical volume unrotated at (0,0,0).
  //
  auto worldPV = new G4PVPlacement(nullptr,  // no rotation
                                   G4ThreeVector(),  // at (0,0,0)
                                   worldLV,  // its logical volume
                                   "World",  // its name
                                   nullptr,  // its mother  volume
                                   false,  // no boolean operations
                                   0,  // copy number
                                   fCheckOverlaps);  // checking overlaps

  // Target

  G4ThreeVector positionTarget = G4ThreeVector(0, 0, -(targetLength + trackerSize));

  auto targetS = new G4Tubs("target", 0., targetRadius, targetLength, 0. * deg, 360. * deg);
  fLogicTarget = new G4LogicalVolume(targetS, fTargetMaterial, "Target", nullptr, nullptr, nullptr);
  new G4PVPlacement(nullptr,  // no rotation
                    positionTarget,  // at (x,y,z)
                    fLogicTarget,  // its logical volume
                    "Target",  // its name
                    worldLV,  // its mother volume
                    false,  // no boolean operations
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  G4cout << "Target is " << 2 * targetLength / cm << " cm of " << fTargetMaterial->GetName()
         << G4endl;

  // Tracker

  G4ThreeVector positionTracker = G4ThreeVector(0, 0, 0);

  auto trackerS = new G4Tubs("tracker", 0, trackerSize, trackerSize, 0. * deg, 360. * deg);
  auto trackerLV = new G4LogicalVolume(trackerS, air, "Tracker", nullptr, nullptr, nullptr);
  new G4PVPlacement(nullptr,  // no rotation
                    positionTracker,  // at (x,y,z)
                    trackerLV,  // its logical volume
                    "Tracker",  // its name
                    worldLV,  // its mother  volume
                    false,  // no boolean operations
                    0,  // copy number
                    fCheckOverlaps);  // checking overlaps

  // Visualization attributes

  G4VisAttributes boxVisAtt(G4Colour::White());
  G4VisAttributes chamberVisAtt(G4Colour::Yellow());

  worldLV->SetVisAttributes(boxVisAtt);
  fLogicTarget->SetVisAttributes(boxVisAtt);
  trackerLV->SetVisAttributes(boxVisAtt);

  // Tracker segments

  // Tính toán các thông số chung
  G4double firstPosition = -trackerSize + chamberSpacing; // Vị trí z khối đầu tiên
  G4double rmaxFirst = 0.5 * trackerLength / 10;         // Bán kính khối đầu tiên
  G4double rmaxIncr = 0.5 * trackerLength / (fNbOfChambers - 1); // Tăng bán kính giữa các khối

  if (chamberSpacing < chamberWidth) {
      G4Exception("DetectorConstruction::DefineVolumes()", "InvalidSetup", FatalException,
          "Width>Spacing");
  }

  // Tạo vật liệu cho khối 1 và khối 2
  G4Material* materialSi = G4Material::GetMaterial("G4_Si");  // Vật liệu Silicon
  G4Material* materialCZT = G4Material::GetMaterial("G4_CADMIUM_TELLURIDE"); // Vật liệu CZT

  // Tạo khối đầu tiên (Si)
  G4double Zposition1 = firstPosition; // Vị trí z của khối đầu tiên
  G4double halfLength1 = rmaxFirst;   // Chiều dài (half) = bán kính
  G4double halfWidth1 = rmaxFirst;    // Chiều rộng (half) = bán kính
  G4double halfHeight1 = 0.5 * chamberWidth; // Chiều cao (half)

  auto chamberS1 = new G4Box("Chamber1_solid", halfLength1, halfWidth1, halfHeight1);

  fLogicChamber[0] =
      new G4LogicalVolume(chamberS1, materialSi, "Chamber1_LV", nullptr, nullptr, nullptr);

  fLogicChamber[0]->SetVisAttributes(chamberVisAtt);

  new G4PVPlacement(nullptr,
      G4ThreeVector(0, 0, Zposition1),
      fLogicChamber[0],
      "Chamber1_PV",
      trackerLV,
      false,
      0,
      fCheckOverlaps);

  // Tạo khối thứ hai (CZT)
  G4double Zposition2 = firstPosition + chamberSpacing; // Vị trí z của khối thứ hai
  G4double rmax2 = rmaxFirst + rmaxIncr;                // Bán kính khối thứ hai
  G4double halfLength2 = rmax2;
  G4double halfWidth2 = rmax2;
  G4double halfHeight2 = 0.5 * chamberWidth;

  auto chamberS2 = new G4Box("Chamber2_solid", halfLength2, halfWidth2, halfHeight2);

  fLogicChamber[1] =
      new G4LogicalVolume(chamberS2, materialCZT, "Chamber2_LV", nullptr, nullptr, nullptr);

  fLogicChamber[1]->SetVisAttributes(chamberVisAtt);

  new G4PVPlacement(nullptr,
      G4ThreeVector(0, 0, Zposition2),
      fLogicChamber[1],
      "Chamber2_PV",
      trackerLV,
      false,
      1,
      fCheckOverlaps);


  // Example of User Limits
  //
  // Below is an example of how to set tracking constraints in a given
  // logical volume
  //
  // Sets a max step length in the tracker region, with G4StepLimiter

  G4double maxStep = 0.5 * chamberWidth;
  fStepLimit = new G4UserLimits(maxStep);
  trackerLV->SetUserLimits(fStepLimit);

  /// Set additional contraints on the track, with G4UserSpecialCuts
  ///
  /// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
  /// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
  ///                                           maxLength,
  ///                                           maxTime,
  ///                                           minEkin));

  // Always return the physical world

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  G4String trackerChamberSDname = "/TrackerChamberSD";
  auto trackerSD = new TrackerSD(trackerChamberSDname, "TrackerHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(trackerSD);
  // Setting trackerSD to all logical volumes with the same name
  // of "Chamber_LV".
  SetSensitiveDetector("Chamber1_LV", trackerSD, true);
  SetSensitiveDetector("Chamber2_LV", trackerSD, true);

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue = G4ThreeVector();
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTargetMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

  if (fTargetMaterial != pttoMaterial) {
    if (pttoMaterial) {
      fTargetMaterial = pttoMaterial;
      if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
      G4cout << G4endl << "----> The target is made of " << materialName << G4endl;
    }
    else {
      G4cout << G4endl << "-->  WARNING from SetTargetMaterial : " << materialName << " not found"
             << G4endl;
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetChamberMaterial(G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();

  G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);

  if (fChamberMaterial != pttoMaterial) {
    if (pttoMaterial) {
      fChamberMaterial = pttoMaterial;
      for (G4int copyNo = 0; copyNo < fNbOfChambers; copyNo++) {
        if (fLogicChamber[copyNo]) fLogicChamber[copyNo]->SetMaterial(fChamberMaterial);
      }
      G4cout << G4endl << "----> The chambers are made of " << materialName << G4endl;
    }
    else {
      G4cout << G4endl << "-->  WARNING from SetChamberMaterial : " << materialName << " not found"
             << G4endl;
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit) && (maxStep > 0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
  fCheckOverlaps = checkOverlaps;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace B2a