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

#include <TMath.h>
#include <TF1.h>
#include <iostream.h>
#include "AliITSsegmentationSSD.h"
#include "AliITSgeom.h"


ClassImp(AliITSsegmentationSSD)
AliITSsegmentationSSD::AliITSsegmentationSSD(){
  // default constructor
   fGeom=0;
   fCorr=0;
}
//------------------------------
AliITSsegmentationSSD::AliITSsegmentationSSD(AliITSgeom *geom){
  // constuctor
   fGeom=geom;
   fCorr=0;
   SetDetSize();
   cout<<"Dx="<<fDx<<endl;
   SetPadSize();
   SetNPads();
   Init();

}
//____________________________________________________________________________
AliITSsegmentationSSD& AliITSsegmentationSSD::operator=(AliITSsegmentationSSD &source){
// Operator =
     if(this==&source) return *this;
     this->fNstrips = source.fNstrips;
     this->fStereoP = source.fStereoP;
     this->fStereoN = source.fStereoN;
     this->fPitch   = source.fPitch;
     this->fDz      = source.fDz;
     this->fDx      = source.fDx;
     this->fDy      = source.fDy;
     this->fGeom    = source.fGeom; // copy only the pointer
     this->fCorr    = new TF1(*(source.fCorr)); // make a proper copy
     return *this;
     
}
//____________________________________________________________________________
AliITSsegmentationSSD::AliITSsegmentationSSD(AliITSsegmentationSSD &source){
  // copy constructor
  *this = source;
}
//------------------------------
void AliITSsegmentationSSD::Init(){
  // standard initalizer

  //AliITSgeomSSD *gssd = (AliITSgeomSSD *) (fGeom->GetShape(5,1,1));
  //const Float_t kconv=10000.;
    /*
    fDx = 2.*kconv*gssd->GetDx();
    fDz = 2.*kconv*gssd->GetDz();
    fDy = 2.*kconv*gssd->GetDy();
    */
    SetPadSize();
    SetNPads();
    SetAngles();

}
//-------------------------------------------------------
void AliITSsegmentationSSD::GetPadTxz(Float_t &x,Float_t &z){
  // returns P and N sided strip numbers for a given location.
    // Transformation from microns detector center local coordinates
    // to detector P and N side strip numbers..
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
    // expects x, z in microns
    */
    Float_t tanP=TMath::Tan(fStereoP);
    Float_t tanN=TMath::Tan(-fStereoN);
    Float_t x1 = x;
    Float_t z1 = z;
    x1 += fDx/2;
    z1 += fDz/2;
    x = (x1 - z1*tanP)/fPitch;
    z = (x1 - tanN*(z1 - fDz))/fPitch;
}
//-------------------------------------------------------
void AliITSsegmentationSSD::GetPadIxz(Float_t x,Float_t z,Int_t &iP,Int_t &iN)
{
  // returns P and N sided strip numbers for a given location.
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/

    // expects x, z in microns
  */
    Float_t tanP=TMath::Tan(fStereoP);
    Float_t tanN=TMath::Tan(fStereoN);
    //cout<<"1 segment::GetPad: xL,zL,fDx,fDz ="<<x<<","<<z<<","<<fDx<<","<<fDz<<endl;
    //cout<<"2 segment: ? tanP,tanN ="<<tanP<<","<<tanN<<endl;
   tanP = 0.0075;
   tanN = 0.0275;
    Float_t x1=x,z1=z;
//    cout << "GetPadIxz::Tan(" << fStereoP << ")=" << tanP << endl;
//    cout << "GetPadIxz::Tan(" << fStereoN << ")=" << tanN << endl;
    x1 += fDx/2;
    z1 += fDz/2;
    //Float_t  ldX = x1 - z1*tanP;          // distance from left-down edge 

    this->GetPadTxz(x,z);  // use existing routine.
    iP = (Int_t) x; //(Int_t)(ldX/fPitch);  // remove declaration of ldX if you remove this comment
    iP = (iP<0)? -1: iP;      
    iP = (iP>fNstrips)? -1: iP;
/*
    //cout<<"3 segment::GetPad: x1,tanP,ix1 ="<<ldX<<","<<tanP<<","<<iP<<endl;

    ldX = x1 - tanN*(fDz - z1);
*/
    iN = (Int_t) z;  //(Int_t)(ldX/fPitch);
    iN = (iN<0)? -1: iN;
    iN = (iN>fNstrips)? -1: iN;

    //cout<<"4 segment::GetPad: x2,tanN,ix2 ="<<ldX<<","<<tanN<<","<<iN<<endl;

}
//-------------------------------------------------------
void AliITSsegmentationSSD::GetPadCxz(Int_t iP,Int_t iN,Float_t &x,Float_t &z)
{
    // actually this is the GetCrossing(Float_t &,Float_t &) 

    // returns x, z  in microns !

    Float_t flag=2*fDx;

    Float_t tanP=TMath::Tan(fStereoP);
    Float_t tanN=TMath::Tan(fStereoN);

    Float_t dx = 0.1;
//    cout << "GetPadCxz::Tan(" << fStereoP << ")=" << tanP << endl;
//    cout << "GetPadCxz::Tan(" << fStereoN << ")=" << tanN << endl;
//    cout << "GetPadCxz::dx=" << dx << endl;
    x = iP*fPitch;
    z = iN*fPitch; 

    if(tanP + tanN  == 0) {x=z=flag; return ;}

    z = (z - x + tanN * fDz) / (tanP + tanN);    
    x = x + tanP * z;                         

    x -= fDx/2;
    z -= fDz/2;

    if ( ( z < -(fDz/2+dx) ) || ( z > (fDz/2+dx) ) ) {x=z=flag; return ;}
    if ( ( x < -(fDx/2+dx) ) || ( x > (fDx/2+dx) ) ) {x=z=flag; return ;}

    return;   
}
//______________________________________________________________________
void AliITSsegmentationSSD::LocalToDet(Float_t x,Float_t z,
				       Int_t &iP,Int_t &iN){
    // Transformation from Geant cm detector center local coordinates
    // to detector P and N side strip numbers..
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
    */
    const Double_t kconst = 1.0E-04; // convert microns to cm.

    x /= kconst;  // convert to microns
    z /= kconst;  // convert to microns
    this->GetPadTxz(x,z);

    // first for P side
    iP = (Int_t) x;
    if(iP<0 || iP>=fNstrips) iP=-1; // strip number must be in range.
    // Now for N side)
    iN = (Int_t) z;
    if(iN<0 || iN>=fNstrips) iN=-1; // strip number must be in range.
    return;
}
//----------------------------------------------------------------------
void AliITSsegmentationSSD::DetToLocal(Int_t ix,Int_t iPN,
				       Float_t &x,Float_t &z){
    // Transformation from detector segmentation/cell coordiantes starting
    // from 0. iPN=0 for P side and 1 for N side strip. Returned is z=0.0
    // and the corresponding x value..
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
    */
    // for strips p-side
    // x = a + b + z*tan(fStereoP); a = Dpx(iP)*(iP+0.5)-dx; b = dz*th;
    // for strips n-side
    // x = a + b + z*tan(fStereoP); a = Dpx(iN)*(iN+0.5)-dx; b = -dz*th;
    const Double_t kconst = 1.0E-04; // convert microns to cm.
    Float_t flag=kconst*Dx(); // error value
    Double_t th=0.0,dx,dz,i,a,b=0.0,xb[4],zb[4];

    z = 0.0;  // Strip center in z.
    if(iPN<0 || iPN>1){// if error return full detector size in x.
	x = z = flag; return;
    } // end if
    if(ix<0 || ix>=fNstrips) {x = z = flag; return;} // if error return full
                                                     // detector size in x.
    i  = (Double_t) ix;      // convert to double
    dx = 0.5*kconst*Dx();    // half distance in x in cm
    dz = 0.5*kconst*Dz();    // half distance in z in cm
    a  = kconst*Dpx(ix)*(i+0.5)-dx; // Min x value.
    if(iPN==0){ //P-side angle defined backwards.
	th = TMath::Tan(fStereoP); 
	b  = dz*th;
    }else if(iPN==1){ // N-side
	 th = TMath::Tan(-fStereoN);
	 b  = -dz*th;
    } // end if
    // compute average/center position of the strip.
    xb[0] = +dx; if(th!=0.0) zb[0] = (+dx-a-b)/th; else zb[0] = 0.0;
    xb[1] = -dx; if(th!=0.0) zb[1] = (-dx-a-b)/th; else zb[1] = 0.0;
    xb[2] = a+b+dz*th; zb[2] = +dz;
    xb[3] = a+b-dz*th; zb[3] = -dz;
    x = 0.0; z = 0.0;
    for(Int_t j=0;j<4;j++){
//	cout << "xb["<<j<<"]="<<xb[j]<<" zb["<<j<<"[="<<zb[j]<<endl;
	if(xb[j]>=-dx && xb[j]<=dx && zb[j]>=-dz && zb[j]<=dz){
	    x += xb[j];
	    z += zb[j];
	} // end if
    } // end for
    x *= 0.5;
    z *= 0.5;
    return;
}
//----------------------------------------------------------------------
Bool_t AliITSsegmentationSSD::GetCrossing(Int_t iP,Int_t iN,
					  Float_t &x,Float_t &z,
					  Float_t c[2][2]){
    // Given one P side strip and one N side strip, Returns kTRUE if they
    // cross each other and the location of the two crossing strips and
    // their correxlation matrix c[2][2].
    /*                       _-  Z
                    + angle /    ^
        fNstrips           v     |   N-Side        ...0
            \-------/------------|-----------\--------\
            |\\\\\\/////////////.|\\\\\\\\\\\\\\\\\\\\|
            |0\\\\/////////////..|.\\\\\\\\\\\\\\\\\\\|
            |00\\/////////////...|..\\\\\\\\\\\\\\\\\\|
       X <--|000/////////////... |...\\\\\\\\\\\\\\\\\|
            |00/////////////...  | ...\\\\\\\\\\\\\\\\|
            |0/////////////...   |  ...\\\\\\\\\\\\\\\|
            |//////////////...   |  ...\\\\\\\\\\\\\\\|
            /-----\--------------|--------------------/
        fNstrips-1             P-Side              ...0
                     |0\
                     |00\
	Dead region: |000/
                     |00/
                     |0/
       c[2][2] is defined as follows
       /c[0][0]  c[0][1]\ /delta iP\ = /delta x\
       \c[1][0]  c[1][1]/ \delta iN/ = \delta z/
    */
    const Double_t kconst = 1.0E-04; // convert microns to cm.
    Double_t thp,thn,th,dx,dz,p,ip,in;

    
    thp = TMath::Tan(fStereoP);
    thn = TMath::Tan(-fStereoN);
    th  = thp-thn;
    if(th==0.0) { // parall strips then never cross.
	x = 0.0;
	z = 0.0;
	c[0][0] = c[1][0] = c[0][1] = c[1][1] = 0.0;
	return kFALSE;
    } // end if
    // The strips must cross some place in space.
    ip = (Double_t) iP;       // convert to double now for speed
    in = (Double_t) iN;       // convert to double now for speed
    dx = 0.5*kconst*Dx();     // half distance in x in cm
    dz = 0.5*kconst*Dz();     // half distance in z in cm
    p  = kconst*Dpx(iP);             // Get strip spacing/pitch now
    x  = 0.5*p+dx + (p*(in*thp-ip*thn)-2.0*dz*thp*thn)/th;
    z  =(p*(in-ip)-dz*(thp+thn))/th;
    // compute correlations.
    c[0][0] = -thn*p/th; // dx/diP
    c[1][1] = p/th; // dz/diN
    c[0][1] = p*thp/th; // dx/diN
    c[1][0] = -p/th; // dz/diP
    if(x<-dx || x>dx || z<-dz || z>dz) return kFALSE; // crossing is outside
                                                      // of the detector so
                                                      // these strips don't
                                                      // cross.
    return kTRUE;
}
