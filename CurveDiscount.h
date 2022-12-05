#pragma once
#include <set>
#include <map>
#include <vector>

#include "ICurve.h"
namespace minirisk {

struct Market;

struct CurveDiscount : ICurveDiscount
{
    virtual string name() const { return m_name; }

    CurveDiscount(Market *mkt, const Date& today, const string& curve_name);

    void init_curve_discount(Market* mkt);
    int retrive_tenor_unit(const string& unit);
    // compute the discount factor
    double df(const Date& t) const;

    virtual Date today() const { return m_today; }

private:
    Date   m_today;
    string m_name;
    double m_rate;

    //Q4
    std::vector<std::pair<double, double>> m_rate_map;

};

} // namespace minirisk
