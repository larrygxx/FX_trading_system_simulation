#pragma once
#include <string>
#include "ICurve.h"
#include "Date.h"

using namespace std;

namespace minirisk {

    //create new object CurveFXSpot which implements the interface ICurveFXSpot 

    struct CurveFXSpot : ICurveFXSpot
    {
        //declarations of functions
    public:
        string name() const {
            return m_fxpair;
        }
        Date today() const {
            return m_today;
        }
        double spot() const {
            return m_fxrate;
        }
        CurveFXSpot(Market* mkt, const Date& today, const string& name);

    private:
        Date m_today;
        string m_fxpair;
        double m_fxrate;
    };
}
