#include "Market.h"
#include "CurveDiscount.h"
#include "CurveFXSpot.h"
#include "CurveFXForward.h"
#include <vector>
#include <limits>

namespace minirisk {

template <typename I, typename T>
std::shared_ptr<const I> Market::get_curve(const string& name)
{
    ptr_curve_t& curve_ptr = m_curves[name];
    if (!curve_ptr.get())
        curve_ptr.reset(new T(this, m_today, name));
    std::shared_ptr<const I> res = std::dynamic_pointer_cast<const I>(curve_ptr);
    MYASSERT(res, "Cannot cast object with name " << name << " to type " << typeid(I).name());
    return res;
}

const ptr_disc_curve_t Market::get_discount_curve(const string& name)
{
    return get_curve<ICurveDiscount, CurveDiscount>(name);
}

const ptr_spot_curve_t Market::get_spot_curve(const string& name)
{
    return get_curve<ICurveFXSpot, CurveFXSpot>(name);
}

const ptr_fwd_curve_t Market::get_fwd_curve(const string& name) {
    return get_curve<ICurveFXForward, CurveFXForward>(name);
}

double Market::from_mds(const string& objtype, const string& name)
{
    auto ins = m_risk_factors.emplace(name, std::numeric_limits<double>::quiet_NaN());
    if (ins.second) { // just inserted, need to be populated
        MYASSERT(m_mds, "Cannot fetch " << objtype << " " << name << " because the market data server has been disconnnected");
        ins.first->second = m_mds->get(name);
    }
    return ins.first->second;
}

const double Market::get_yield(const string& ccyname)
{
    string name(ir_rate_prefix + ccyname);
    return from_mds("yield curve", name);
};

const double Market::get_fx_spot(const string& name)
{
    string ccy1 = name.substr(name.length() - 7, 3);
    string ccy2 = name.substr(name.length() - 3, 3);
    double rate1, rate2;
    ccy1 == "USD" ? (rate1 = 1.0) : rate1 = from_mds("fx spot", "FX.SPOT." + ccy1);
    ccy2 == "USD" ? (rate2 = 1.0) : rate2 = from_mds("fx spot", "FX.SPOT." + ccy2);
    return double(rate1 / rate2);
}

void Market::set_risk_factors(const vec_risk_factor_t& risk_factors)
{
    clear();
    for (const auto& d : risk_factors) {
        auto i = m_risk_factors.find(d.first);
        MYASSERT((i != m_risk_factors.end()), "Risk factor not found " << d.first);
        i->second = d.second;
    }
}

Market::vec_risk_factor_t Market::get_risk_factors(const std::string& expr) const
{
    vec_risk_factor_t result;
    std::regex r(expr);
    for (const auto& d : m_risk_factors)
        if (std::regex_match(d.first, r))
            result.push_back(d);
    return result;
}

//Q4
Market::vec_risk_factor_t Market::retrive_mds_ir(const std::string& expr) 
{
    vec_risk_factor_t ir_result;

    if (m_mds != NULL){
        auto x = m_mds->match(expr);
        for (const auto& it:x) {
                ir_result.push_back(std::make_pair(it, from_mds("curve rate", it)));
    }
    }
    else {
        auto x = get_risk_factors(expr);
        for (const auto& it : x) {
            ir_result.push_back(it);
            //std::cout << it.first;
        }
    }

    return ir_result;
}


} // namespace minirisk
