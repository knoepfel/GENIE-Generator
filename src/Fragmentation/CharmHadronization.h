//____________________________________________________________________________
/*!

\class    genie::CharmHadronization

\brief    Provides access to the PYTHIA hadronization models. \n
          Is a concrete implementation of the HadronizationModelI interface.

\author   Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
          STFC, Rutherford Appleton Laboratory

          Hugh Gallagher <gallag@minos.phy.tufts.edu>
          Tufts University

\created  August 17, 2004

\cpright  Copyright (c) 2003-2007, GENIE Neutrino MC Generator Collaboration
          All rights reserved.
          For the licensing terms see $GENIE/USER_LICENSE.
*/
//____________________________________________________________________________

#ifndef _CHARM_HADRONIZATION_H_
#define _CHARM_HADRONIZATION_H_

#include "Fragmentation/HadronizationModelI.h"

class TPythia6;
class TF1;

namespace genie {

class Spline;
class FragmentationFunctionI;

class CharmHadronization : public HadronizationModelI {

public:
  CharmHadronization();
  CharmHadronization(string config);
  virtual ~CharmHadronization();

  // Implement the HadronizationModelI interface
  //
  void           Initialize       (void)                                    const;
  TClonesArray * Hadronize        (const Interaction* )                     const;
  double         Weight           (void)                                    const;
  PDGCodeList *  SelectParticles  (const Interaction*)                      const;
  TH1D *         MultiplicityProb (const Interaction*, Option_t* opt = "")  const;

  // Overload the Algorithm::Configure() methods to load private data
  // members from configuration options
  //
  void Configure(const Registry & config);
  void Configure(string config);

private:

  void LoadConfig          (void);
  int  GenerateCharmHadron (int nupdg, double EvLab) const;

  // Configuration parameters
  //
  bool                           fCharmOnly;   ///< don't hadronize non-charm blob
  TF1 *                          fCharmPT2pdf; ///< charm hadron pT^2 pdf
  const FragmentationFunctionI * fFragmFunc;   ///< charm hadron fragmentation func
  Spline *                       fD0FracSpl;   ///< nu charm fraction vs Ev: D0
  Spline *                       fDpFracSpl;   ///< nu charm fraction vs Ev: D+
  Spline *                       fDsFracSpl;   ///< nu charm fraction vs Ev: Ds+
  double                         fD0BarFrac;   ///< nubar \bar{D0} charm fraction
  double                         fDmFrac;      ///< nubar D- charm fraction
  mutable TPythia6 *             fPythia;      ///< remnant (non-charm) hadronizer
};

}         // genie namespace

#endif    // _CHARM_HADRONIZATION__H_

