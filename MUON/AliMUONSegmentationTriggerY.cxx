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
Revision 1.1.2.1  2000/06/09 21:51:58  morsch
Code from AliMUONSegResTriggerY.cxx

*/

/*
Old Log:
Revision 1.1.2.4  2000/05/05 10:17:04  morsch
Y strip numerotation changed (CP)

Revision 1.1.2.3  2000/04/26 12:33:40  morsch
Minor changes in some methods (CP)

Revision 1.1.2.2  2000/02/20 07:49:50  morsch
Bugs in Dpx, Dpy and ISector methods corrected (P.C.)

Revision 1.1.2.1  2000/02/17 14:34:57  morsch
Draft version from P. Crochet

*/

#include "AliMUONSegmentationTriggerY.h"
#include "TMath.h"
#include "TRandom.h"
#include "TArc.h"
#include "AliMUONChamber.h"
#include <iostream.h> 
ClassImp(AliMUONSegmentationTriggerY)

void AliMUONSegmentationTriggerY::Init(AliMUONChamber* Chamber)
{
  cout << "Initialize Trigger Chamber Geometry Y " << "\n";    
  AliMUONSegmentationTrigger::Init(Chamber);
    
// calculate x & y position of Y strips
  for (Int_t imodule=0; imodule<fgNmodule; imodule++) {    
    Float_t width=StripSizeY(fgNum[imodule]);
    for (Int_t istrip=0; istrip<fgNstripy[imodule]; istrip++){
      if (imodule<63) {
	fXofysmin[imodule][istrip]=(fgXcmin[imodule]+width*(istrip))*fZscale;
	fXofysmax[imodule][istrip]=(fgXcmin[imodule]+width*(istrip+1))*fZscale;
      } else {	
	fXofysmin[imodule][istrip]=-1.*fXofysmax[imodule-63][istrip];
	fXofysmax[imodule][istrip]=-1.*fXofysmin[imodule-63][istrip];
      }      
      fYofysmin[imodule][istrip] = fgYcmin[imodule]*fZscale;
      fYofysmax[imodule][istrip] = fgYcmax[imodule]*fZscale;
    }
  }

}

//------------------------------------------------------------------
void AliMUONSegmentationTriggerY::GetPadIxy(Float_t x,Float_t y,Int_t &ix,Int_t &iy){
//  Returns pad coordinates (ix,iy) for given real coordinates (x,y)
//  x,y = real coordinates; ix = module number , iy = strip number

  ix = 0;    
  iy = 0;
  for (Int_t imodule=0; imodule<fgNmodule; imodule++) {
    for (Int_t istrip=0; istrip<fgNstripy[imodule]; istrip++){
      if (x>fXofysmin[imodule][istrip]&&x<fXofysmax[imodule][istrip]&&
	  y>fYofysmin[imodule][istrip]&&y<fYofysmax[imodule][istrip]){
	ix = fgNum[imodule];
	iy = istrip;
      }
    }
  }
}

//------------------------------------------------------------------
void AliMUONSegmentationTriggerY::GetPadCxy(Int_t ix, Int_t iy, Float_t &x, Float_t &y){
//  Returns real coordinates (x,y) for given pad coordinates (ix,iy)
//  ix = module number , iy = strip number;  x,y = center of strip
  x = 0.;    
  y = 0.;
  for (Int_t imodule=0; imodule<fgNmodule; imodule++) {
    if (fgNum[imodule]==ix){
      x=fXofysmin[imodule][iy]+(fXofysmax[imodule][iy]-fXofysmin[imodule][iy])/2.;
      y=fYofysmin[imodule][iy]+(fYofysmax[imodule][iy]-fYofysmin[imodule][iy])/2.;
    }
  }
}

//------------------------------------------------------------------
void AliMUONSegmentationTriggerY::SetPadSize(Float_t p1, Float_t p2)
{
//  Sets the padsize 
//  
  fDpx=p1;
  fDpy=p2;
}

