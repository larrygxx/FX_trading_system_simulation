#pragma once

#include <map>
#include "Global.h"
#include "Date.h"

//Question 9
namespace minirisk {

    struct FixingDataServer
    {
    public:
        FixingDataServer(const string& filename);

        // queries
        double get(const string& name, const Date& t) const;
        std::pair<double, bool> lookup(const string& name, const Date& t) const;

    private:
        std::map<string, std::map<Date, double>> m_data;

    };

    string mds_spot_name(const string& name);

} // namespace minirisk


#pragma once
