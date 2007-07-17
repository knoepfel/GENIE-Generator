//____________________________________________________________________________
/*
 Copyright (c) 2003-2007, GENIE Neutrino MC Generator Collaboration
 All rights reserved.
 For the licensing terms see $GENIE/USER_LICENSE.

 Author: Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         STFC, Rutherford Appleton Laboratory - May 04, 2004

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include <TMath.h>

#include "Conventions/Constants.h"
#include "CrossSections/COHXSec.h"
#include "CrossSections/GXSecFunc.h"
#include "Messenger/Messenger.h"
#include "Numerical/IntegratorI.h"
#include "PDG/PDGUtils.h"
#include "Utils/MathUtils.h"
#include "Utils/Range1.h"

using namespace genie;
using namespace genie::constants;
using namespace genie::utils;

//____________________________________________________________________________
COHXSec::COHXSec() :
XSecIntegratorI("genie::COHXSec")
{

}
//____________________________________________________________________________
COHXSec::COHXSec(string config) :
XSecIntegratorI("genie::COHXSec", config)
{

}
//____________________________________________________________________________
COHXSec::~COHXSec()
{

}
//____________________________________________________________________________
double COHXSec::Integrate(
                 const XSecAlgorithmI * model, const Interaction * in) const
{
  if(! model->ValidProcess(in) ) return 0.;

  const KPhaseSpace & kps = in->PhaseSpace();
  if(!kps.IsAboveThreshold()) {
     LOG("COHXSec", pDEBUG)  << "*** Below energy threshold";
     return 0;
  }
  Range1D_t xl = kps.Limits(kKVx);
  Range1D_t yl = kps.Limits(kKVy);

  LOG("COHXSec", pINFO)
            << "x integration range = [" << xl.min << ", " << xl.max << "]";
  LOG("COHXSec", pINFO)
            << "y integration range = [" << yl.min << ", " << yl.max << "]";

  Interaction * interaction = new Interaction(*in);
  interaction->SetBit(kISkipProcessChk);
  //interaction->SetBit(kISkipKinematicChk);

  GXSecFunc * func = new Integrand_D2XSec_DxDy_E(model, interaction);
  func->SetParam(0,"x",xl);
  func->SetParam(1,"y",yl);
  double xsec = fIntegrator->Integrate(*func);

  const InitialState & init_state = in->InitState();
  double Ev = init_state.ProbeE(kRfLab);
  LOG("COHXSec", pINFO)  << "XSec[COH] (E = " << Ev << " GeV) = " << xsec;

  delete interaction;
  delete func;
  return xsec;
}
//____________________________________________________________________________
void COHXSec::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void COHXSec::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfig();
}
//____________________________________________________________________________
void COHXSec::LoadConfig(void)
{
  //-- get specified integration algorithm
  fIntegrator = dynamic_cast<const IntegratorI *> (this->SubAlg("Integrator"));
  assert(fIntegrator);
}
//____________________________________________________________________________
