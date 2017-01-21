#ifndef PANDAPROD_FILLER_H
#define PANDAPROD_FILLER_H

#include "Includes.h"


#include <vector>
using namespace std;

namespace panda {
class BaseFiller
{
    public:
        virtual int  analyze(const edm::Event &) = 0 ;
        virtual int  analyze(const edm::Event &iEvent,const edm::EventSetup& iSetup) { return analyze(iEvent) ; } ;
        virtual inline string name(){return "BaseFiller";};
				virtual inline string description() { return ""; }
        virtual void endJob() { return; }
        virtual void init(TTree *t) = 0;

        bool *skipEvent=0;
        bool *reduceEvent=0;

        /**
         * \brief Skip the event if the pointer is set and false
         */
        bool SkipEvent() { return ((skipEvent!=0) && (*skipEvent)); }
        /**
         * \brief Reduce the event content if the pointer is set and false
         */
        bool ReduceEvent() { return ((reduceEvent!=0) && (*reduceEvent)); }
        
    protected:

        bool firstEntry=true; //!< used by inherited classes to track whether this is the first time analyze() is called
};
}
#endif
