#include "PandaProd/Ntupler/interface/JetFiller.h"

#include "PandaProd/Objects/interface/PJet.h"

#ifndef JETIDFUNC
#define JETIDFUNC

bool PassJetID(const pat::Jet &j, panda::PJet::JetID id); 

#endif
