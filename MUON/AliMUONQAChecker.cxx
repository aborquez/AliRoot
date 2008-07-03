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

// $Id$

#include "AliMUONQAChecker.h"

/// \class AliMUONQAChecker
///
/// Implementation of AliQACheckerBase for MCH and MTR
///
/// For the moment we only implement the checking of raw data QA for the tracker
/// by looking at the occupancy at the manu level.
/// We count the number of manus above a given occupancy threshold, and
/// depending on that number, the resulting QA flag is warning, error or fatal.
/// (there's no "info" type in this case).
///
/// \author Laurent Aphecetche, Subatech

#include "AliQA.h"
#include "AliMUONVTrackerData.h"
#include "AliMpManuIterator.h"
#include <TDirectory.h>

/// \cond CLASSIMP
ClassImp(AliMUONQAChecker)
/// \endcond

//__________________________________________________________________
AliMUONQAChecker::AliMUONQAChecker() : 
    AliQACheckerBase("MUON","MUON Quality Assurance Data Maker") 
{
	/// ctor
}          

//__________________________________________________________________
AliMUONQAChecker::~AliMUONQAChecker() 
{
	/// dtor
}

//__________________________________________________________________
AliMUONQAChecker::AliMUONQAChecker(const AliMUONQAChecker& qac) : 
    AliQACheckerBase(qac.GetName(), qac.GetTitle()) 
{
	/// copy ctor 
}   

//__________________________________________________________________
AliMUONQAChecker& AliMUONQAChecker::operator = (const AliMUONQAChecker& /*qac*/ )
{
    /// Equal operator.
    return *this;
}

//______________________________________________________________________________
const Double_t 
AliMUONQAChecker::Check(AliQA::ALITASK_t /*index*/)
{
  /// Check data
  
  AliError(Form("This method is not implemented. Should it be ? fDataSubDir = %p (%s)",
                fDataSubDir, ( fDataSubDir ? fDataSubDir->GetPath() : "")));
  return 0.0;
}

//______________________________________________________________________________
const Double_t 
AliMUONQAChecker::Check(AliQA::ALITASK_t index, TObjArray * list)
{
  /// Check objects in list
  
  if ( index == AliQA::kRAW ) 
  {
    return CheckRaws(list);
  }
  
  AliWarning(Form("Checker for task %d not implement for the moment",index));
  return 0.0;
}

//______________________________________________________________________________
const Double_t 
AliMUONQAChecker::CheckRaws(TObjArray * list)
{
	TIter next(list);
	TObject* object;
	AliMUONVTrackerData* data(0x0);
  
	while ( (object=next()) && !data )
	{
		if (object->InheritsFrom("AliMUONVTrackerData"))
		{
			data = static_cast<AliMUONVTrackerData*>(object);
		}
	}

  if ( !data ) 
  {
    AliError("Did not find TrackerData in the list !");
    return 0.0;
  }
  
  AliMpManuIterator it;
  Int_t detElemId;
  Int_t manuId;
  Int_t n50(0); // number of manus with occupancy above 0.5
  Int_t n75(0); // number of manus with occupancy above 0.75
  Int_t n(0); // number of manus with some occupancy
  
  while ( it.Next(detElemId,manuId) )
  {
    Float_t occ = data->Manu(detElemId,manuId,2);
    if (occ > 0 ) ++n;
    if (occ >= 0.5 ) ++n50;
    if (occ >= 0.75 ) ++n75;    
  }

  AliInfo(Form("n %d n50 %d n75 %d",n,n50,n75));
  
  if ( n == 0 ) 
  {
    AliError("Oups. Got zero occupancy in all manus ?!");
    return 0.0;
  }

  if ( n75 ) 
  {
    AliError(Form("Got %d manus with occupancy above 0.75",n75));
    return 0.1;
  }
    
  if ( n50 ) 
  {
    AliWarning(Form("Got %d manus with occupancy above 0.5",n50));
    return 0.9;
  }

	return 1.0;
}

//______________________________________________________________________________
void 
AliMUONQAChecker::SetQA(AliQA::ALITASK_t index, const Double_t value) const
{
	// sets the QA according the return value of the Check

  AliQA * qa = AliQA::Instance(index);
  
  qa->UnSet(AliQA::kFATAL);
  qa->UnSet(AliQA::kWARNING);
  qa->UnSet(AliQA::kERROR);
  qa->UnSet(AliQA::kINFO);
  
  if ( value == 1.0 ) 
  {
    qa->Set(AliQA::kINFO);
  }
  else if ( value == 0.0 )
  {
    qa->Set(AliQA::kFATAL);
  }
  else if ( value > 0.5 ) 
  {
    qa->Set(AliQA::kWARNING);
  }
  else
  {
    qa->Set(AliQA::kERROR);
  }
}