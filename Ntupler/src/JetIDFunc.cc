#include "../interface/JetIDFunc.h"

bool PassJetID(const pat::Jet &j, panda::PJet::JetID id) {
	// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
	//															Loose -- Tight Jet ID																																																																								 
	// --- Number of Constituents	 > 1		 > 1
	// --- Neutral Hadron Fraction	< 0.99	< 0.90																																																																												
	// --- Neutral EM Fraction			< 0.99	< 0.90
	// --- Muon Fraction		< 0.8	 < 0.8																																																																																 
	// --- And for -2.4 <= eta <= 2.4 in addition apply
	// --- Charged Hadron Fraction	> 0		 > 0																																																																													 
	// --- Charged Multiplicity		 > 0		 > 0
	// --- Charged EM Fraction			< 0.99	< 0.90

	bool jetid = false;																																																																																									 
	float NHF		= j.neutralHadronEnergyFraction();																																																																											 
	float NEMF	 = j.neutralEmEnergyFraction();
	float CHF		= j.chargedHadronEnergyFraction();																																																																											 
	//float MUF		= j.muonEnergyFraction();
	float CEMF	 = j.chargedEmEnergyFraction();																																																																													 
	int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
	int CHM			= j.chargedMultiplicity();																																																																															 
	int NumNeutralParticle =j.neutralMultiplicity();
	float eta = j.eta();

	if (id==panda::PJet::kLoose || id==panda::PJet::kMonojet) {
			jetid = (NHF<0.99 && NEMF<0.99 && NumConst>1) 
								&& ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(eta)>2.4) 
								&& fabs(eta)<=3.0;
			jetid = jetid || (NEMF<0.90 && NumNeutralParticle>10 && fabs(eta)>3.0);
	}

	if (id==panda::PJet::kTight) {	 
			jetid = (NHF<0.90 && NEMF<0.90 && NumConst>1) 
								&& ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(eta)>2.4) 
								&& fabs(eta)<=3.0;
			jetid = jetid || (NEMF<0.90 && NumNeutralParticle>10 && fabs(eta)>3.0 ); 
	}

	if (id==panda::PJet::kMonojet)
		jetid = jetid && (NHF < 0.8 && CHF > 0.1);

	return jetid;
}

