/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/*
$Log$
Revision 1.1.2.3  2000/06/09 21:03:09  morsch
Make includes consistent with new file structure.

Revision 1.1.2.2  2000/06/09 12:58:05  gosset
Removed comment beginnings in Log sections of .cxx files
Suppressed most violations of coding rules

Revision 1.1.2.1  2000/06/07 14:44:53  gosset
Addition of files for track reconstruction in C++
*/

//__________________________________________________________________________
//
// Track parameters in ALICE dimuon spectrometer
//__________________________________________________________________________

#include <iostream.h>

#include "AliCallf77.h" 
#include "AliMUON.h" 
#include "AliMUONHitForRec.h" 
#include "AliMUONSegment.h" 
#include "AliMUONTrackParam.h" 
#include "AliMUONChamber.h" 
#include "AliRun.h" 

ClassImp(AliMUONTrackParam) // Class implementation in ROOT context

#ifndef WIN32 
# define reco_ghelix reco_ghelix_
#else 
# define reco_ghelix RECO_GHELIX
#endif 

extern "C"
{
void type_of_call reco_ghelix(Double_t &Charge, Double_t &StepLength, Double_t *VGeant3, Double_t *VGeant3New);
}

// Inline functions for Get and Set: inline removed because it does not work !!!!
Double_t AliMUONTrackParam::GetInverseBendingMomentum(void) {
  // Get fInverseBendingMomentum
  return fInverseBendingMomentum;}
void AliMUONTrackParam::SetInverseBendingMomentum(Double_t InverseBendingMomentum) {
  // Set fInverseBendingMomentum
  fInverseBendingMomentum = InverseBendingMomentum;}
Double_t AliMUONTrackParam::GetBendingSlope(void) {
  // Get fBendingSlope
  return fBendingSlope;}
void AliMUONTrackParam::SetBendingSlope(Double_t BendingSlope) {
  // Set fBendingSlope
  fBendingSlope = BendingSlope;}
Double_t AliMUONTrackParam::GetNonBendingSlope(void) {
  // Get fNonBendingSlope
  return fNonBendingSlope;}
void AliMUONTrackParam::SetNonBendingSlope(Double_t NonBendingSlope) {
  // Set fNonBendingSlope
  fNonBendingSlope = NonBendingSlope;}
Double_t AliMUONTrackParam::GetZ(void) {
  // Get fZ
  return fZ;}
void AliMUONTrackParam::SetZ(Double_t Z) {
  // Set fZ
  fZ = Z;}
Double_t AliMUONTrackParam::GetBendingCoor(void) {
  // Get fBendingCoor
  return fBendingCoor;}
void AliMUONTrackParam::SetBendingCoor(Double_t BendingCoor) {
  // Set fBendingCoor
  fBendingCoor = BendingCoor;}
Double_t AliMUONTrackParam::GetNonBendingCoor(void) {
  // Get fNonBendingCoor
  return fNonBendingCoor;}
void AliMUONTrackParam::SetNonBendingCoor(Double_t NonBendingCoor) {
  // Set fNonBendingCoor
  fNonBendingCoor = NonBendingCoor;}

  //__________________________________________________________________________
