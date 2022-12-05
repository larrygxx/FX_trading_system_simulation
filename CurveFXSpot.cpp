#include "Market.h"
#include "CurveFXSpot.h"
using namespace std;

namespace minirisk {
    //constructor for CurveFXSpot
    CurveFXSpot::CurveFXSpot(
        Market* mkt, const Date& today, const string& name)
        : m_today(today), m_fxpair(name), m_fxrate(mkt->get_fx_spot(name)) {}

}

