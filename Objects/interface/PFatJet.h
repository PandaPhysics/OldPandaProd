#ifndef PANDA_PFATJET
#define PANDA_PFATJET

#include "PObject.h"
#include "PPFCand.h"
#include "PJet.h"
#include <tuple>

namespace panda
{

	class PFatJet : public PJet
	{
		public:
			PFatJet():
				PJet(),
				tau1(-1),
				tau2(-1),
				tau3(-1),
				mSD(-1),
				htt_mass(-1),
				htt_frec(-1),
			 subjets(0)
			{ }
		~PFatJet() { for (auto *s : *subjets) delete s; delete subjets; }
		float tau1, tau2, tau3; //!< N-subjettiness
		float mSD, tau1SD=-1, tau2SD=-1, tau3SD=-1; //!< groomed quantities
		float Double_sub=-3;
		float htt_mass, htt_frec; //!< HEPTopTagger quantities
		float ecfs[3][4][4]; //!< (groomed,normalized) energy correlation functions; 
												 // [o-1][N-1][beta] = x
												 // beta = .5, 1, 2, 4

		/**
		 * \brief Get the normalized ECF. Returns -1 if parameters are invalid
		 * \param o_ order, must be 1,2,3
		 * \param N_ N, must be 1,2,3,4
		 * \param ib_ beta index, must be 0,1,2,3
		 */
		float get_ecf(short o_, short N_, int ib_) const {
			if (o_<1 || o_>3 || N_<1 || N_>4 || ib_<0 || ib_>3) 
				return -1;
			return ecfs[o_-1][N_-1][ib_];
		}
		/**
		 * \brief Set the normalized ECF. Returns 1 if ECFN could not be set
		 * \param o_ order, must be 1,2,3
		 * \param N_ N, must be 1,2,3,4
		 * \param ib_ beta index, must be 0,1,2,3
		 */
		int set_ecf(int o_, int N_, int ib_, float x_) {
			if (o_<1 || o_>3 || N_<1 || N_>4 || ib_<0 || ib_>3) 
				return 1;
			ecfs[o_-1][N_-1][ib_] = x_;
			return 0;
		}

		VJet *subjets; //!< vector of subjets, which are really PJet pointers

		ClassDef(PFatJet,1)
		
	};

	typedef std::vector<PFatJet*> VFatJet;
}
#endif
