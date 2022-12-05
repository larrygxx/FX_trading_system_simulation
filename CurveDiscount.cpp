
#include "CurveDiscount.h"
#include "Market.h"
#include "Streamer.h"
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>

namespace minirisk {

CurveDiscount::CurveDiscount(Market *mkt, const Date& today, const string& curve_name)
    : m_today(today)
    , m_name(curve_name)
{
    init_curve_discount(mkt);
}

void CurveDiscount::init_curve_discount(Market* mkt) {
    std::string format = ir_rate_prefix+ "[0-9]+(D|W|M|Y)\\." + m_name.substr(m_name.length() - 3) ;
    //retrive name of the curve first
    std::vector<std::pair<string, double>> rate_map = mkt->retrive_mds_ir(format);
    
    //retrive curve
    for (auto& it : rate_map) {
        auto tenor = (retrive_tenor_unit(it.first.substr(it.first.length()-5, 1)) * std::stoi(it.first.substr(3, it.first.length() - 8)))/365.0;
        auto df = -it.second * tenor;
        m_rate_map.push_back(std::make_pair(tenor, df));
    }
    //adding dates between today and 1w, edge case
    m_rate_map.push_back(std::make_pair(0, 0));
    std::sort(m_rate_map.begin(), m_rate_map.end());
    
}

int CurveDiscount::retrive_tenor_unit(const string& unit) {
    if (unit == "Y") {
        return 365;
    }
    else if (unit == "M") {
        return 30;
    }else if (unit == "W") {
        return 7;
    }
    else {
        return 1;
    }
}

//calculate discount factor
double  CurveDiscount::df(const Date& t) const
{
    double dt = time_frac(m_today, t);

    MYASSERT((!(t < m_today)), "Curve " << m_name << ", DF not available before anchor date " << m_today << ", requested " << t);
    Date last_tenor(m_today.get_serial() + static_cast<unsigned>(365 * m_rate_map.back().first));
    MYASSERT((!(dt > m_rate_map.back().first)), "Curve " << m_name << ", DF not available beyond last tenor date " << last_tenor << ", requested " << t);

    auto it = std::lower_bound(m_rate_map.begin(), m_rate_map.end(), dt, [](const std::pair<double, double>& x,const double& d) { return x.first < d; });
    
    //t == today
    if (it == m_rate_map.begin()) {
        return std::exp(m_rate_map.front().second);
    }
    const auto& hb = *it;
    const auto& lb = *(--it);
    
    double local_r = (lb.second - hb.second) / (hb.first - lb.first);

    return std::exp(lb.second - local_r * (dt - lb.first));
}

} // namespace minirisk
