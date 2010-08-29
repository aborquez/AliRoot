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

// --- ROOT system ---
#include <Riostream.h>
#include <TMath.h>

// --- Standard library ---

// --- AliRoot header files ---

#include "AliEMCALRawDigit.h"

ClassImp(AliEMCALRawDigit)

//____________________________________________________________________________
AliEMCALRawDigit::AliEMCALRawDigit() : TObject(),
fId(-1),
fNSamples(0),
fSamples(0x0)
{
	// default ctor 
}

//____________________________________________________________________________
AliEMCALRawDigit::AliEMCALRawDigit(Int_t id, Int_t timeSamples[], Int_t nSamples) : TObject(),
fId(id),
fNSamples(nSamples),
fSamples(0x0)
{
	//
	fSamples = new Int_t[fNSamples] ;
	for (Int_t i=0; i < fNSamples; i++) fSamples[i] = timeSamples[i];
}

//____________________________________________________________________________
AliEMCALRawDigit::AliEMCALRawDigit(const AliEMCALRawDigit& digit) : TObject(),//AliDigitNew(digit),
fId(digit.fId),
fNSamples(digit.fNSamples),
fSamples(0x0)

{
	// Copy ctor
	// Data members of the base class (AliNewDigit)
	
//	fAmp         = digit.fAmp;
//	fIndexInList = digit.fIndexInList;
	fSamples     = new Int_t[fNSamples]; 
	for (Int_t i=0; i < digit.fNSamples; i++) fSamples[i] = digit.fSamples[i];
}

//____________________________________________________________________________
AliEMCALRawDigit::~AliEMCALRawDigit() 
{
  // Delete array of time samples
  if(fSamples) delete [] fSamples;
}

//____________________________________________________________________________
void AliEMCALRawDigit::Clear(Option_t *) 
{
  // Delete array of time samples
  if(fSamples) delete [] fSamples;
}


//____________________________________________________________________________
Bool_t AliEMCALRawDigit::GetTimeSample(const Int_t iSample, Int_t& timeBin, Int_t& amp) const
{
	if (iSample > fNSamples || iSample < 0) return kFALSE;
	
	amp     =  fSamples[iSample] & 0xFFF;
	timeBin = (fSamples[iSample] >> 12) & 0xFF;

	return kTRUE;
}

//____________________________________________________________________________
void AliEMCALRawDigit::Print(const Option_t* /*opt*/) const
{
	printf("===\nDigit id: %4d / %d Time Samples: \n",fId,fNSamples);
	for (Int_t i=0; i < fNSamples; i++) 
	{
		Int_t timeBin=-1, amp=0;
		GetTimeSample(i, timeBin, amp);
		printf("(%d,%d) ",timeBin,amp);
	}
	
	printf("\n");
}

//____________________________________________________________________________
Int_t AliEMCALRawDigit::Compare(const TObject* obj) const
{
	// Compares two digits with respect to its Id
	// to sort according increasing Id

	Int_t rv=2;

	AliEMCALRawDigit* digit = (AliEMCALRawDigit *)obj; 

	Int_t iddiff = fId - digit->GetId(); 

	if ( iddiff > 0 ) 
		rv = 1;
	else if ( iddiff < 0 )
		rv = -1; 
	else
		rv = 0;
  
	return rv; 
}

//____________________________________________________________________________
Bool_t AliEMCALRawDigit::operator==(AliEMCALRawDigit const & digit) const 
{
	// Two digits are equal if they have the same Id
  
	if(fId == digit.fId) 
		return kTRUE;
	else 
		return kFALSE;
}
