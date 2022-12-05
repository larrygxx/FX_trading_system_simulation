#include <iomanip>

#include "Date.h"

namespace minirisk {

struct DateInitializer : std::array<unsigned, Date::n_years>
{
    DateInitializer()
    {
        for (unsigned i = 0, s = 0, y = Date::first_year; i < size(); ++i, ++y) {
            (*this)[i] = s;
            s += 365 + (Date::is_leap_year(y) ? 1 : 0);
        }
    }
};

const std::array<unsigned, 12> Date::days_in_month = { {31,28,31,30,31,30,31,31,30,31,30,31} };
const std::array<unsigned, 12> Date::days_ytd{ {0,31,59,90,120,151,181,212,243,273,304,334} };
const std::array<unsigned, Date::n_years> Date::days_epoch(static_cast<const std::array<unsigned, Date::n_years>&>(DateInitializer()));

//calculate leap year base on condition
bool Date::is_leap_year(unsigned year)
{
    return ((year % 4 != 0) ? false : (year % 100 != 0) ? true : (year % 400 != 0) ? false : true);
}

// The function pads a zero before the month or day if it has only one digit.
std::string Date::padding_dates(unsigned month_or_day)
{
    std::ostringstream os;
    os << std::setw(2) << std::setfill('0') << month_or_day;
    return os.str();
}

void Date::check_valid(unsigned y, unsigned m, unsigned d)
{
    MYASSERT(y >= first_year, "The year must be no earlier than year " << first_year << ", got " << y);
    MYASSERT(y < last_year, "The year must be smaller than year " << last_year << ", got " << y);
    MYASSERT(m >= 1 && m <= 12, "The month must be a integer between 1 and 12, got " << m);
    unsigned dmax = days_in_month[m - 1] + ((m == 2 && is_leap_year(y)) ? 1 : 0);
    MYASSERT(d >= 1 && d <= dmax, "The day must be a integer between 1 and " << dmax << ", got " << d);
}

long operator-(const Date& d1, const Date& d2)
{
    return  d1.m_serial - d2.m_serial;
}

}



