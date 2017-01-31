#ifndef PANDA_PEVENTINFO
#define PANDA_PEVENTINFO

#include <TObject.h>
#include <TClonesArray.h>


namespace panda
{
	class PEvent : public TObject
	{
		public:
			PEvent():
				runNumber(0),
				lumiNumber(0),
				eventNumber(0),
				isData(false),
				npv(0),
				mcWeight(-1),
				rho(0)
				{
					metfilters = new std::vector<bool>;
					tiggers = new std::vector<bool>;
				}
		~PEvent(){ delete metfilters; delete tiggers; }
		
		int runNumber, lumiNumber;
		ULong64_t eventNumber;
		bool isData;
		int npv;
		float mcWeight,rho;
		std::vector<bool> *metfilters, *tiggers;
		ClassDef(PEvent,1)
	};
}
#endif
