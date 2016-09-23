#ifndef PANDA_OBJECTS_LINKDEF_H
#define PANDA_OBJECTS_LINKDEF_H
#include "PandaProd/Objects/interface/PObject.h"
#include "PandaProd/Objects/interface/PEvent.h"
#include "PandaProd/Objects/interface/PPFCand.h"
#include "PandaProd/Objects/interface/PJet.h"
#include "PandaProd/Objects/interface/PFatJet.h"
#include "PandaProd/Objects/interface/PGenParticle.h"
#endif

#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma link C++ namespace panda;

#pragma link C++ class panda::PObject+;
#pragma link C++ class panda::PEvent+;
#pragma link C++ class panda::PPFCand+;
#pragma link C++ class panda::PJet+;
#pragma link C++ class panda::PFatJet+;
#pragma link C++ class panda::PGenParticle+;
#pragma link C++ class panda::VPFCand+;
#pragma link C++ class panda::VJet+;
#pragma link C++ class panda::VFatJet+;
#pragma link C++ class panda::VGenParticle+;
#endif
