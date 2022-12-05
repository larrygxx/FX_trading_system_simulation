#include "PricerFXForward.h"
#include "TradePayment.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"
#include "Market.h"

namespace minirisk {

    PricerForward::PricerForward(const TradeFXForward& trd, const std::string& base_ccy)
        : m_amt(trd.quantity())
        , m_strike(trd.strike())
        , m_fixing_date(trd.fixing_date())
        , m_settle_date(trd.settle_date())
        , m_ccy1(trd.ccy1())
        , m_ccy2(trd.ccy2())
        , m_base_ccy(base_ccy) //this is the ccy that we want to see at the output.
    {}
        
    double PricerForward::price(Market& mkt, std::shared_ptr<const FixingDataServer> fds) const
    {
        ptr_disc_curve_t disc = mkt.get_discount_curve(ir_curve_discount_name(m_ccy2));
        double df = disc->df(m_settle_date); // this throws an exception if m_dt<today
        double fwd_rate = -1.0;
        double fx_spot = mkt.get_fx_spot(fx_spot_name(m_ccy2, m_base_ccy));
       
 
        if (fds && mkt.today() >= m_fixing_date) {
            // T0 > T1
            if (mkt.today() > m_fixing_date) {
                fwd_rate = fds->get(fx_spot_name(m_ccy1, m_ccy2), m_fixing_date);
            }
            //T0 == T1
            else {
                const auto& res = fds->lookup(fx_spot_name(m_ccy1, m_ccy2), m_fixing_date);
                if (res.second) {
                    fwd_rate = res.first;
                }
                else {
                    //fds not available
                    fwd_rate = mkt.get_fx_spot(fx_spot_name(m_ccy1, m_ccy2));
                }
            }
        }
        //fwd_rate < 0 means it has not been calculated above, hence we get from fwd curve to interpolate
        if(fwd_rate < 0){
            ptr_fwd_curve_t fwd_curve = mkt.get_fwd_curve(fx_fwd_name(m_ccy1, m_ccy2));
            fwd_rate = fwd_curve->fwd(m_fixing_date);
        }

        if (mkt.today() == m_settle_date) df = 1;

        return m_amt * df * (fwd_rate - m_strike) * fx_spot;
    }

} // namespace minirisk


