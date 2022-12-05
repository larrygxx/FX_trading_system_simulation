#include "PricerPayment.h"
#include "TradePayment.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"

namespace minirisk {

PricerPayment::PricerPayment(const TradePayment& trd, const std::string& base_ccy)
    : m_amt(trd.quantity())
    , m_dt(trd.delivery_date())
    , m_ir_curve(ir_curve_discount_name(trd.ccy()))
    , m_fx_ccy(trd.ccy() == base_ccy ? "" : fx_spot_name(trd.ccy(), base_ccy))
{
}

double PricerPayment::price(Market& mkt, std::shared_ptr<const FixingDataServer> fds) const
{
    ptr_disc_curve_t disc = mkt.get_discount_curve(m_ir_curve);
    double df = disc->df(m_dt); // this throws an exception if m_dt<today

    //task 6.6
    double fx_spot = 1;

    // This PV is expressed in m_ccy. It must be converted in USD.
    if (!m_fx_ccy.empty()) {
        //task 6.6
        ptr_spot_curve_t fx = mkt.get_spot_curve(fx_spot_prefix + m_fx_ccy);
        fx_spot = fx->spot();
        df *= fx_spot;
    }
    
    return m_amt * df;
}

} // namespace minirisk