void AliMUONTrackParam::ExtrapToZ(Double_t Z)
{
  // Track parameter extrapolation to the plane at "Z".
  // On return, the track parameters resulting from the extrapolation
  // replace the current track parameters.
  // Use "reco_ghelix" which should be replaced by something else !!!!
  if (this->fZ == Z) return; // nothing to be done if same Z
  Double_t forwardBackward; // +1 if forward, -1 if backward
  if (Z > this->fZ) forwardBackward = 1.0;
  else forwardBackward = -1.0;
  Double_t temp, vGeant3[7], vGeant3New[7]; // 7 in parameter ????
  Int_t iGeant3, stepNumber;
  Int_t maxStepNumber = 5000; // in parameter ????
  // For safety: return kTRUE or kFALSE ????
  // Parameter vector for calling GHELIX in Geant3
  SetGeant3Parameters(vGeant3, forwardBackward);
  // For use of reco_ghelix...: invert X and Y, PX/PTOT and PY/PTOT !!!!
  temp = vGeant3[0]; vGeant3[0] = vGeant3[1]; vGeant3[1] = temp;
  temp = vGeant3[3]; vGeant3[3] = vGeant3[4]; vGeant3[4] = temp;
  // charge must be changed with momentum for backward motion
  Double_t charge =
    forwardBackward * TMath::Sign(Double_t(1.0), this->fInverseBendingMomentum);
  Double_t stepLength = 6.0; // in parameter ????
  // Extrapolation loop
  stepNumber = 0;
  while (((forwardBackward * (vGeant3[2] - Z)) <= 0.0) &&
	 (stepNumber < maxStepNumber)) {
    stepNumber++;
    // call Geant3 "ghelix" subroutine through a copy in "reco_muon.F":
    // the true function should be called, but how ???? and remove prototyping ...
    reco_ghelix(charge, stepLength, vGeant3, vGeant3New);
    if ((forwardBackward * (vGeant3New[2] - Z)) > 0.0) break; // one is beyond Z
    // better use TArray ????
    for (iGeant3 = 0; iGeant3 < 7; iGeant3++)
      {vGeant3[iGeant3] = vGeant3New[iGeant3];}
  }
  // check maxStepNumber ????
  // For use of reco_ghelix...:
  // invert back X and Y, PX/PTOT and PY/PTOT, both for vGeant3 and vGeant3New !!!!
  temp = vGeant3[0]; vGeant3[0] = vGeant3[1]; vGeant3[1] = temp;
  temp = vGeant3New[0]; vGeant3New[0] = vGeant3New[1]; vGeant3New[1] = temp;
  temp = vGeant3[3]; vGeant3[3] = vGeant3[4]; vGeant3[4] = temp;
  temp = vGeant3New[3]; vGeant3New[3] = vGeant3New[4]; vGeant3New[4] = temp;
  // Interpolation back to exact Z (2nd order)
  // should be in function ???? using TArray ????
  Double_t dZ12 = vGeant3New[2] - vGeant3[2]; // 1->2
  Double_t dZ1i = Z - vGeant3[2]; // 1-i
  Double_t dZi2 = vGeant3New[2] - Z; // i->2
  Double_t xPrime = (vGeant3New[0] - vGeant3[0]) / dZ12;
  Double_t xSecond =
    ((vGeant3New[3] / vGeant3New[5]) - (vGeant3[3] / vGeant3[5])) / dZ12;
  Double_t yPrime = (vGeant3New[1] - vGeant3[1]) / dZ12;
  Double_t ySecond =
    ((vGeant3New[4] / vGeant3New[5]) - (vGeant3[4] / vGeant3[5])) / dZ12;
  vGeant3[0] = vGeant3[0] + xPrime * dZ1i - 0.5 * xSecond * dZ1i * dZi2; // X
  vGeant3[1] = vGeant3[1] + yPrime * dZ1i - 0.5 * ySecond * dZ1i * dZi2; // Y
  vGeant3[2] = Z; // Z
  Double_t xPrimeI = xPrime - 0.5 * xSecond * (dZi2 - dZ1i);
  Double_t yPrimeI = yPrime - 0.5 * ySecond * (dZi2 - dZ1i);
  vGeant3[5] =
    1.0 / TMath::Sqrt(1.0 + xPrimeI * xPrimeI + yPrimeI * yPrimeI); // PZ/PTOT
  vGeant3[3] = xPrimeI * vGeant3[5]; // PX/PTOT
  vGeant3[4] = yPrimeI * vGeant3[5]; // PY/PTOT
  // Track parameters from Geant3 parameters
  GetFromGeant3Parameters(vGeant3, charge);
}

  //__________________________________________________________________________
