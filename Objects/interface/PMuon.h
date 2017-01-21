#ifndef PANDA_PMuon
#define PANDA_PMuon

#include "PObject.h"
#include <vector>

namespace panda
{
	class PMuon : public PObject
	{
		public:
			enum MuonID {
				kVeto	 = 1UL<<0,
				kLoose	= 1UL<<1,
				kMedium = 1UL<<2,
				kTight	= 1UL<<3
			};
			PMuon():
				PObject(),
				q(0),
				id(0),
				iso(0),
				chiso(0),
				nhiso(0),
				phoiso(0),
				puiso(0)
			{	}
		~PMuon(){ }
		
		int q;
		unsigned int id;
		float iso, chiso, nhiso, phoiso, puiso;

		ClassDef(PMuon,1)
	};

	typedef std::vector<PMuon*> VMuon;
}
#endif
