#include "CurveFXForward.h"
#include "Global.h"
#include "Market.h"

namespace minirisk {
    //implement constructor
    CurveFXForward::CurveFXForward(Market* mkt, const Date& today, const std::string& name)
        : m_today(today)
    {
        m_spot = mkt->get_fx_spot(name);
        string ccy1 = name.substr(name.length() - 7, 3);
        string ccy2 = name.substr(name.length() - 3, 3);
        ccy1_df = mkt->get_discount_curve(ir_curve_discount_name(ccy1));
        ccy2_df = mkt->get_discount_curve(ir_curve_discount_name(ccy2));
    }

    //implement function
    double CurveFXForward::fwd(const Date& t) const {

        auto d1 = ccy1_df->df(t);
        auto d2 = ccy2_df->df(t);

        return m_spot * d1 / d2; //return forward rate from spot rate and discount factors of ccy 1 and ccy2
    }

}