void AliMUONTrackParam::SetGeant3Parameters(Double_t *VGeant3, Double_t ForwardBackward)
{
  // Set vector of Geant3 parameters pointed to by "VGeant3"
  // from track parameters in current AliMUONTrackParam.
  // Since AliMUONTrackParam is only geometry, one uses "ForwardBackward"
  // to know whether the particle is going forward (+1) or backward (-1).
  VGeant3[0] = this->fNonBendingCoor; // X
  VGeant3[1] = this->fBendingCoor; // Y
  VGeant3[2] = this->fZ; // Z
  Double_t pYZ = TMath::Abs(1.0 / this->fInverseBendingMomentum);
  Double_t pZ =
    pYZ / TMath::Sqrt(1.0 + this->fBendingSlope * this->fBendingSlope);
  VGeant3[6] =
    TMath::Sqrt(pYZ * pYZ +
		pZ * pZ * this->fNonBendingSlope * this->fNonBendingSlope); // PTOT
  VGeant3[5] = ForwardBackward * pZ / VGeant3[6]; // PZ/PTOT
  VGeant3[3] = this->fNonBendingSlope * VGeant3[5]; // PX/PTOT
  VGeant3[4] = this->fBendingSlope * VGeant3[5]; // PY/PTOT
}

  //__________________________________________________________________________
void AliMUONTrackParam::GetFromGeant3Parameters(Double_t *VGeant3, Double_t Charge)
{
  // Get track parameters in current AliMUONTrackParam
  // from Geant3 parameters pointed to by "VGeant3".
  // "InverseBendingMomentum" is signed with "Charge".
  this->fNonBendingCoor = VGeant3[0]; // X
  this->fBendingCoor = VGeant3[1]; // Y
  this->fZ = VGeant3[2]; // Z
  Double_t pYZ = VGeant3[6] * TMath::Sqrt(1.0 - VGeant3[3] * VGeant3[3]);
  this->fInverseBendingMomentum = Charge / pYZ;
  this->fBendingSlope = VGeant3[4] / VGeant3[5];
  this->fNonBendingSlope = VGeant3[3] / VGeant3[5];
}

  //__________________________________________________________________________
void AliMUONTrackParam::ExtrapToStation(Int_t Station, AliMUONTrackParam *TrackParam)
{
  // Track parameters extrapolated from current track parameters ("this")
  // to both chambers of the station(0..) "Station"
  // are returned in the array (dimension 2) of track parameters
  // pointed to by "TrackParam" (index 0 and 1 for first and second chambers).
  Double_t extZ[2], z1, z2;
  Int_t i1, i2;
  AliMUON *pMUON = (AliMUON*) gAlice->GetModule("MUON"); // necessary ????
  // range of Station to be checked ????
  z1 = (&(pMUON->Chamber(2 * Station)))->Z(); // Z of first chamber
  z2 = (&(pMUON->Chamber(2 * Station + 1)))->Z(); // Z of second chamber
  // First and second Z to extrapolate at
  if ((z1 > this->fZ) && (z2 > this->fZ)) {i1 = 0; i2 = 1;}
  else if ((z1 < this->fZ) && (z2 < this->fZ)) {i1 = 1; i2 = 0;}
  else {
    cout << "ERROR in AliMUONTrackParam::CreateExtrapSegmentInStation" << endl;
    cout << "Starting Z (" << this->fZ << ") in between z1 (" << z1 <<
      ") and z2 (" << z2 << ") of station(0..) " << Station << endl;
  }
  extZ[i1] = z1;
  extZ[i2] = z2;
  // copy of track parameters
  TrackParam[i1] = *this;
  // first extrapolation
  (&(TrackParam[i1]))->ExtrapToZ(extZ[0]);
  TrackParam[i2] = TrackParam[i1];
  // second extrapolation
  (&(TrackParam[i2]))->ExtrapToZ(extZ[1]);
  return;
}

