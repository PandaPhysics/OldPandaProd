#ifndef PANDA_PJGENET
#define PANDA_PJGENET

#include "PObject.h"
#include <vector>

namespace panda
{
	class PGenJet : public PObject
	{
		public:
			PGenJet():
				PObject(),
				pdgid(-1)
			{	}
		~PGenJet(){ }
		
		int pdgid;

		ClassDef(PGenJet,1)
	};

	typedef std::vector<PGenJet*> VGenJet;
}
#endif
