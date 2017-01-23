#ifndef PANDA_PJET
#define PANDA_PJET

#include "PObject.h"
#include "PPFCand.h"
#include <vector>

namespace panda
{
	class PJet : public PObject
	{
		public:
			enum JetID {
				kLoose	 = 1UL<<1,
				kTight	 = 1UL<<2,
				kMonojet = 1UL<<3
			};
			PJet():
				PObject(),
				rawPt(0),
				csv(-1),
				qgl(-1),
				nhf(-1),
				chf(-1),
				constituents(0),
				id(-1)
			{	}
		~PJet(){ delete constituents; }
		
		float rawPt,csv,qgl;
		float nhf,chf;
		std::vector<UShort_t> *constituents; //!< indicies of this jet's constituents in a VPFCand
		unsigned int id;

		/**
		 * \brief Get the PPFCand of this jet corresponding to an index
		 * \param ipf index of the PF candidate
		 * \param VPFCand the PF candidate collection from which this jet was clustered
		 */
		PPFCand *getPFCand(unsigned int ipf, VPFCand *vpf) { return vpf->at(constituents->at(ipf)); }

		ClassDef(PJet,1)
	};

	typedef std::vector<PJet*> VJet;

	inline bool SortPJetByCSV(PJet *o1, PJet *o2) {
		return o1->csv > o2->csv;
	}
}
#endif
