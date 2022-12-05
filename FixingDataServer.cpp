#include "FixingDataServer.h"
#include <fstream>
#include "Macros.h"
#include "Global.h"
#include "Streamer.h"
#include <limits>

//Question 9
namespace minirisk {

    FixingDataServer::FixingDataServer(const std::string& filename) {
        std::ifstream is(filename);
        MYASSERT(!is.fail(), "Could not open file " << filename);
        do {
            string name;
            string date;
            double value;
            is >> name >> date >> value;
            if (name == "") break;
            //std::cout << name << " " << date << " " << value << "\n";
            int year = std::atoi(date.substr(0, 4).c_str());
            int month = std::atoi(date.substr(4, 2).c_str());
            int day = std::atoi(date.substr(6, 2).c_str());

            m_data.emplace(name, std::map<Date, double>());
            auto ins = m_data[name].emplace(Date(year, month, day), value);
            MYASSERT(ins.second, "Duplicated risk factor: " << name << " " << date << " " << value);

        } while (is);

    }


    double FixingDataServer::get(const string& name, const Date& t) const {

        // find data according to name
        auto name_iter = m_data.find(name);
        MYASSERT(name_iter != m_data.end(), "Fixing not found: " << name << "," << t);

        //find data according to data
        auto date_iter = name_iter->second.find(t);
        MYASSERT(date_iter != name_iter->second.end(), "Fixing not found: " << name << "," << t);
        return date_iter->second;

    }

    //lookup according to the right name and date
    std::pair<double, bool> FixingDataServer::lookup(const string& name, const Date& t) const {
        auto name_iter = m_data.find(name);
        if (name_iter != m_data.end())
        {
            auto date_iter = name_iter->second.find(t);

            if (date_iter != name_iter->second.end())
                return std::make_pair(date_iter->second, true);
        }
        return std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
    }

} // namespace minirisk