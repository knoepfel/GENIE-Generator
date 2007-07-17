//____________________________________________________________________________
/*!

\class    genie::Kinematics

\brief    Generated/set kinematical variables for an event

\author   Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
          STFC, Rutherford Appleton Laboratory

\created  May 08, 2004

\cpright  Copyright (c) 2003-2007, GENIE Neutrino MC Generator Collaboration
          All rights reserved.
          For the licensing terms see $GENIE/USER_LICENSE.
*/
//____________________________________________________________________________

#ifndef _KINEMATICS_H_
#define _KINEMATICS_H_

#include <map>
#include <iostream>

#include <TObject.h>

#include "Conventions/KineVar.h"

using std::map;
using std::ostream;

class TLorentzVector;

namespace genie {

class Kinematics : public TObject {

public:
  Kinematics();
  Kinematics(const Kinematics & kv);
 ~Kinematics();

  double x       (bool selected=false) const;
  double y       (bool selected=false) const;
  double Q2      (bool selected=false) const;
  double q2      (bool selected=false) const;
  double W       (bool selected=false) const;
  double t       (bool selected=false) const;
  double Logx    (bool selected=false) const;
  double Logy    (bool selected=false) const;
  double LogQ2   (bool selected=false) const;
  double LogW    (bool selected=false) const;
  double Log10x  (bool selected=false) const;
  double Log10y  (bool selected=false) const;
  double Log10Q2 (bool selected=false) const;
  double Log10W  (bool selected=false) const;

  const TLorentzVector & FSLeptonP4(void) const { return *fP4Fsl;     }
  const TLorentzVector & HadSystP4 (void) const { return *fP4HadSyst; }

  void   Setx  (double x,  bool selected=false);
  void   Sety  (double y,  bool selected=false);
  void   SetQ2 (double Q2, bool selected=false);
  void   Setq2 (double q2, bool selected=false);
  void   SetW  (double W,  bool selected=false);
  void   Sett  (double t,  bool selected=false);

  void   SetFSLeptonP4 (const TLorentzVector & p4);
  void   SetFSLeptonP4 (double px, double py, double pz, double E);
  void   SetHadSystP4  (const TLorentzVector & p4);
  void   SetHadSystP4  (double px, double py, double pz, double E);

  bool   KVSet(KineVar_t kv) const;
  double GetKV(KineVar_t kv) const;
  void   SetKV(KineVar_t kv, double value);

  void ClearRunningValues    (void);
  void UseSelectedKinematics (void);

  //-- Copy, reset, compare and print itself
  void Reset (void);
  void Copy  (const Kinematics & kine);
  void Print (ostream & stream) const;

  Kinematics &     operator =  (const Kinematics & kine);
  friend ostream & operator << (ostream & stream, const Kinematics & kine);

private:

  void Init    (void); ///< initialize 
  void CleanUp (void); ///< clean-up 

  //-- Private data members

  map<KineVar_t, double> fKV;        ///< selected kinematics
  TLorentzVector *       fP4Fsl;     ///< generated final state primary lepton 4-p  (LAB)
  TLorentzVector *       fP4HadSyst; ///< generated final state hadronic system 4-p (LAB)

ClassDef(Kinematics,1)
};

}       // genie namespace

#endif  // _KINEMATICS_H_
