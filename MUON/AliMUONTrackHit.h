#ifndef ALIMUONTRACKHIT_H
#define ALIMUONTRACKHIT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/

#include <TROOT.h>
#include "AliMUONTrackParam.h"

class AliMUONHitForRec;

class AliMUONTrackHit : public TObject {
 public:
  AliMUONTrackHit(){
    // Constructor
    ;} // Constructor
  virtual ~AliMUONTrackHit(){
    // Destructor
    ;} // Destructor
  AliMUONTrackHit (const AliMUONTrackHit& AliMUONTrackHit); // copy constructor
  AliMUONTrackHit& operator=(const AliMUONTrackHit& AliMUONTrackHit); // assignment operator
  AliMUONTrackHit(AliMUONHitForRec* Hit); // Constructor from one HitForRec

  AliMUONHitForRec* GetHitForRecPtr(void);
  AliMUONTrackParam* GetTrackParam(void);
  void SetTrackParam(AliMUONTrackParam* TrackParam);

  // What is necessary for sorting TClonesArray's; sufficient too ????
  Bool_t IsSortable () const {
    // necessary for sorting TClonesArray of TrackHit's
    return kTRUE; }
  Int_t Compare(TObject* TrackHit); // "Compare" function for sorting

 protected:
 private:
  AliMUONTrackParam fTrackParam; // Track parameters
  AliMUONHitForRec *fHitForRecPtr; // Pointer to HitForRec
  AliMUONTrackHit *fNextTrackHitWithSameHitForRec; // Pointer to next track hit with same HitForRec
  AliMUONTrackHit *fPrevTrackHitWithSameHitForRec; // Pointer to previous track hit with same HitForRec

  ClassDef(AliMUONTrackHit, 1) // Class definition in ROOT context
    };
	
#endif
