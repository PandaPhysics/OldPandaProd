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
        virtual void init(TTree *t) = 0;

        bool *skipEvent=0;
};
}
#endif
