//____________________________________________________________________________
/*
 Copyright (c) 2003-2007, GENIE Neutrino MC Generator Collaboration
 All rights reserved.
 For the licensing terms see $GENIE/USER_LICENSE.

 Author: Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         STFC, Rutherford Appleton Laboratory - May 13, 2005

 For the class documentation see the corresponding header file.

 Important revisions after version 2.0.0 :

*/
//____________________________________________________________________________

#include "Algorithm/AlgConfigPool.h"
#include "BaryonResonance/BaryonResUtils.h"
#include "EVGModules/RESInteractionListGenerator.h"
#include "EVGCore/InteractionList.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"

using namespace genie;

//___________________________________________________________________________
RESInteractionListGenerator::RESInteractionListGenerator() :
InteractionListGeneratorI("genie::RESInteractionListGenerator")
{

}
//___________________________________________________________________________
RESInteractionListGenerator::RESInteractionListGenerator(string config) :
InteractionListGeneratorI("genie::RESInteractionListGenerator", config)
{

}
//___________________________________________________________________________
RESInteractionListGenerator::~RESInteractionListGenerator()
{

}
//___________________________________________________________________________
InteractionList * RESInteractionListGenerator::CreateInteractionList(
                                       const InitialState & init_state) const
{
  LOG("InteractionList", pINFO) << "InitialState = " << init_state.AsString();

  // In the thread generating interactions from the list produced here (RES), 
  // we simulate (for free and nuclear targets) semi-inclusive resonance
  // interactions: v + N -> v(l) + R -> v(l) + X
  // Specifically, the RES thread generates:
  //
  //  CC:
  //    nu       + p (A) -> l-       R (A), for all resonances with Q=+2 
  //    nu       + n (A) -> l-       R (A), for all resonances with Q=+1 
  //    \bar{nu} + p (A) -> l+       R (A), for all resonances with Q= 0 
  //    \bar{nu} + n (A) -> l+       R (A), for all resonances with Q=-1 
  //  NC:
  //    nu       + p (A) -> nu       R (A), for all resonances with Q=+1 
  //    nu       + n (A) -> nu       R (A), for all resonances with Q= 0 
  //    \bar{nu} + p (A) -> \bar{nu} R (A), for all resonances with Q=+1 
  //    \bar{nu} + n (A) -> \bar{nu} R (A), for all resonances with Q= 0 
  //
  // and then the resonance R should be allowed to decay to get the full
  // hadronic final state X. All decay channels kinematically accessible
  // to the (off the mass-shell produced) resonance can be allowed.
  // Another similar thread exists (SPP) but there we generate exlusive
  // single pion interactions from resonance production.

  // specify the requested interaction type
  InteractionType_t inttype;
  if      (fIsCC) inttype = kIntWeakCC;
  else if (fIsNC) inttype = kIntWeakNC;
  else {
     LOG("InteractionList", pWARN)
       << "Unknown InteractionType! Returning NULL InteractionList "
                         << "for init-state: " << init_state.AsString();
     return 0;
  }

  // create a process information object
  ProcessInfo proc_info(kScResonant, inttype);

  // learn whether the input nuclear or free target has avail. p and n
  const Target & inp_target = init_state.Tgt();
  bool hasP = (inp_target.Z() > 0);
  bool hasN = (inp_target.N() > 0);

  // possible hit nucleons
  const int hit_nucleon[2] = {kPdgProton, kPdgNeutron};

  // create an interaction list
  InteractionList * intlist = new InteractionList;

  // loop over all baryon resonances considered in current MC job
  unsigned int nres = fResList.NResonances();
  for(unsigned int ires = 0; ires < nres; ires++) {

     //get current resonance
     Resonance_t res = fResList.ResonanceId(ires);

     // loop over hit nucleons
     for(int i=0; i<2; i++) {

       // proceed only if the hit nucleon exists in the current init state
       if(hit_nucleon[i]==kPdgProton  && !hasP) continue;
       if(hit_nucleon[i]==kPdgNeutron && !hasN) continue;

       // proceed only if the current resonance conserves charge
       // (the only problematic case is when the RES charge has to be +2
       //  because then only Delta resonances are possible)
       bool skip_res =  proc_info.IsWeakCC() &&
                        pdg::IsNeutrino(init_state.ProbePdg()) &&
                        (hit_nucleon[i]==kPdgProton) &&
                        (!utils::res::IsDelta(res));
       if(skip_res) continue;

       // create an interaction
       Interaction * interaction = new Interaction(init_state, proc_info);
    
       // add the struck nucleon
       Target * target = interaction->InitStatePtr()->TgtPtr();
       target->SetHitNucPdg(hit_nucleon[i]);

       // add the baryon resonance in the exclusive tag
       XclsTag * xcls = interaction->ExclTagPtr();
       xcls->SetResonance(res);

       // add the interaction at the interaction list
       intlist->push_back(interaction);

     }//hit nucleons
  } //resonances

  if(intlist->size() == 0) {
     LOG("InteractionList", pERROR)
       << "Returning NULL InteractionList for init-state: "
                                                  << init_state.AsString();
     delete intlist;
     return 0;
  }

  return intlist;
}
//___________________________________________________________________________
void RESInteractionListGenerator::Configure(const Registry & config)
{
  Algorithm::Configure(config);
  this->LoadConfigData();
}
//____________________________________________________________________________
void RESInteractionListGenerator::Configure(string config)
{
  Algorithm::Configure(config);
  this->LoadConfigData();
}
//____________________________________________________________________________
void RESInteractionListGenerator::LoadConfigData(void)
{
  AlgConfigPool * confp = AlgConfigPool::Instance();
  const Registry * gc = confp->GlobalParameterList();

  fIsCC = fConfig->GetBoolDef("is-CC", false);
  fIsNC = fConfig->GetBoolDef("is-NC", false);

  // Create the list with all the baryon resonances that the user wants me to
  // consider (from this algorithm's config file).

  LOG("InteractionList", pDEBUG) << "Getting the baryon resonance list";

  fResList.Clear();
  string resonances = fConfig->GetStringDef(
                   "ResonanceNameList", gc->GetString("ResonanceNameList"));
  SLOG("InteractionList", pDEBUG) << "Resonance list: " << resonances;

  fResList.DecodeFromNameList(resonances);
  LOG("InteractionList", pDEBUG) << fResList;
}
//____________________________________________________________________________

