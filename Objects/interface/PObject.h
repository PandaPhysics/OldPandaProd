#ifndef PANDA_POBJECT
#define PANDA_POBJECT

#include "PandaUtilities/Common/interface/Common.h"
#include <TObject.h>
#include <TLorentzVector.h>


namespace panda
{
	class PObject : public TObject
	{
		public:
			PObject():
				pt(0),
				eta(0),
				phi(0),
				m(0)
				{}
			~PObject(){ delete cachedP4; }

			/**
			 * \brief Return a 4-vector for this object
			 * \param refreshCache if true or if the cache does not exist, the 4-vector will be recomputed
			 *
			 * Builds and caches a TLorentzVector for this object. On next call, the 4-vector
			 * can be re-used
			 */
			TLorentzVector *p4(bool refreshCache=false) {
				if (refreshCache || cachedP4==0) {
					delete cachedP4;
					cachedP4 = new TLorentzVector();
					cachedP4->SetPtEtaPhiM(pt,eta,phi,m);
				}
				return cachedP4;
			}
		
		float pt,eta, phi, m;

		private:
			TLorentzVector *cachedP4=0; //!< cached 4-vector constructed on-demand

		ClassDef(PObject,1)
	};

	/**
	 * \brief Order two PObject pointers by pT
	 */
	inline bool SortPObjects(PObject *o1, PObject *o2) {
		return o1->pt > o2->pt;
	}

}
#endif
