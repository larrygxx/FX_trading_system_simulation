#pragma once

#include "IPricer.h"
#include "TradePayment.h"

namespace minirisk {

struct PricerPayment : IPricer
{
    //PricerPayment(const TradePayment& trd);
    PricerPayment(const TradePayment& trd, const std::string& base_ccy);
    virtual double price(Market& m, std::shared_ptr<const FixingDataServer> fds) const;

private:
    double m_amt;
    Date   m_dt;
    string m_ir_curve;
    string m_fx_ccy;
};

} // namespace minirisk

