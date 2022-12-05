#pragma once

#include "Macros.h"
#include <string>
#include <array>

namespace minirisk {

struct Date
{
public:
    static const unsigned first_year = 1900;
    static const unsigned last_year = 2200;
    static const unsigned n_years = last_year - first_year;

private:
    static std::string padding_dates(unsigned);

    friend long operator-(const Date& d1, const Date& d2);

    static const std::array<unsigned, 12> days_in_month;  // num of days in month M in a normal year
    static const std::array<unsigned, 12> days_ytd;      // num of days since 1-jan to 1-M in a normal year
    static const std::array<unsigned, n_years> days_epoch;   // num of days since 1-jan-1900 to 1-jan-yyyy (until 2200)

public:
    // Default constructor
    Date() : m_serial(0) {}
    
    // Constructor where the input value is checked.
    Date(unsigned year, unsigned month, unsigned day) { init(year, month, day); }
    
    void init(unsigned year, unsigned month, unsigned day) {
        check_valid(year, month, day);
        
        unsigned m_is_leap = is_leap_year(year);
        m_serial = days_epoch[year - 1900] + days_ytd[month - 1] + ((month > 2 && m_is_leap) ? 1 : 0) + (day - 1);
    }
    
    Date(unsigned l) { init(l); }

    void init(unsigned l) { m_serial = l; }

    std::string to_string(bool pretty = true) const {

        int m_y = 1900;
        int m_m = 1;
        int m_d = 1;
        int m_is_leap = false;
        int tmp = m_serial;

        tmp += 1;
        while (tmp > (365 + m_is_leap)) {
            tmp -= (365 + m_is_leap);
            m_y += 1;
            m_is_leap = is_leap_year(m_y);
        }
        tmp -= 1;

        while (tmp > static_cast<int>(days_ytd[m_m]) - 1 + (((m_is_leap) && (m_m >= 2)) ? 1 : 0)) {
            m_m += 1;
            if (m_m == 12) { break; }
        }

        m_d = tmp - days_ytd[m_m - 1] + 1 - ((m_is_leap && m_m > 2) ? 1 : 0);

        return (pretty
            ? std::to_string((int)m_d) + "-" + std::to_string((int)m_m) + "-" + std::to_string(m_y)
            : std::to_string(m_y) + padding_dates((int)m_m) + padding_dates((int)m_d));

    }

    static void check_valid(unsigned y, unsigned m, unsigned d);

    //operator overload
    bool operator<(const Date& d) const { return m_serial < d.m_serial;}
    
    bool operator==(const Date& d) const { return m_serial == d.m_serial;}

    bool operator>=(const Date& d) const { return m_serial >= d.m_serial; }

    bool operator<=(const Date& d) const { return m_serial <= d.m_serial; }

    bool operator>(const Date& d) const { return m_serial > d.m_serial; }

    unsigned get_serial() const { return m_serial; }
    
    static bool is_leap_year(unsigned yr);

private:
    unsigned int m_serial;

};

long operator-(const Date& d1, const Date& d2);

inline double time_frac(const Date& d1, const Date& d2)
{
    return static_cast<double>(d2 - d1) / 365.0;
}

} // namespace minirisk