//------------------------------------------------------------------
void AliMUONSegmentationTriggerY::
Neighbours(Int_t iX, Int_t iY, Int_t* Nlist, Int_t Xlist[2], Int_t Ylist[2]){
// Returns list of next neighbours for given Pad (ix, iy)  
  Int_t absiX=TMath::Abs(iX); 
  *Nlist = 0;
  
  if (absiX!=0) {                         
    Int_t numModule=ModuleNumber(absiX);
    
    if (iY==fgNstripy[numModule]-1) {                      // strip right 
      if (absiX%10!=7) {
	*Nlist=1;
	Xlist[0]=absiX+1;
	Ylist[0]=0;
      } 
    } else {
      *Nlist=1;
      Xlist[0]=absiX;
      Ylist[0]=iY+1;
    }
    
    if (iY==0) {                                            // strip left 
      if (absiX%10!=1&&absiX!=52) {
	*Nlist=*Nlist+1;
	Xlist[*Nlist-1]=absiX-1;
	Ylist[*Nlist-1]=fgNstripy[numModule-1]-1;
      } 
    } else {
      *Nlist=*Nlist+1;
      Xlist[*Nlist-1]=absiX;
      Ylist[*Nlist-1]=iY-1;
    }
    
    if (iX<0) {                                  // left side of chamber 
      for (Int_t i=0; i<*Nlist; i++) {Xlist[i]=-Xlist[i];}
    }
  }     
}

//------------------------------------------------------------------   
void AliMUONSegmentationTriggerY::SetPad(Int_t ix, Int_t iy)
{
  // Sets virtual pad coordinates, needed for evaluating pad response 
  // outside the tracking program 
  GetPadCxy(ix,iy,fx,fy);
  GetPadIxy(fx,fy,fix,fiy);
  fSector=Sector(ix,iy);    
}

//------------------------------------------------------------------   
Int_t AliMUONSegmentationTriggerY::ISector()
{ return fSector;}

//------------------------------------------------------------------   
Int_t AliMUONSegmentationTriggerY::Ix()
{ return fix;}

//------------------------------------------------------------------   
Int_t AliMUONSegmentationTriggerY::Iy()
{ return fiy;}

//------------------------------------------------------------------
Float_t AliMUONSegmentationTriggerY::Dpx(Int_t isec)
{ // returns x size of y strips for sector isec

  if (isec==1) {
    return 2.125*fZscale;
  } else if (isec==2) {
    return 2.125*fZscale;
  } else if (isec==3) {
    return 2.125*fZscale;
  } else if (isec==4) {
    return 4.25*fZscale;
  } else {
    return 0.;	
  }       
}

//------------------------------------------------------------------
Float_t AliMUONSegmentationTriggerY::Dpy(Int_t isec)
{ // returns y size of y strips for sector isec
    // cout << " In AliMUONSegmentationTriggerX::Dpx" << "\n";  

  if (isec==1) {
    return 68.0*fZscale;
  } else if (isec==2) {
    return 51.0*fZscale;
  } else if (isec==3) {
    return 68.0*fZscale;
  } else if (isec==4) {
    return 68.0*fZscale;
  } else if (isec==5) {
    return 68.0*fZscale;
  } else {
    return 0.;
  }

}

//------------------------------------------------------------------   
void AliMUONSegmentationTriggerY::SetHit(Float_t xhit, Float_t yhit)
{ AliMUONSegmentationTrigger::SetHit(xhit,yhit);}

//------------------------------------------------------------------   
Int_t AliMUONSegmentationTriggerY::Sector(Int_t ix, Int_t iy)
{
// Returns sector number for given module
// 
  Int_t absix=TMath::Abs(ix);
  Int_t iwidth=Int_t(StripSizeY(absix));
    
  if (absix==52) {
    return 1;
  } else if (absix==41||absix==61) {
    return 2;
  } else if (iwidth==2) {
    return 3;
  } else if (iwidth==4) {
    return 4;
  } else {
    return 0;
  }
}

//------------------------------------------------------------------   
void AliMUONSegmentationTriggerY::
IntegrationLimits(Float_t& x1, Float_t& x2, Float_t& x3, Float_t& width) 
{ 
// returns quantities needed to evaluate neighbour strip response
  Int_t ix,iy;
  Float_t xstrip,ystrip;
  GetPadIxy(fxhit,fyhit,ix,iy);  
  GetPadCxy(ix,iy,xstrip,ystrip);  
  x1=fxhit;        // hit x position
  x2=xstrip;       // x coordinate of the main strip
  x3=fx;           // current strip real x coordinate  
  width=StripSizeY(ix);   // width of the main strip 
}




