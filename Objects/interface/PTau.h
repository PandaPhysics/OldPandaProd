#ifndef PANDA_PTau
#define PANDA_PTau

#include "PObject.h"
#include <vector>

namespace panda
{
	class PTau : public PObject
	{
		public:
			enum TauID {
				kBaseline								= 1UL<<0,
				kDecayModeFindingNewDMs	= 1UL<<1,
				kDecayModeFinding				= 1UL<<2
			};
			PTau():
				PObject(),
				q(0),
				id(0),
				iso(0),
				isoDeltaBetaCorr(0)
			{	}
		~PTau(){ }
		
		int q;
		unsigned int id;
		float iso,isoDeltaBetaCorr;

		ClassDef(PTau,1)
	};

	typedef std::vector<PTau*> VTau;
}
#endif
